#ifndef _GAME_H_
#define _GAME_H_

#include "Option.h"

#define DIR_UP      1
#define DIR_DOWN    2
#define DIR_LEFT    3
#define DIR_RIGHT   4

int GameSave(LPOPTION lpOption);
int GameLoad(LPOPTION lpOption);
int GamePause(LPOPTION lpOption);
int GameDirKey(int dir,LPOPTION lpOption);
int GameInit(LPOPTION lpOption,int w,int h);
//int GameCreatNewBlock(LPOPTION lpOption);

/*
Clac the numb of maps blank tile
Notice:
DO NOT USE for your AI, write a new one for yourself!
*******************************************************************************/
//int CheckBlank(const int (*map)[5]);

/*
Check have two same tiles near or not
return num of those tiles, use of check gameover
Notice:
DO NOT USE for your AI!
*******************************************************************************/
//int CheckALLDirNearby(const int (*map)[5],int w,int h,int dir);

/*
Check can move to this direction or not
return 0 or 1 and a new map
Notic:
for AI only!
Copy it for you AI, DO NOT direct use it!
*******************************************************************************/

/*
#define GameBlockUnite(F,S) if(*S != 0){                    \
                                if(*F == 0){                \
                                    *F = *S;                \
                                    *S = 0;                 \
                                    fHadmove = 1;           \
                                }                           \
                                else if(*F == *S){          \
                                    *F = *F+1;              \
                                    *S = 0;                 \
                                    fHadmove = 1;           \
                                    fUniting = 1;           \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }

static int AICheckIfDir(int (*map)[5],int w,int h,int dir){
    int fHadmove = 0;
    int fUniting = 0;
    switch(dir){
    case DIR_UP:
        forp(i,w){
            for(int j=0;j<h-1;j++){
                int *F = &map[j][i];
                int *S = NULL;

                for(int k=j+1;k<h;k++){
                    S = &map[k][i];
                    //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",j,i,map[j][i],k,i,map[k][i]);
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_DOWN:
        forp(y,w){
            for(int x=h-1;x>0;x--){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=x-1;i>=0;i--){
                    S = &map[i][y];
                    //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_LEFT:
        forp(x,h){
            for(int y=0;y<w-1;y++){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y+1;i<w;i++){
                    S = &map[x][i];
                    //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_RIGHT:
        forp(x,h){
            for(int y=w-1;y>0;y--){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y-1;i>=0;i--){
                    S = &map[x][i];
                    //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    }
    return fHadmove;
}
*/



#endif