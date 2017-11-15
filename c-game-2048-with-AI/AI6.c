#include <assert.h>
#include "Macro.h"
#include "Game.h"

static enum{
    // normal rule
    WEIGHT_BLANK,
    WEIGHT_BIG_NEIGHBOR,
    WEIGHT_NEIGHBOR2,
    WEIGHT_NEIGHBOR_NUM,
    WEIGHT_MAXNUM,
    WEIGHT_MERGE,
    WEIGHT_MERGE_NUM,

    WEIGHT_BIG_BLOCK,

    WEIGHT_SMALL_AROUND,
    WEIGHT_SMALL_AROUND2,
    WEIGHT_SMALL_AROUND3,
    WEIGHT_SMALL_WAY,

    WEIGHT_DIE_END,
    WEIGHT_FIND_WAY,
    WEIGHT_ONLY_ONE_NUM,
    WEIGHT_NEAR_BLANK,

    WEIGHT_END
};

static char *WeightTableName[] = {
    "blank",
    "big neighbor",
    "neighbor2",
    "neighbor num",
    "maxnum",
    "merge",
    "merge num",

    "big block",

    "small around",
    "small around2",
    "small around3",
    "small way",

    "die end",
    "have a way",
    "only one same",
    "near blank",
};

#include "AI.h"

#define AICheckInRegion(x,h)        ((x)<(h) && (x)>=0)
#define AICheckInRegionXY(x,y,w,h)  ((x)<(h) && (x)>=0 && (y)<(w) && (y)>=0)

#undef max
#undef min

static int AIMax(int a,int b){
    return a>b?a:b;
}

static int AIMin(int a,int b){
    return a<b?a:b;
}

static int AIIsHaveNum(int (*map)[5],int w,int h,int num){
    forp(x,h)forp(y,w){
        if(map[x][y] == num)return 1;
    }
    return 0;
}

static int AIFindMaxNum(const int (*map)[5],int w,int h){
    int max = 0;
    forp(x,h)forp(y,w){
        if(map[x][y] > max){
            max = map[x][y];
        }
    }
    return max;
}

static int AIFindALLNumXY(const int(*map)[5],int w,int h,int *px,int *py,int num){
    int i = 0;
    forp(x,h)forp(y,w){
        if(map[x][y] == num){
            num = map[x][y];
            px[i] = x;
            py[i] = y;
            i++;
        }
    }
    return i;
}

static float AICheckBigNeighbor(const int(*map)[5],int w,int h){
    float sum = 0;
    int max = AIFindMaxNum(map,w,h);
    forp(x,h)forp(y,w){
        if(map[x][y]){
            int fHave = 0;
            fHave += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y]) ? 1 : 0;
            fHave += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y]) ? 1 : 0;
            fHave += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y]) ? 1 : 0;
            fHave += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y]) ? 1 : 0;
            //if(fHave)debug("[%d,%d] %g",x,y,sum);
            sum += fHave>0 ? map[x][y]>max/2 ? map[x][y]-max/2 : 1 : 0;
        }
    }
    return sum/2;
}

static float AICheckBigNeighbor3(const int(*map)[5],int w,int h){
    float sum = 0;
    forp(x,h)forp(y,w){
        if(map[x][y]){
            int fHave = 0;
            int max = AIFindMaxNum(map,w,h);
            fHave += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y]) ? 1 : 0;
            fHave += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y]) ? 1 : 0;
            fHave += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y]) ? 1 : 0;
            fHave += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y]) ? 1 : 0;
            //if(fHave)debug("[%d,%d] %g",x,y,sum);
            sum += fHave>0 ? map[x][y]<max/2-1 ? max/2-1-map[x][y] : 1 : 0;
            //sum += fHave>0 ? map[x][y] : 0;
            if(map[x][y] == 10 && fHave>0)sum+=10000;
        }
    }
    return sum/2;
}

static int AICheckAllNeighbor2_helper(const int(*map)[5],int w,int h,int x,int y){
    int sum = 0;
    sum += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y]) ? 1 : 0;
    return sum>0 ? map[x][y]:0;
}
static int AICheckAllNeighbor2(const int(*map)[5],int w,int h){
    int sum = 0;
    int max = AIFindMaxNum(map,w,h);
    forp(x,h)forp(y,w){
        if(map[x][y]!=0 /*&& map[x][y]>2*/){
            int v = AICheckAllNeighbor2_helper(map,w,h,x,y);
            sum += max-v;
            //if(v)debug("[%d,%d]",x,y);
        }
    }
    //debug("-");
    return sum;
}

static int AICheckAllNeighborNum_helper(const int(*map)[5],int w,int h,int x,int y){
    int sum = 0;
    sum += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y]) ? 1 : 0;
    return sum>0 ? 1 : 0;
}
static int AICheckAllNeighborNum(const int(*map)[5],int w,int h){
    int sum = 0;
    forp(x,h)forp(y,w){
        if(map[x][y]!=0 /*&& map[x][y]>2*/){
            int v = AICheckAllNeighborNum_helper(map,w,h,x,y);
            sum += v;
            //if(v)debug("[%d,%d]",x,y);
        }
    }
    //debug("-");
    return sum;
}

#define AICheckALLBoard(x,y,w,h)    forp(x,w)forp(y,h)
#define AICheckAround(x,y,w,h)      for(int i=-1;i<=1;i++)for(int j=-1;j<=1;j++)if(AICheckInRegionXY(x+i,y+j,w,h) \
        && !(i==0&&j==0) && !(i==1&&j==1) && !(i==-1&&j==-1) && !(i==-1&&j==1) && !(i==1&&j==-1))

static int AICheckDieEndNum(const int(*map)[5],int w,int h){
    int sum = 0;
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] == 0){
            int fIsDieEnd = 1;
            AICheckAround(x,y,w,h){
                if(map[x+i][y+j] == 2){
                    fIsDieEnd = 0;
                }
            }
            int fHaveBlank = 0;
            AICheckAround(x,y,w,h){
                if(map[x+i][y+j] == 0){
                    fHaveBlank++;
                }
            }
            if(fHaveBlank >= 2){
                fIsDieEnd = 0;
            }
            if(fIsDieEnd){
                sum++;
            }
        }
    }
    //debug("--");
    return sum;
}

static float AICheckDieEnd(const int(*map)[5],int w,int h){
    int v = AICheckDieEndNum(map,w,h);
    return -v;
}

static float AICheckOnlyOneNum(const int(*map)[5],int w,int h){
    int max = AIFindMaxNum(map,w,h);
    float sum = 0;
    int board[16] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,-2,-1};
    int finder[16] = {0};
    AICheckALLBoard(x,y,w,h){
        forp(k,16){
            if(map[x][y] == board[k]){
                if(finder[k] > (max/2-board[k])){
                    //debug("[%d,%d]",x,y);
                    int fNear = 0;
                    int fSim  = 0;
                    AICheckAround(x,y,w,h){
                        if(map[x][y] == map[x+i][y+j]){
                            fNear = 1;
                        }
                        if(map[x][y] == map[x+i][y+j]-1){
                            fSim = 1;
                        }
                    }
                    if(!fNear){
                        sum -= (max-board[k])*1.5+finder[k];
                        finder[k] += 1;
                    }
                    if(fSim){
                        //sum += max-board[k];
                    }
                }
                else{
                    finder[k] += 1;
                }
            }
        }
    }
    //debug("%d",sum);
    return sum;
}

/*******************************************************************************
如果發生了卡位的現象，則扣分
*******************************************************************************/
static float AICheckBigBlock(const int(*map)[5],int w,int h){
    float sum = 0;

    int tmp = AIFindMaxNum(map,w,h);
    const int max = AIFindMaxNum(map,w,h);
    int X[20],Y[20];
    int len = 0;
    const int blank = AICheckBlank(map,w,h);

    //while(len < 16) {
    while(len < (16-blank)/2) {
        if(tmp < max/2)break;
        //if(tmp < 1)break;
        len += AIFindALLNumXY(map,w,h,&X[len],&Y[len],tmp--);
    }

    forp(i,len){
        const int x = X[i],y = Y[i];
        const int this = map[x][y];
        float fBlock = 0;

        if(AICheckInRegionXY(x-1,y,w,h) && AICheckInRegionXY(x+1,y,w,h)){
            int one = -1,two = -1;
            
            if(map[x-1][y]<map[x][y] && map[x+1][y]<map[x][y]){
                one = map[x+1][y];
                two = map[x-1][y];
            }
            elif(AICheckInRegionXY(x-2,y,w,h) && map[x-1][y]==map[x][y] && map[x-2][y]<map[x][y] && map[x+1][y]<map[x][y]){
                one = map[x+1][y];
                one = map[x-2][y];
            }
            elif(AICheckInRegionXY(x+2,y,w,h) && map[x+1][y]==map[x][y] && map[x+2][y]<map[x][y] && map[x-1][y]<map[x][y]){
                one = map[x+2][y];
                two = map[x-1][y];
            }

            if(one != -1 && two != -1){
                fBlock += 4+max*2;

            }
        }
        if(AICheckInRegionXY(x,y-1,w,h) && AICheckInRegionXY(x,y+1,w,h)){
            int one = -1,two = -1;

            if(map[x][y-1]<map[x][y] && map[x][y+1]<map[x][y]){
                one = map[x][y+1];
                two = map[x][y-1];
            }
            elif(AICheckInRegionXY(x,y-2,w,h) && map[x][y-1]==map[x][y] && map[x][y-2]<map[x][y] && map[x][y+1]<map[x][y]){
                one = map[x][y+1];
                two = map[x][y-2];
            }
            elif(AICheckInRegionXY(x,y+2,w,h) && map[x][y+1]==map[x][y] && map[x][y+2]<map[x][y] && map[x][y-1]<map[x][y]){
                one = map[x][y+2];
                two = map[x][y-1];
            }

            if(one != -1 && two != -1){
                fBlock += 4+max*2;
            }
        }
        if(fBlock)fBlock += this*2;
        //if(fBlock)debug("[%d,%d](%d) %g",x,y,map[x][y],fBlock);
        sum -= fBlock;
    }
    return sum;
}

static int AICheckSmallNumAround_helper(const int(*map)[5],int w,int h,int x,int y){
    int sum = 0;
    int res = 4;
    int tmp = 4;
    int v   = map[x][y];

    for(int i=-4;i<=4;i++){
        if(i==0)continue;
        int w = map[x+i][y];
        if(w==v || w==v-1 || w==v-2){
            tmp = i>0 ? i : -i;
        }
    }
    for(int j=-4;j<=4;j++){
        if(j==0)continue;
        int w = map[x][y+j];
        if(w==v || w==v-1 || w==v-2){
            res = j>0 ? j : -j;
        }
    }

    return res>tmp ? tmp : res;
}

/*******************************************************************************
找小數的周圍，如果該小數位置不好，則扣分
*******************************************************************************/
static int AICheckSmallNumAround(const int(*map)[5],int w,int h){
    int sum = 0;
    int max = AIFindMaxNum(map,w,h);
    //debug("%s:(%d)",__FUNCTION__,max/2);
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] == 1 /*|| map[x][y] == 2*/){
            int fHaveBlank = 0;
            AICheckAround(x,y,w,h){
                // 周圍有空白
                if(map[x+i][y+j]==map[x][y] || map[x+i][y+j]==0){
                    fHaveBlank = 1;
                }
            }

            if(!fHaveBlank){
                sum -= AICheckSmallNumAround_helper(map,w,h,x,y);
            }

        }
    }
    //debug("--");
    return sum;
}

static int AICheckSmallNumAround2(const int(*map)[5],int w,int h){
    int sum = 0;
    int max = AIFindMaxNum(map,w,h);
    //debug("%s:(%d)",__FUNCTION__,max/2);
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] == 1 || map[x][y] == 2){
            int fHaveBlank = 0;
            AICheckAround(x,y,w,h){
                // 周圍有空白
                if(map[x+i][y+j]==map[x][y] || map[x+i][y+j]==map[x][y]-1 || map[x+i][y+j]==map[x][y]-2){
                    fHaveBlank = 1;
                }
            }

            if(!fHaveBlank){
                int maxnum = 0;
                AICheckAround(x,y,w,h){
                    if(map[x+i][y+j] > maxnum){
                        maxnum = map[x+i][y+j];
                    }
                }
                //debug("[%d,%d] %d",x,y,maxnum);
                //debug("[%d,%d]%d %d",x,y,map[x][y],maxnum);
                sum -= maxnum;
            }
        }
    }
    //debug("--");
    return sum;
}

static int AICheckSmallNearBlank_helper_(const int(*map)[5],int w,int h,int x,int y,int finder[5][5]){
    int sum = 0;
    AICheckAround(x,y,w,h){
        if(!finder[x+i][y+j] && map[x+i][y+j]==0){
            finder[x+i][y+j] = 1;
            sum += 1 + AICheckSmallNearBlank_helper_(map,w,h,x+i,y+j,finder);
        }
    }
    return sum;
}

static int AICheckSmallNearBlank_helper(const int(*map)[5],int w,int h,int x,int y){
    int finder[5][5] = {0};
    int sum = 0;
    AICheckAround(x,y,w,h){
        if(!finder[x+i][y+j] && map[x+i][y+j]==0){
            finder[x+i][y+j] = 1;
            sum += 1 + AICheckSmallNearBlank_helper_(map,w,h,x+i,y+j,finder);
        }
    }
    return sum;
}

static int AICheckSmallNearBlank(const int(*map)[5],int w,int h){
    int sum = 0;
    int max = AIFindMaxNum(map,w,h);
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] == 1){
            int fHaveSame = 0;
            int fHaveFather = 0;
            AICheckAround(x,y,w,h){
                if(map[x+i][y+j]==map[x][y]){
                    fHaveSame = 1;
                }
                if(map[x+i][y+j]==map[x][y]+1){
                    fHaveFather = 1;
                }
            }
            if(fHaveSame)continue;
            if(fHaveFather){
                int blank = AICheckSmallNearBlank_helper(map,w,h,x,y);
                if(blank < 2){
                    //
                }else{
                    sum += blank;
                }
                continue;
            }
            sum -= max-map[x][y];
        }
        if(map[x][y] == 2){
            int fHaveSame = 0;
            int fHaveFather = 0;
            AICheckAround(x,y,w,h){
                if(map[x+i][y+j]==map[x][y] || map[x+i][y+j]==map[x][y]-1){
                    fHaveSame = 1;
                }
                if(map[x+i][y+j]==map[x][y]+1){
                    fHaveFather = 1;
                }
            }
            if(fHaveSame)continue;
            if(fHaveFather){
                int blank = AICheckSmallNearBlank_helper(map,w,h,x,y);
                if(blank < 3){
                    //
                }
                else{
                    sum += blank;
                }
                continue;
            }
            sum -= max-map[x][y];
        }
    }
    return sum;
}

static int AICheckSmallNumAround3(const int(*map)[5],int w,int h){
    int sum = 0;
    int max = AIFindMaxNum(map,w,h);
    //debug("%s:(%d)",__FUNCTION__,max/2);
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] == 1 || map[x][y] == 2){
            int fFit = 0;
            AICheckAround(x,y,w,h){
                // 周圍有合適的格子
                if(map[x+i][y+j]==map[x][y] || map[x+i][y+j]==map[x][y]-1 || map[x+i][y+j]==map[x][y]-2){
                    fFit = 1;
                }
            }

            if(fFit){
                sum += map[x][y];
            }else{
                sum -= max/2-map[x][y];
            }

        }
    }
    //debug("--");
    return sum;
}

/*******************************************************************************
找一條路，讓大數之間能夠相互連結
*******************************************************************************/
static int AIFindWay_helper(int (*map)[5],int w,int h,int x,int y,char finder[5][5]){
    int sum = 0;
    int this = map[x][y];

    int those[4] = {-1,-1,-1,-1};

    int board[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int have[17] = {0};
    AICheckALLBoard(x,y,w,h){
        forp(k,16){
            if(map[x][y] == board[k]){
                have[k] += 1;
                break;
            }
        }
    }

    int one = map[x][y]-1;
    int i = 1;
    while(1){
        if(have[map[x][y]-i]){
            one = map[x][y]-i;
            break;
        }
        i++;
        if(i == map[x][y]){
            one = 0;
            break;
        }
    }

    those[0] = (AICheckInRegionXY(x-1,y,w,h) && map[x-1][y]<=map[x][y] && map[x-1][y]>=one && map[x-1][y]) ? map[x-1][y] : -1;
    those[1] = (AICheckInRegionXY(x+1,y,w,h) && map[x+1][y]<=map[x][y] && map[x+1][y]>=one && map[x+1][y]) ? map[x+1][y] : -1;
    those[2] = (AICheckInRegionXY(x,y-1,w,h) && map[x][y-1]<=map[x][y] && map[x][y-1]>=one && map[x][y-1]) ? map[x][y-1] : -1;
    those[3] = (AICheckInRegionXY(x,y+1,w,h) && map[x][y+1]<=map[x][y] && map[x][y+1]>=one && map[x][y+1]) ? map[x][y+1] : -1;


    if(those[0] != -1 && !finder[x-1][y]){
        finder[x-1][y] = 1;
        those[0] = this + AIFindWay_helper(map,w,h,x-1,y,finder);
    }
    if(those[1] != -1 && !finder[x+1][y]){
        finder[x+1][y] = 1;
        those[1] = this + AIFindWay_helper(map,w,h,x+1,y,finder);
    }
    if(those[2] != -1 && !finder[x][y-1]){
        finder[x][y-1] = 1;
        those[2] = this + AIFindWay_helper(map,w,h,x,y-1,finder);
    }
    if(those[3] != -1 && !finder[x][y+1]){
        finder[x][y+1] = 1;
        those[3] = this + AIFindWay_helper(map,w,h,x,y+1,finder);
    }
    
    forp(i,4){
        if(sum < those[i]){
            sum = those[i];
        }
    }

    return sum;
}

static int AIFindBigWay(const int (*map)[5],int w,int h){
    int sum = 0;

    int max = AIFindMaxNum(map,w,h);
    int X[20],Y[20];
    int len = 0;

    while(len < 3) {
        if(max <= 0)break;
        len += AIFindALLNumXY(map,w,h,&X[len],&Y[len],max--);
    }

    int sums[20] = {0};
    char finder[5][5] = {0};
    forp(i,len){
        if(!finder[X[i]][Y[i]]){
            finder[X[i]][Y[i]] = 1;
            sums[i] = AIFindWay_helper(map,w,h,X[i],Y[i],finder);
            if(sums[i] == 0)sum-=map[X[i]][Y[i]]*2;
        }
    }
    forp(i,len){
        //if(sum < sums[i])
        sum += sums[i];
    }
    return sum;
}

static int AIFindSmallWay_helper(int(*map)[5],int w,int h,int x,int y,char finder[5][5]){

    int sum = 0;
    int this = map[x][y];

    int those[4] = {-1,-1,-1,-1};

    int one = map[x][y];

    //debug("one %d",one);

    those[0] = (AICheckInRegionXY(x-1,y,w,h) && map[x-1][y]>=map[x][y] && map[x-1][y]<=one && map[x-1][y]<4) ? map[x-1][y] : -1;
    those[1] = (AICheckInRegionXY(x+1,y,w,h) && map[x+1][y]>=map[x][y] && map[x+1][y]<=one && map[x+1][y]<4) ? map[x+1][y] : -1;
    those[2] = (AICheckInRegionXY(x,y-1,w,h) && map[x][y-1]>=map[x][y] && map[x][y-1]<=one && map[x][y-1]<4) ? map[x][y-1] : -1;
    those[3] = (AICheckInRegionXY(x,y+1,w,h) && map[x][y+1]>=map[x][y] && map[x][y+1]<=one && map[x][y+1]<4) ? map[x][y+1] : -1;

    if(those[0] != -1 && !finder[x-1][y]){
        finder[x-1][y] = 1;
        those[0] = AIFindSmallWay_helper(map,w,h,x-1,y,finder);
    }
    if(those[1] != -1 && !finder[x+1][y]){
        finder[x+1][y] = 1;
        those[1] = AIFindSmallWay_helper(map,w,h,x+1,y,finder);
    }
    if(those[2] != -1 && !finder[x][y-1]){
        finder[x][y-1] = 1;
        those[2] = AIFindSmallWay_helper(map,w,h,x,y-1,finder);
    }
    if(those[3] != -1 && !finder[x][y+1]){
        finder[x][y+1] = 1;
        those[3] = AIFindSmallWay_helper(map,w,h,x,y+1,finder);
    }

    int max = 0;
    int d = -1;
    forp(i,4){
        if(max < those[i]){
            max = those[i];
            d = i;
        }
    }
    sum += max;
    sum += this;

    return sum;
}

static int AIFindSmallWay(const int (*map)[5],int w,int h){
    int sum = 0;

    int min = 1;
    int X[20],Y[20];
    int len = 0;

    while(len < 5){
        if(min > 4)break;
        len += AIFindALLNumXY(map,w,h,&X[len],&Y[len],min++);
    }

    int sums[20] = {0};
    char finder[5][5] = {0};
    forp(i,len){
        if(!finder[X[i]][Y[i]]){
            finder[X[i]][Y[i]] = 1;
            sum += AIFindSmallWay_helper(map,w,h,X[i],Y[i],finder,0);
        }
    }

    return sum;
}

static int AICheckMergeNum(const int (*mNextMap)[5],const int (*mCurMap)[5],int w,int h){
    return AICheckBlank(mNextMap,w,h)-AICheckBlank(mCurMap,w,h);
}

#define _HUGE_ENUF  (1e+300)
#define INFINITY   ((float)(_HUGE_ENUF * _HUGE_ENUF))
#define NAN        ((float)(INFINITY * 0.0F))

int AI6(int mCurMap[5][5],int w,int h){
    // frankrx41's AI
    // update form AI2
    float x[5] = {NAN,NAN,NAN,NAN,NAN};
    int mNextMap[5][5];
    float v[50][5] = {0};

    int RuleUsed[] = {
        WEIGHT_BLANK,WEIGHT_MERGE,WEIGHT_MERGE_NUM,
        WEIGHT_NEIGHBOR2,WEIGHT_NEIGHBOR_NUM,
        WEIGHT_BIG_NEIGHBOR,
        
        WEIGHT_BIG_BLOCK,
        WEIGHT_SMALL_AROUND,WEIGHT_SMALL_AROUND2,WEIGHT_SMALL_WAY,
        WEIGHT_FIND_WAY,
        WEIGHT_ONLY_ONE_NUM,
        WEIGHT_NEAR_BLANK,
    };

    for(int dir=0;dir<=4;dir++){

        memcpy(mNextMap,mCurMap,sizeof(mNextMap));
        if(dir){
            v[WEIGHT_MERGE][dir] = AICheckIfDir(mNextMap,w,h,dir);
        }else{
            v[WEIGHT_MERGE][dir] = 1;
        }

        if(v[WEIGHT_MERGE][dir])
        {

            int max = AIFindMaxNum(mNextMap,w,h);
            int blank = AICheckBlank(mCurMap,w,h);
            //debug("-------------------blank: %d",blank);

            v[WEIGHT_MERGE][dir]    *= 2;
            v[WEIGHT_BLANK][dir]    = AICheckBlank(mNextMap,w,h) * 2;
            v[WEIGHT_FIND_WAY][dir] = AIFindBigWay(mNextMap,w,h);
            v[WEIGHT_BIG_BLOCK][dir]= AICheckBigBlock(mNextMap,w,h);
            v[WEIGHT_DIE_END][dir]  = AICheckDieEnd(mNextMap,w,h);
            v[WEIGHT_ONLY_ONE_NUM][dir] = AICheckOnlyOneNum(mNextMap,w,h) * 2;

            if(blank < max/2){
                v[WEIGHT_MERGE_NUM][dir] = AICheckMergeNum(mNextMap,mCurMap,w,h) * 3;
                v[WEIGHT_MERGE_NUM][dir] *= 6;
            }


            int X[20],Y[20];
            int score[50][20] = {0};
            int len = AIFindALLNumXY(mNextMap,w,h,X,Y,0);
            forp(i,len)
            {
                mNextMap[X[i]][Y[i]] = 1;

                score[WEIGHT_BIG_NEIGHBOR][i]   = AICheckBigNeighbor(mNextMap,w,h);
                score[WEIGHT_NEIGHBOR_NUM][i]   = AICheckAllNeighborNum(mNextMap,w,h);
                score[WEIGHT_SMALL_AROUND][i]   = AICheckSmallNumAround(mNextMap,w,h);
                score[WEIGHT_SMALL_AROUND2][i]  = AICheckSmallNumAround2(mNextMap,w,h);
                score[WEIGHT_SMALL_WAY][i]      = AIFindSmallWay(mNextMap,w,h);
                score[WEIGHT_NEAR_BLANK][i]     = AICheckSmallNearBlank(mNextMap,w,h);
                
                if(blank < max/2)
                {
                    if(blank < 3){    
                    }
                    score[WEIGHT_SMALL_AROUND2][i]  = 0;
                    score[WEIGHT_BIG_NEIGHBOR][i]   = AICheckBigNeighbor3(mNextMap,w,h) * 2;
                }

                mNextMap[X[i]][Y[i]] = 0;
            }
            int res = -10000;
            int resi = -1;
            forp(i,len){
                int s = score[WEIGHT_SMALL_AROUND][i]   +
                        score[WEIGHT_SMALL_AROUND2][i]  +
                        score[WEIGHT_SMALL_AROUND3][i]  +
                        score[WEIGHT_BIG_NEIGHBOR][i]   +
                        score[WEIGHT_NEIGHBOR2][i]      +
                        score[WEIGHT_NEIGHBOR_NUM][i]   +
                        score[WEIGHT_SMALL_WAY][i]      
                        ;
                if(s > res){
                    res = s;
                    resi = i;
                }
                //debug("resi:%d res:%d score:%d",resi,res,score[WEIGHT_SMALL_AROUND][i]+score[WEIGHT_BIG_NEIGHBOR][i]);
            }
            v[WEIGHT_SMALL_AROUND][dir]     = score[WEIGHT_SMALL_AROUND][resi];
            v[WEIGHT_SMALL_AROUND2][dir]    = score[WEIGHT_SMALL_AROUND2][resi];
            v[WEIGHT_SMALL_AROUND3][dir]    = score[WEIGHT_SMALL_AROUND3][resi];
            v[WEIGHT_BIG_NEIGHBOR][dir]     = score[WEIGHT_BIG_NEIGHBOR][resi];
            v[WEIGHT_NEIGHBOR2][dir]        = score[WEIGHT_NEIGHBOR2][resi];
            v[WEIGHT_SMALL_WAY][dir]        = score[WEIGHT_SMALL_WAY][resi];
            v[WEIGHT_NEIGHBOR_NUM][dir]     = score[WEIGHT_NEIGHBOR_NUM][resi];
            v[WEIGHT_NEAR_BLANK][dir]       = score[WEIGHT_NEAR_BLANK][resi];


            x[dir] = 0;
            forp(i,WEIGHT_END){
                forp(j,arraylen(RuleUsed))
                {
                    if(i==RuleUsed[j])
                    {
                        x[dir] += v[i][dir];
                    }
                }
            }
        }
    }

#ifdef _DEBUG
    printf("\t\t\t\t   ↑ \t   ↓ \t   ← \t   → \n");
    forp(i,WEIGHT_END){
        //if(i == WEIGHT_BIG_BLOCK)SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0E);
        forp(j,arraylen(RuleUsed))
        {
            //debug("(%d %d) %d",j,RuleUsed[j],i);
            if(i==RuleUsed[j])
            {
                printf("%-16s",WeightTableName[i]);
                for(int k=0;k<=4;k++){
                    if(v[i][k]!=0){
                        printf("\t%6.2f",v[i][k]);
                    }
                    else{
                        printf("\t%s"," ");
                    }
                }
                printf("\n");
            }
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07);
    }
    loop(12)printf("-");
    printf("\n");
    printf("%-16s","Total");
    for(int k=0;k<=4;k++){
        if(x[k]==x[k]){
            printf("\t%6.2f",x[k]);
        }else{
            printf("\t%s"," ");
        }
    }
    printf("\n");
#endif


    int maxpoint = -0xFFFF;
    int dir = 0;
    for(int i=1;i<=4;i++){
        if(x[i] == -0xFFFF){
            continue;
        }
        if(maxpoint < x[i]){
            maxpoint = x[i];
            dir = i;
        }
    }
    return AIDebugPrintDir("Goto",dir);
}