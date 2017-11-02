#include "Macro.h"
#include "Game.h"
#include <assert.h>

/*
per blank + 1 point
per nearby + 1 point
*/

#define WEIGHT_BLANK        0
#define WEIGHT_NEARBY       1
#define WEIGHT_MAXNUM       2
#define WEIGHT_MERGE        3

#define WEIGHT_CORNER_VALUE 4
#define WEIGHT_ALL_INSIDE   5
#define WEIGHT_ALL_AROUND   6

#define WEIGHT_BIG_AROUND   7
#define WEIGHT_BIG_INCORNER 8
#define WEIGHT_BIG_INSIDE   9

float WeightTable[][13] = { 
    //  0       2       4       8       16      32      64      128     256     512     1024    2048    4096
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BLANK
  //{   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    {   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // MAXNUM 
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    0.5,    1.0,    1.5,    1.5,    2.5,    3.0,    },  // MERGE 
                                   
    {   0.0,    1.0,    2.0,    3.0,    4.0,    6.0,    8.0,    10.0,   14.,    15.,    16.,    21.,    25.,    },  // CORNER_VALUE
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_INSIDE
  //{   1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    },  // ALL_AROUND
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_AROUND
                       
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_AROUND
    {   2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      },  // BIG_INCORNER
    {   0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      },  // BIG_INSIDE
};

#define AICheckInRegion(x,h)        ((x)<(h) && (x)>=0)
#define AICheckInRegionXY(x,y,w,h)  ((x)<(h) && (x)>=0 && (y)<(w) && (y)>=0)

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
                                    fMerge += *F;           \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }

static int AICheckIfDir(int(*map)[5],int w,int h,int dir){
    int fHadmove = 0;
    int fMerge = 0;
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
    if(fMerge)return fMerge;
    else return fHadmove;
}

static int AIDebugPrintDir(char *str,int dir){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);
    switch(dir){
    case DIR_UP:
        debug("AI %s: Up",str);
        break;
    case DIR_DOWN:
        debug("AI %s: Down",str);
        break;
    case DIR_LEFT:
        debug("AI %s: Left",str);
        break;
    case DIR_RIGHT:
        debug("AI %s: Right",str);
        break;
    default:
        debug("AI %s error!",str);
        break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07);
    return dir;
}

static int AICheckBlank(const int(*map)[5],int w,int h){
    int sum = 0;
    for(int x=0;x<h;x++){
        for(int y=0;y<w;y++){
            if(map[x][y] == 0)
                sum++;
        }
    }
    //debug("%s: %d",__FUNCTION__,sum);
    return sum;
}

static int AICheckMaxNum(const int (*map)[5],int w,int h){
    int max = 0;
    forp(x,h)forp(y,w){
        if(map[x][y] > max){
            max = map[x][y];
        }
    }
    return max;
}

static int AICheckAllMaxNumXY(const int(*map)[5],int w,int h,int *px,int *py){
    int max = 0;
    int i = 0;
    forp(x,h)forp(y,w){
        if(map[x][y] > max){
            i = 0;
        }
        if(map[x][y] >= max){
            max = map[x][y];
            px[i] = x;
            py[i] = y;
            i++;
        }
    }
    return i;
}

static int AICheckInSide(int x,int y,int w,int h){
    //return !AICheckInRegion(x-1,h) || !AICheckInRegion(x+1,h) || !AICheckInRegion(y-1,w) || !AICheckInRegion(y+1,w);
    return x==0 || x==h-1 || y==0 || y==w-1;
}

static int AICheckInCorner(int x,int y,int w,int h){
    //return !AICheckInRegionXY(x-1,y-1,w,h) || !AICheckInRegionXY(x-1,y+1,w,h) || 
    //       !AICheckInRegionXY(x+1,y-1,w,h) || !AICheckInRegionXY(x+1,y+1,w,h);
    return (x==0 && y==0) || (x==h && y==0) || (x==0 && y==w) || (x==h && y==w);
}

static int AICheckBigNumInCorner(int (*map)[5],int w,int h){
    int x[20] = {0},y[20] = {0};
    int len = AICheckAllMaxNumXY(map,w,h,&x,&y);
    int max = AICheckMaxNum(map,w,h);
    forp(i,len){
        if(AICheckInCorner(x[i],y[i],w,h)){
            //debug("%s:[%d,%d]",__FUNCTION__,x[i],y[i]);
            return max;
        }
    }
    return -max;
}

static int AIIsFix(int a,int b){
    int max = max(a,b);
    int min = min(a,b);
    //return min>=max/2+1;
    //return max-(max/2+1-min);
    /*if(min == 0){
    return max/2;
    }*/
    /*if(min == 0){
    return 0;
    }*/
    if(min>=max/2+1){
        return min;
    }
    else{
        return min-max;
    }
    //return max/2+min-1;
}

static int AICheckAllCornerValue(int(*map)[5],int w,int h){
    int sum = 0;
    int cnt = 0;
    int x[4] = {0};
    
    x[0] = map[0][0];
    x[1] = map[0][w-1];
    x[2] = map[h-1][0];
    x[3] = map[h-1][w-1];
    //debug("%s: %d %d %d %d",__FUNCTION__,x[0],x[1],x[2],x[3]);


    //if( !AIIsFix(x[0],x[1]) || AIIsFix(x[0],x[2]) || AIIsFix(x[0],x[3])

    for(int i=0;i<4;i++){
        cnt = 0;
        for(int j=0;j<4;j++){
            if(i == j)continue;
            if(x[i] == 0 || x[j] == 0)continue;
            if(!AIIsFix(x[i],x[j]))cnt ++;
        }
        if(cnt >= 2)sum+=x[i];
        //debug("%s:x[%d]:%d sum:%d",__FUNCTION__,i,x[i],sum);
    }
    return sum;
}


static int AICheckBigNumInSide(int(*map)[5],int w,int h){
    int x[20],y[20];
    int len = AICheckAllMaxNumXY(map,w,h,&x,&y);
    int max = AICheckMaxNum(map,w,h);
    forp(i,len){
        if(AICheckInSide(x[i],y[i],w,h));
        return max;
    }
    return -max;
}

static int AICheckAllInSide(int(*map)[5],int w,int h){
    int sum = 0;
    forp(x,h)forp(y,w){
        sum += AICheckInSide(x,y,w,h) * map[x][y];
    }
    return sum;
}

static int AICheckBigNumAround(const int (*map)[5],int w,int h){
    int sum = 0;
    int maxnum = AICheckMaxNum(map,w,h);
    forp(x,h)forp(y,w){
        if(map[x][y] >= maxnum-1){
            

            //if(sum)debug("v:%d [%d,%d]",sum,x,y);
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);

            //sum += !AICheckInRegion(x-1,h)?0:AIIsFix(map[x-1][y],map[x][y]);
            //sum += !AICheckInRegion(x+1,h)?0:AIIsFix(map[x+1][y],map[x][y]);
            //sum += !AICheckInRegion(y-1,w)?0:AIIsFix(map[x][y-1],map[x][y]);
            //sum += !AICheckInRegion(y+1,w)?0:AIIsFix(map[x][y+1],map[x][y]);

            //sum += (!AICheckInRegion(x-1,h) || !AICheckInRegion(y-1,w))?0:AIIsFix(map[x-1][y-1],map[x][y]);
            //sum += (!AICheckInRegion(x+1,h) || !AICheckInRegion(y-1,w))?0:AIIsFix(map[x+1][y-1],map[x][y]);
            //sum += (!AICheckInRegion(x-1,h) || !AICheckInRegion(y+1,w))?0:AIIsFix(map[x-1][y+1],map[x][y]);
            //sum += (!AICheckInRegion(x+1,h) || !AICheckInRegion(y+1,w))?0:AIIsFix(map[x+1][y+1],map[x][y]);

            //if(sum)debug("v:%d [%d,%d]",sum,x,y);

            int v[4];
            v[0] = !AICheckInRegion(x-1,h)?0:AIIsFix(map[x-1][y],map[x][y]);
            v[1] = !AICheckInRegion(x+1,h)?0:AIIsFix(map[x+1][y],map[x][y]);
            v[2] = !AICheckInRegion(y-1,w)?0:AIIsFix(map[x][y-1],map[x][y]);
            v[3] = !AICheckInRegion(y+1,w)?0:AIIsFix(map[x][y+1],map[x][y]);
            int max = 0;
            forp(i,4){
                if(max < v[i]){
                    max = v[i];
                }
            }
            sum += max;
        }
    }
    return sum;
}

static int AICheckAllAround(const int (*map)[5],int w,int h){
    int sum = 0;
    forp(x,h)forp(y,w){
        //if(map[x][y] > 7)
        {
            // Big num check only one side
            int v[4];
            v[0] = !AICheckInRegion(x-1,h)?0:AIIsFix(map[x-1][y],map[x][y]);
            v[1] = !AICheckInRegion(x+1,h)?0:AIIsFix(map[x+1][y],map[x][y]);
            v[2] = !AICheckInRegion(y-1,w)?0:AIIsFix(map[x][y-1],map[x][y]);
            v[3] = !AICheckInRegion(y+1,w)?0:AIIsFix(map[x][y+1],map[x][y]);
            int max = 0;
            forp(i,4){
                if(max < v[i]){
                    max = v[i];
                }
            }
            sum += max;
        }
        //else{
        //    sum += !AICheckInRegion(x-1,h)?0:AIIsFix(map[x-1][y],map[x][y]);
        //    sum += !AICheckInRegion(x+1,h)?0:AIIsFix(map[x+1][y],map[x][y]);
        //    sum += !AICheckInRegion(y-1,w)?0:AIIsFix(map[x][y-1],map[x][y]);
        //    sum += !AICheckInRegion(y+1,w)?0:AIIsFix(map[x][y+1],map[x][y]);
        //}
    }
    return sum;
}

static int AICheckOneNearby(const int(*map)[5],int w,int h,int x,int y){
    int sum = 0;
    sum += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y])?map[x][y]:0;
    sum += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y])?map[x][y]:0;
    sum += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y])?map[x][y]:0;
    sum += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y])?map[x][y]:0;
    return sum>0?map[x][y]:0;
}

static int AICheckAllNearby(const int(*map)[5],int w,int h){
    int sum = 0;
    forp(x,h)forp(y,w){
        if(map[x][y]!=0){
            int v = AICheckOneNearby(map,w,h,x,y);
            //if(v)debug("v:%d [%d,%d]",v,x,y);
            sum += v;
        }
    }
    return sum;
}

int AI1(int map[5][5],int w,int h){
    debug("--------------------AI--------------------");
    // some AI, I hadn't named it :)
    int x[5] = {-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF};
    float v = 0;
    int mNextMap[5][5];
    int mCurMap[5][5];

    //assert(WeightTable[WEIGHT_CORNER_VALUE][0]==16);
    //assert(WeightTable[WEIGHT_BIG_INSIDE][0]==0);

    for(int i=1;i<=4;i++){
        memcpy(mNextMap,map,sizeof(mNextMap));
        v = AICheckIfDir(mNextMap,w,h,i) * WEIGHT_MERGE;
        if(v)
        {
            int max = AICheckMaxNum(mNextMap,w,h);
            x[i] = 0;
            AIDebugPrintDir("check dir",i);
            /*memcpy(mCurMap,map,sizeof(mCurMap));
            if(*/

            x[i] += v;
            debug("WEIGHT_MERGE: %g",v);

            // All number

            v += AICheckAllCornerValue(map,w,h) * WeightTable[WEIGHT_CORNER_VALUE][max];
            x[i] += v;
            debug("WEIGHT_CORNER_VALUE: %g",v);

            v = AICheckAllInSide(mNextMap,w,h) * WeightTable[WEIGHT_ALL_INSIDE][max];
            x[i] += v;
            debug("WEIGHT_ALL_INSIDE: %g",v);

            v = AICheckAllAround(mNextMap,w,h) * WeightTable[WEIGHT_ALL_AROUND][max];
            x[i] += v;
            debug("WEIGHT_ALL_AROUND: %g",v);


            // Big number
            v = AICheckBigNumInCorner(mNextMap,w,h) * WeightTable[WEIGHT_BIG_INCORNER][max];
            x[i] += v;
            debug("WEIGHT_BIG_INCORNER: %g",v);

            v = AICheckBigNumInSide(mNextMap,w,h) * WeightTable[WEIGHT_BIG_INSIDE][max];
            x[i] += v;
            debug("WEIGHT_BIG_INSIDE: %g",v);

            v = AICheckBigNumAround(mNextMap,w,h) * WeightTable[WEIGHT_BIG_AROUND][max];
            x[i] += v;
            debug("WEIGHT_BIG_AROUND: %g",v);


            // mormal
            v = AICheckMaxNum(mNextMap,w,h) * WeightTable[WEIGHT_MAXNUM][max];
            x[i] += v;
            debug("WEIGHT_MAXNUM: %g",v);
            
            v = AICheckBlank(mNextMap,w,h) * WeightTable[WEIGHT_BLANK][max];
            x[i] += v;
            debug("WEIGHT_BLANK: %g",v);
            
            v = AICheckAllNearby(mNextMap,w,h) * WeightTable[WEIGHT_NEARBY][max];
            x[i] += v;
            debug("WEIGHT_NEARBY: %g",v);
        }
    }

    int maxpoint = -0xFFFF;
    int dir = 0;
    for(int i=1;i<=4;i++){
        if(x[i] == -0xFFFF){
            continue;
            debug("x[%d] get null points",i);
        }
        if(maxpoint < x[i]){
            maxpoint = x[i];
            dir = i;
        }
        debug("x[%d] get %d points",i,x[i]);
    }
    debug("--------------------AI--------------------");
    return AIDebugPrintDir("Goto",dir);
}


