#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Macro.h"
#include "Game.h"

/*******************************************************************************
AI beigin
*******************************************************************************/
#define MAX 255
#define max(a,b)    ((a)>(b)?(a):(b))
#define min(a,b)    ((a)<(b)?(a):(b))
#define unif_random(x)  (rand()%x)

typedef uint64_t board_t;
typedef uint16_t row_t;

typedef struct trans_table_entry_t{
    uint8_t depth;
    float heuristic;
}trans_table_entry_t;

static const board_t ROW_MASK = 0xFFFFULL;
static const board_t COL_MASK = 0x000F000F000F000FULL;

static board_t unpack_col(row_t row) {
    board_t tmp = row;
    return (tmp | (tmp << 12ULL) | (tmp << 24ULL) | (tmp << 36ULL)) & COL_MASK;
}

static row_t reverse_row(row_t row) {
    return (row >> 12) | ((row >> 4) & 0x00F0)  | ((row << 4) & 0x0F00) | (row << 12);
}

static void print_board(board_t board) {
    int i,j;
    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            uint8_t powerVal = (board)& 0xF;
            printf("%-4.d",powerVal);
            board >>= 4;
        }
        printf("\n");
    }
    printf("\n");
}

typedef struct trans_table_t{
    int     id_;
    board_t board_;
    trans_table_entry_t trans_table_entry_;
}trans_table_t;

typedef struct eval_state {
    trans_table_t trans_table[MAX]; // transposition table, to cache previously-seen moves
    int maxdepth;
    int curdepth;
    int cachehits;
    unsigned long moves_evaled;
    int depth_limit;
}eval_state;


static board_t transpose(board_t x)
{
    board_t a1 = x & 0xF0F00F0FF0F00F0FULL;
    board_t a2 = x & 0x0000F0F00000F0F0ULL;
    board_t a3 = x & 0x0F0F00000F0F0000ULL;
    board_t a = a1 | (a2 << 12) | (a3 >> 12);
    board_t b1 = a & 0xFF00FF0000FF00FFULL;
    board_t b2 = a & 0x00FF00FF00000000ULL;
    board_t b3 = a & 0x00000000FF00FF00ULL;
    return b1 | (b2 >> 24) | (b3 << 24);
}

static int count_empty(board_t x)
{
    x |= (x >> 2) & 0x3333333333333333ULL;
    x |= (x >> 1);
    x = ~x & 0x1111111111111111ULL;
    x += x >> 32;
    x += x >> 16;
    x += x >>  8;
    x += x >>  4;
    return x & 0xf;
}

static row_t row_left_table[65536];
static row_t row_right_table[65536];
static board_t col_up_table[65536];
static board_t col_down_table[65536];
static float heur_score_table[65536];
static float score_table[65536];

// Heuristic scoring settings
static const float SCORE_LOST_PENALTY = 200000.0f;
static const float SCORE_MONOTONICITY_POWER = 4.0f;
static const float SCORE_MONOTONICITY_WEIGHT = 47.0f;
static const float SCORE_SUM_POWER = 3.5f;
static const float SCORE_SUM_WEIGHT = 11.0f;
static const float SCORE_MERGES_WEIGHT = 700.0f;
static const float SCORE_EMPTY_WEIGHT = 270.0f;

void init_tables() {
    for (unsigned row = 0; row < 65536; ++row) {
        unsigned line[4] = {
                (row >>  0) & 0xf,
                (row >>  4) & 0xf,
                (row >>  8) & 0xf,
                (row >> 12) & 0xf
        };

        // Score
        float score = 0.0f;
        for (int i = 0; i < 4; ++i) {
            int rank = line[i];
            if (rank >= 2) {
                // the score is the total sum of the tile and all intermediate merged tiles
                score += (rank - 1) * (1 << rank);
            }
        }
        score_table[row] = score;


        // Heuristic score
        float sum = 0;
        int empty = 0;
        int merges = 0;

        int prev = 0;
        int counter = 0;
        for (int i = 0; i < 4; ++i) {
            int rank = line[i];
            sum += pow(rank, SCORE_SUM_POWER);
            if (rank == 0) {
                empty++;
            } else {
                if (prev == rank) {
                    counter++;
                } else if (counter > 0) {
                    merges += 1 + counter;
                    counter = 0;
                }
                prev = rank;
            }
        }
        if (counter > 0) {
            merges += 1 + counter;
        }

        float monotonicity_left = 0;
        float monotonicity_right = 0;
        for (int i = 1; i < 4; ++i) {
            if (line[i-1] > line[i]) {
                monotonicity_left += pow(line[i-1], SCORE_MONOTONICITY_POWER) - pow(line[i], SCORE_MONOTONICITY_POWER);
            } else {
                monotonicity_right += pow(line[i], SCORE_MONOTONICITY_POWER) - pow(line[i-1], SCORE_MONOTONICITY_POWER);
            }
        }

        heur_score_table[row] = SCORE_LOST_PENALTY +
            SCORE_EMPTY_WEIGHT * empty +
            SCORE_MERGES_WEIGHT * merges -
            SCORE_MONOTONICITY_WEIGHT * min(monotonicity_left, monotonicity_right) -
            SCORE_SUM_WEIGHT * sum;

        // execute a move to the left
        for (int i = 0; i < 3; ++i) {
            int j;
            for (j = i + 1; j < 4; ++j) {
                if (line[j] != 0) break;
            }
            if (j == 4) break; // no more tiles to the right

            if (line[i] == 0) {
                line[i] = line[j];
                line[j] = 0;
                i--; // retry this entry
            } else if (line[i] == line[j]) {
                if(line[i] != 0xf) {
                    /* Pretend that 32768 + 32768 = 32768 (representational limit). */
                    line[i]++;
                }
                line[j] = 0;
            }
        }

        row_t result = (line[0] <<  0) |
                       (line[1] <<  4) |
                       (line[2] <<  8) |
                       (line[3] << 12);
        row_t rev_result = reverse_row(result);
        unsigned rev_row = reverse_row(row);

        row_left_table [    row] =                row  ^                result;
        row_right_table[rev_row] =            rev_row  ^            rev_result;
        col_up_table   [    row] = unpack_col(    row) ^ unpack_col(    result);
        col_down_table [rev_row] = unpack_col(rev_row) ^ unpack_col(rev_result);
    }
}

static board_t execute_move_0(board_t board) {
    board_t ret = board;
    board_t t = transpose(board);
    ret ^= col_up_table[(t >>  0) & ROW_MASK] <<  0;
    ret ^= col_up_table[(t >> 16) & ROW_MASK] <<  4;
    ret ^= col_up_table[(t >> 32) & ROW_MASK] <<  8;
    ret ^= col_up_table[(t >> 48) & ROW_MASK] << 12;
    return ret;
}

static board_t execute_move_1(board_t board) {
    board_t ret = board;
    board_t t = transpose(board);
    ret ^= col_down_table[(t >>  0) & ROW_MASK] <<  0;
    ret ^= col_down_table[(t >> 16) & ROW_MASK] <<  4;
    ret ^= col_down_table[(t >> 32) & ROW_MASK] <<  8;
    ret ^= col_down_table[(t >> 48) & ROW_MASK] << 12;
    return ret;
}

static board_t execute_move_2(board_t board) {
    board_t ret = board;
    ret ^= (board_t)(row_left_table[(board >>  0) & ROW_MASK]) <<  0;
    ret ^= (board_t)(row_left_table[(board >> 16) & ROW_MASK]) << 16;
    ret ^= (board_t)(row_left_table[(board >> 32) & ROW_MASK]) << 32;
    ret ^= (board_t)(row_left_table[(board >> 48) & ROW_MASK]) << 48;
    return ret;
}

static board_t execute_move_3(board_t board) {
    board_t ret = board;
    ret ^= (board_t)(row_right_table[(board >>  0) & ROW_MASK]) <<  0;
    ret ^= (board_t)(row_right_table[(board >> 16) & ROW_MASK]) << 16;
    ret ^= (board_t)(row_right_table[(board >> 32) & ROW_MASK]) << 32;
    ret ^= (board_t)(row_right_table[(board >> 48) & ROW_MASK]) << 48;
    return ret;
}

/* Execute a move. */
static board_t execute_move(int move,board_t board) {
    switch(move) {
    case 0: // up
        return execute_move_0(board);
    case 1: // down
        return execute_move_1(board);
    case 2: // left
        return execute_move_2(board);
    case 3: // right
        return execute_move_3(board);
    default:
        return ~0ULL;
    }
}

static int get_max_rank(board_t board) {
    int maxrank = 0;
    while(board) {
        maxrank = max(maxrank,(int)(board & 0xf));
        board >>= 4;
    }
    return maxrank;
}

static int count_distinct_tiles(board_t board) {
    uint16_t bitset = 0;
    while(board) {
        bitset |= 1<<(board & 0xf);
        board >>= 4;
    }

    // Don't count empty tiles.
    bitset >>= 1;

    int count = 0;
    while(bitset) {
        bitset &= bitset - 1;
        count++;
    }
    return count;
}


// score a single board heuristically
static float score_heur_board(board_t board);
// score a single board actually (adding in the score from spawned 4 tiles)
static float score_board(board_t board);
// score over all possible moves
static float score_move_node(eval_state *state,board_t board,float cprob);
// score over all possible tile choices and placements
static float score_tilechoose_node(eval_state *state,board_t board,float cprob);

static float score_helper(board_t board,const float* table) {
    return table[(board >>  0) & ROW_MASK] +
        table[(board >> 16) & ROW_MASK] +
        table[(board >> 32) & ROW_MASK] +
        table[(board >> 48) & ROW_MASK];
}

static float score_heur_board(board_t board) {
    return score_helper(board,heur_score_table) +
        score_helper(transpose(board),heur_score_table);
}

static float score_board(board_t board) {
    return score_helper(board,score_table);
}

static const float CPROB_THRESH_BASE = 0.0001f;
static const int CACHE_DEPTH_LIMIT  = 15;

static float score_tilechoose_node(eval_state *state,board_t board,float cprob) {
    if( cprob < CPROB_THRESH_BASE || state->curdepth >= state->depth_limit)
    {
        state->maxdepth = max(state->curdepth,state->maxdepth);
        return score_heur_board(board);
    }
    if(state->curdepth < CACHE_DEPTH_LIMIT) {

        trans_table_entry_t entry;
        int i = 0;
        for(i=0;i<MAX;i++){
            if( state->trans_table[i].board_ == board){
                entry = state->trans_table[i].trans_table_entry_;
                break;
            }
        }

        if(i < MAX) {
            if(entry.depth <= state->curdepth)
            {
                state->cachehits++;
                return entry.heuristic;
            }
        }
    }

    int num_open = count_empty(board);
    cprob /= num_open;

    float res = 0.0f;
    board_t tmp = board;
    board_t tile_2 = 1;
    while(tile_2) {
        if((tmp & 0xf) == 0) {
            res += score_move_node(state,board |  tile_2,cprob * 0.9f) * 0.9f;
            res += score_move_node(state,board | (tile_2 << 1),cprob * 0.1f) * 0.1f;
        }
        tmp >>= 4;
        tile_2 <<= 4;
    }
    res = res / num_open;

    if(state->curdepth < CACHE_DEPTH_LIMIT) {
        int i = 0;
        for(i=0;i<MAX;i++){
            if(state->trans_table[i].board_ == board){
                break;
            }
        }
        state->trans_table[i].trans_table_entry_.depth = state->curdepth;
        state->trans_table[i].trans_table_entry_.heuristic = res;
    }

    return res;
}

static float score_move_node(eval_state *state,board_t board,float cprob) {
    float best = 0.0f;
    state->curdepth++;
    for(int move = 0; move < 4; ++move) {
        board_t newboard = execute_move(move,board);
        state->moves_evaled++;

        if(board != newboard) {
            float a = score_tilechoose_node(state,newboard,cprob);
            best = max(best,a);
        }
    }
    state->curdepth--;

    return best;
}

static float _score_toplevel_move(eval_state *state,board_t board,int move) {
    //int maxrank = get_max_rank(board);
    board_t newboard = execute_move(move,board);

    if(board == newboard)
        return 0;

    return score_tilechoose_node(state,newboard,1.0f) + 1e-6;
}

float score_toplevel_move(board_t board, int move) {
    float res = {0};
    eval_state xstate = {0};
    eval_state * state = &xstate;

    state->depth_limit = max(3,count_distinct_tiles(board) - 2);

    res = _score_toplevel_move(state,board,move);

    return res;
}

int find_best_move(board_t board) {
    int move;
    float best = 0;
    int bestmove = -1;

    //print_board(board);

    for(move=0; move<4; move++) {
        float res = score_toplevel_move(board,move);

        if(res > best) {
            best = res;
            bestmove = move;
        }
    }

    return bestmove;
}

/*******************************************************************************
AI end
*******************************************************************************/
board_t AIMaptoBoard(int map[5][5]){
    // TODO:
    board_t x = ((board_t)(map[3][3]) << 60) |
                ((board_t)(map[3][2]) << 56) |
                ((board_t)(map[3][1]) << 52) |
                ((board_t)(map[3][0]) << 48) |
                ((board_t)(map[2][3]) << 44) |
                ((board_t)(map[2][2]) << 40) |
                ((board_t)(map[2][1]) << 36) |
                ((board_t)(map[2][0]) << 32) |
                ((board_t)(map[1][3]) << 28) |
                ((board_t)(map[1][2]) << 24) |
                ((board_t)(map[1][1]) << 20) |
                ((board_t)(map[1][0]) << 16) |
                ((board_t)(map[0][3]) << 12) |
                ((board_t)(map[0][2]) << 8 ) |
                ((board_t)(map[0][1]) << 4 ) |
                ((board_t)(map[0][0])      ) ;
    return x;
}

int AI4(int map[5][5],int w,int h){
    static int fstart = 0;
    if(!fstart){
        init_tables();
        fstart = 1;
    }

    board_t b = AIMaptoBoard(map);

    for(int y=0;y<4;y++)
    {
        debug("%-4.d%-4.d%-4.d%-4.d",map[y][0],map[y][1],map[y][2],map[y][3]);
    }

    int dir = find_best_move(b);
    dir += 1;

    return dir;
}