#include <assert.h>
#include "Macro.h"
#include "Game.h"

#define WEIGHT_LEN  13

static enum{
    // normal rule
    WEIGHT_BLANK,
    WEIGHT_NEIGHBOR,
    WEIGHT_MAXNUM,
    WEIGHT_MERGE,
    WEIGHT_BLANK_AROUND,
    WEIGHT_WAVE,        // smooth, I not sure it is useful and I have to FIX IT

    WEIGHT_CORNER_NEIGHBOR,
    WEIGHT_CORNER_VALUE,

    WEIGHT_SIDE_LT_CORNER,
    WEIGHT_SIDE_GT_MID,

    WEIGHT_ALL_INSIDE,
    WEIGHT_ALL_AROUND,
    WEIGHT_ALL_NEAR,
    WEIGHT_ALL_TRIANGLE,

    WEIGHT_BIG_AROUND,
    WEIGHT_BIG_INCORNER,
    WEIGHT_BIG_INSIDE,
    WEIGHT_BIG_NEAR,
    WEIGHT_BIG_LINE_NEIGHBOR,
    WEIGHT_BIG_WAVE,
    WEIGHT_BIG_BLOCK,

    WEIGHT_SMALL_AROUND,

    WEIGHT_IN_BIG_SIDE,

    WEIGHT_DIE_END,
    WEIGHT_DEPTH,

    WEIGHT_END
};

static char *WeightTableName[] = {
    "blank",
    "neighbor",
    "maxnum",
    "merge",
    "blank around",
    "wave",

    "corner neighbor",
    "corner value",

    "side < corner",
    "side > mid",

    "all inside",
    "all around",
    "all near",
    "all triangle",

    "big around",
    "big in corner",
    "big in side",
    "big near",
    "big line neighbor",
    "big wave",
    "big block",

    "small around",

    "in big side",
    "die end",

    "depth",
};

static float WeightTable[][WEIGHT_LEN] = { 
    //{   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    //{   1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    },  // ALL_AROUND
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR

    //  0       2       4       8       16      32      64      128     256     512     1024    2048    4096
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BLANK
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      .5,     .5,     .5,      },  // NEIGHBOR 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // MAXNUMBER
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // MERGE 
    {   0.0,    0.0,    0.0,    .25,    .25,    .25,    .25,    .25,    .25,    .50,    .75,    .85,    .95,    },  // BLANK_NEAR_2_OR_4 
    {   .25,    .25,    .25,    .25,    .25,    .20,    .15,    .10,    .10,    .05,    .05,    .01,    .25,    },  // WAVE

    {   0,      0,      0,      0,      0,      0,      0,      1,      1,      1,      1,      1,      1,      },  // CORNER_NEIGHBOR
    {   0.0,    1.0,    2.0,    3.0,    4.0,    6.0,    8.0,    10.0,   14.,    15.,    16.,    21.,    25.,    },  // CORNER_VALUE
                                   
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // SIDE_<_CORNER
    {   0,      0,      0,      0,      0,      0.1,    0.2,    0.3,    0.4,    .95,    1.1,    1.2,     .1,    },  // SIDE_>_MID

    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_INSIDE
    {   .25,    .25,    .25,    .25,    .25,    .30,    .35,    .40,    .45,    .50,    0.45,   .25,    .25,    },  // ALL_AROUND
    {   .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .10,    },  // ALL_NEAR
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_TRIANGLE

    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1.5,    1.5,    1.5,    1.5,    },  // BIG_AROUND
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_INCORNER
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_INSIDE
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1.2,    1.5,    1,      1,      },  // BIG_NEAR
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // BIG_LINE_NEIGHBOR
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .35,    .45,    .55,    .25,    },  // BIG_WAVE
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_BLOCK

    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // SMALL_AROUND

    {   .25,    .25,    .25,    .25,    .55,    .55,    .55,    .55,    .55,    .55,    .55,    .55,    .55,    },  // INBIGSIDE

    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1.2,    1.5,    1,      1,      },  // DIE_END
    {   1,      .05,    .05,    .05,    .05,    .05,    .05,    .05,    1,      1,      1,      1,      1,      },  // DEPTH
};


/*******************************************************************************
Scoring rules:
Every Blank, Neighbor, Merger will get point

*******************************************************************************/



#include "AI.h"

#define AICheckInRegion(x,h)        ((x)<(h) && (x)>=0)
#define AICheckInRegionXY(x,y,w,h)  ((x)<(h) && (x)>=0 && (y)<(w) && (y)>=0)

static int AIIsFit(int a,int b){
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

#undef max
#undef min

static int AIMax(int a,int b){
    return a>b?a:b;
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

static int AIFindALLNumXY(const int(*map)[5],int w,int h,int *px,int *py,int max){
    int i = 0;
    forp(x,h)forp(y,w){
        if(map[x][y] == max){
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
    return (x==0 && y==0) || (x==h-1 && y==0) || (x==0 && y==w-1) || (x==h-1 && y==w-1);
}

static int AICheckBigNumInCorner(int (*map)[5],int w,int h){
    int x[20] = {0},y[20] = {0};
    int len = AIFindALLNumXY(map,w,h,&x,&y,AIFindMaxNum(map,w,h));
    int max = AIFindMaxNum(map,w,h);
    forp(i,len){
        if(AICheckInCorner(x[i],y[i],w,h)){
            //debug("%s:[%d,%d]",__FUNCTION__,x[i],y[i]);
            return max;
        }
    }
    return -max;
}

static int AICheckAllCornerValue(int(*map)[5],int w,int h){
    // this function is too inscrutable, so I disable it!
    return 0;

    int sum = 0;
    int cnt = 0;
    int x[4] = {0};
    
    x[0] = map[0][0];
    x[1] = map[0][w-1];
    x[2] = map[h-1][0];
    x[3] = map[h-1][w-1];
    //debug("%s: %d %d %d %d",__FUNCTION__,x[0],x[1],x[2],x[3]);


    //if( !AIIsFit(x[0],x[1]) || AIIsFit(x[0],x[2]) || AIIsFit(x[0],x[3])

    for(int i=0;i<4;i++){
        cnt = 0;
        for(int j=0;j<4;j++){
            if(i == j)continue;
            if(x[i] == 0 || x[j] == 0)continue;
            if(!AIIsFit(x[i],x[j]))cnt ++;
        }
        if(cnt >= 2)sum+=x[i];
        //debug("%s:x[%d]:%d sum:%d",__FUNCTION__,i,x[i],sum);
    }
    return sum;
}

static int AICheckBigNumInSide(int(*map)[5],int w,int h){
    int x[20],y[20];
    int len = AIFindALLNumXY(map,w,h,&x,&y,AIFindMaxNum(map,w,h));
    int max = AIFindMaxNum(map,w,h);
    forp(i,len){
        if(AICheckInSide(x[i],y[i],w,h)){
            return max;
        }
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

static int AICheckInBigSide(int x,int y,int w,int h,int d){
    
}

static int AICheckAllInBigSide(int(*map)[5],int w,int h){
    int sum = 0;
    int v[4] = {map[0][0],map[h-1][0],map[0][w-1],map[h-1][w-1]};
    int max = -1;
    int d;
    forp(i,4){
        if(max < v[i]){
            max = v[i];
            d = i;
        }
    }

    switch(d){
    case 0:
        return AIMax( map[0][1]+map[0][2]+map[0][3] , map[1][0]+map[2][0]+map[3][0] );
    case 1:
        return AIMax( map[h-1][1]+map[h-1][2]+map[h-1][3] , map[1][0]+map[2][0]+map[3][0] );
    case 2:
        return AIMax( map[0][1]+map[0][2]+map[0][3] , map[1][w-1]+map[2][w-1]+map[3][w-1] );
    case 3:
        return AIMax( map[h-1][1]+map[h-1][2]+map[h-1][3] , map[1][w-1]+map[2][w-1]+map[3][w-1] );
    }
    return sum;
}

static int AICheckBigNumAround(const int (*map)[5],int w,int h){
    int sum = 0;
    int maxnum = AIFindMaxNum(map,w,h);
    forp(x,h)forp(y,w){
        if(map[x][y] >= maxnum-1){
            
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);
            //if(sum)debug("v:%d [%d,%d]",sum,x,y);

            //sum += !AICheckInRegion(x-1,h)?0:AIIsFit(map[x-1][y],map[x][y]);
            //sum += !AICheckInRegion(x+1,h)?0:AIIsFit(map[x+1][y],map[x][y]);
            //sum += !AICheckInRegion(y-1,w)?0:AIIsFit(map[x][y-1],map[x][y]);
            //sum += !AICheckInRegion(y+1,w)?0:AIIsFit(map[x][y+1],map[x][y]);

            //sum += (!AICheckInRegion(x-1,h) || !AICheckInRegion(y-1,w))?0:AIIsFit(map[x-1][y-1],map[x][y]);
            //sum += (!AICheckInRegion(x+1,h) || !AICheckInRegion(y-1,w))?0:AIIsFix(map[x+1][y-1],map[x][y]);
            //sum += (!AICheckInRegion(x-1,h) || !AICheckInRegion(y+1,w))?0:AIIsFix(map[x-1][y+1],map[x][y]);
            //sum += (!AICheckInRegion(x+1,h) || !AICheckInRegion(y+1,w))?0:AIIsFix(map[x+1][y+1],map[x][y]);

            //if(sum)debug("v:%d [%d,%d]",sum,x,y);

            int v[4];
            v[0] = !AICheckInRegion(x-1,h)?0:AIIsFit(map[x-1][y],map[x][y]);
            v[1] = !AICheckInRegion(x+1,h)?0:AIIsFit(map[x+1][y],map[x][y]);
            v[2] = !AICheckInRegion(y-1,w)?0:AIIsFit(map[x][y-1],map[x][y]);
            v[3] = !AICheckInRegion(y+1,w)?0:AIIsFit(map[x][y+1],map[x][y]);
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
            v[0] = !AICheckInRegion(x-1,h)?0:AIIsFit(map[x-1][y],map[x][y]);
            v[1] = !AICheckInRegion(x+1,h)?0:AIIsFit(map[x+1][y],map[x][y]);
            v[2] = !AICheckInRegion(y-1,w)?0:AIIsFit(map[x][y-1],map[x][y]);
            v[3] = !AICheckInRegion(y+1,w)?0:AIIsFit(map[x][y+1],map[x][y]);
            int max = 0;
            forp(i,4){
                if(max < v[i]){
                    max = v[i];
                }
            }
            sum += max;
        }
        //else{
        //    sum += !AICheckInRegion(x-1,h)?0:AIIsFit(map[x-1][y],map[x][y]);
        //    sum += !AICheckInRegion(x+1,h)?0:AIIsFit(map[x+1][y],map[x][y]);
        //    sum += !AICheckInRegion(y-1,w)?0:AIIsFit(map[x][y-1],map[x][y]);
        //    sum += !AICheckInRegion(y+1,w)?0:AIIsFit(map[x][y+1],map[x][y]);
        //}
    }
    return sum;
}

static int AICheckAllTriangle_helper(const int (*map)[5],int w,int h,int x,int y,int dx,int dy){
    int sum = 0;
    if(AICheckInRegionXY(x+dx,y,w,h) && AIIsFit(map[x+dx][y],map[x][y])){
        sum += map[x+dx][y];
    }else{
        // TODO: should I do this?
    }
    if(AICheckInRegionXY(x,y+dy,w,h) && AIIsFit(map[x][y+dy],map[x][y])){
        sum += map[x][y+dy];
    }
    return sum;
}

static int AICheckAllTriangle(const int (*map)[5],int w,int h){
    // find max corner
    int c[4] = {0};
    c[0] = map[0][0]  +map[0][1]  +map[1][0]  +map[1][1];               // l-t
    c[1] = map[h-1][0]+map[h-1][1]+map[h-2][0]+map[h-2][1];             // l-b
    c[2] = map[0][w-1]+map[0][w-2]+map[1][w-1]+map[1][w-2];             // r-t
    c[3] = map[h-1][w-1]+map[h-1][w-2]+map[h-2][w-1]+map[h-2][w-2];     // r-b
    int max = -1;
    int d = -1;
    forp(i,4){
        if(c[i] > max){
            d = i;
            max = c[i];
        }
    }
    int sum = 0;
    forp(x,h)forp(y,h){
        switch(d){
        case 0:
            sum += AICheckAllTriangle_helper(map,w,h,x,y,+1,+1);
            break;
        case 1:
            sum += AICheckAllTriangle_helper(map,w,h,x,y,-1,+1);
            break;
        case 2:
            sum += AICheckAllTriangle_helper(map,w,h,x,y,+1,-1);
            break;
        case 3:
            sum += AICheckAllTriangle_helper(map,w,h,x,y,-1,-1);
            break;
        }
    }
    return sum;
}

static int AICheckOneNeighbor(const int(*map)[5],int w,int h,int x,int y){
    int sum = 0;
    sum += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y]) ? 1 : 0;
    sum += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y]) ? 1 : 0;
    return sum>0 ? map[x][y]:0;
}

static int AICheckNeighbor(const int(*map)[5],int w,int h){
    int sum = 0;
    forp(x,h)forp(y,w){
        if(map[x][y]!=0 /*&& map[x][y]>2*/){
            int v =AICheckOneNeighbor(map,w,h,x,y); 
            sum += v;
            //if(v)debug("[%d,%d]",x,y);
        }
    }
    return sum;
}

static int AICheckOneNear(int w,int h,int x,int y){

}

static int AICheckXYArrayNear(const int (*map)[5],int w,int h,int x[],int y[],int len){
    int sum = 0;
    for(int i=0;i<len;i++){
        for(int j=i+1;j<len;j++){
            if(x[i]==x[j] && y[i]==y[j]){
                continue;
            }
            int tol = 0;
            tol += map[x[i]][y[i]];
            tol += map[x[j]][y[j]];
            //debug("tol:%d",tol);
            
            //debug("[%d,%d] ~ [%d,%d]",x[i],y[i],x[j],y[j]);

            if((x[i] == x[j] && (y[i] == y[j] + 1 || y[i] == y[j] - 1)) ||
               (y[i] == y[j] && (x[i] == x[j] + 1 || x[i] == x[j] - 1)))
            {
                sum += tol/2;
            }else{
                sum -= tol/2;
                //debug("sum:%d",sum);
            }
        }
    }
    //debug("return sum:%d",sum);
    return sum;
}

static int AICheckBigNear(const int (*map)[5],int w,int h){
    int x[40] = {0},y[40] = {0};
    int len = 0;
    int max = AIFindMaxNum(map, w, h);

    while(len < 3) {
        if(max < 0)break;
        len += AIFindALLNumXY(map,w,h,&x[len],&y[len],max--);
    }

    int sum = 0;
    int fnear = 0;
    sum += AICheckXYArrayNear(map,w,h,x,y,len);
    //debug("%s sum:%d",__FUNCTION__,sum);
    return (int)sum;
}

static int AICheckAllNear(const int (*map)[5],int w,int h){
    int x[40] = {0},y[40] = {0};
    const int maxnum = AIFindMaxNum(map,w,h);
    int sum = 0;
    int time = maxnum;

    while(time--)
    {
        if(time < maxnum-3)break;
        int find = time;
        int len = 0;
        //debug("find:%d",find);
        if(!AIIsHaveNum(map,w,h,find))continue;
        while(len < 2) {
            if(find <= 0)break;
            len += AIFindALLNumXY(map,w,h,&x[len],&y[len],find--);
        }
        forp(i,len){
            //printf("[%d,%d](%d) ",x[i],y[i],map[x[i]][y[i]]);
        }
        //debug("get return %d",AICheckXYArrayNear(map,w,h,x,y,len));
        sum += AICheckXYArrayNear(map,w,h,x,y,len);
    }
    return (int)sum;
}

static int AICheckAroundIsNum(int (*map)[5],int w,int h,int x,int y,int num){
    int sum = 0;
    sum += AICheckInRegionXY(x-1,y,w,h)&&map[x-1][y]==num ? 1 : 0;
    sum += AICheckInRegionXY(x+1,y,w,h)&&map[x+1][y]==num ? 1 : 0;
    sum += AICheckInRegionXY(x,y-1,w,h)&&map[x][y-1]==num ? 1 : 0;
    sum += AICheckInRegionXY(x,y+1,w,h)&&map[x][y+1]==num ? 1 : 0;
    return sum;
}

// use only in AICheckBlankNear2or4
static int AICheckBlankAround_helper(int (*map)[5],int w,int h,int x,int y,int num){
    int sum = 0;
    sum += !AICheckInRegionXY(x-1,y,w,h) ? 0 : 2 - map[x-1][y];
    sum += !AICheckInRegionXY(x+1,y,w,h) ? 0 : 2 - map[x+1][y];
    sum += !AICheckInRegionXY(x,y-1,w,h) ? 0 : 2 - map[x][y-1];
    sum += !AICheckInRegionXY(x,y+1,w,h) ? 0 : 2 - map[x][y+1];
    return sum;
}

static int AICheckBlankAround(int (*map)[5],int w,int h){
    int sum = 0;
    int x[40] = {0},y[40] = {0};
    int len = AIFindALLNumXY(map,w,h,x,y,0);
    forp(i,len){
        sum += AICheckBlankAround_helper(map,w,h,x[i],y[i],1);
    }
    return sum;
}

static int AICheckIsWaveUp(int num[],int len,int *retmax){
    int max = num[0];
    int ret = 0;

    forp(i,len){
        printf("%d ",num[i]);
    }
    debug();

    int tol1 = 1;
    int tol2 = 0;
    for(int i=1;i<len;i++){
        if(num[i] >= max && num[i] <= max+2){
            max = num[i];
            tol1++;
            debug("+ (%d)",max);
        }
        else{
            //debug("tol(Up):%d",tol1);
            debug("enter else");
            max = num[i];
            ret  = tol1>tol2 ? max : ret;
            tol2 = tol1>tol2 ? tol1 : tol2;
            tol1 = 1;
        }
    }
    //debug("tol1: %d tol2: %d",tol1,tol2);
    //assert(tol1>tol2);

    ret  = tol1>tol2 ? max : ret;
    tol2 = tol1>tol2 ? tol1 : tol2;

    *retmax = ret;
    debug("ret tol: %d max: %d",tol2,ret);
    return tol2;
}

static int AICheckIsWaveDown(const int num[],const int len,int *retmax){
    int min = num[0];
    int max = num[0];
    int ret = 0;

    forp(i,len){
        printf("%d ",num[i]);
    }
    debug();

    int tol1 = 1;
    int tol2 = 0;
    for(int i=1;i<len;i++){
        if(num[i] <= min && num[i] >= min-2){
            min = num[i];
            tol1++;
            debug("-");

        }else{
            debug("tol(Down):%d",tol1);
            min = num[i];
            max = num[i];
            ret  = tol1>tol2 ? max : ret;
            tol2 = tol1>tol2 ? tol1 : tol2;
            tol1 = 1;
        }
        //debug("i: %d",i);
    }
    //*maxnum = tol1>tol3 ? num[0] : *maxnum;
    ret  = tol1>tol2 ? max : ret;
    tol2 = tol1>tol2 ? tol1 : tol2;

    *retmax = ret;
    debug("ret tol: %d max: %d",tol2,ret);
    return tol2;
}

static int AICheckWave(int(*map)[5],int w,int h){
    int num[20];
    int sum = 0;
    int max,min;
    int Bigmax;
    
    int tmp = 0;
    int len = 0;

    for(int x=0;x<h;x++){
        max = map[x][0];
        Bigmax = map[x][0];
        tmp = 1;
        for(int y=1;y<w;y++){
            if(map[x][y] >= max && map[x][y] <= max+2){
                tmp++;
                max = map[x][y];
            }else{
                max = map[x][y];
                Bigmax = len>tmp ? max : Bigmax;
                len = len>tmp ? len : tmp;
                tmp = 1;
            }
        }
        if(len == 2 || Bigmax == 0)continue;
        //debug("1 max:%d len:%d",Bigmax,len);
        sum += len * Bigmax;
    }

    for(int x=0;x<h;x++){
        min = map[x][0];
        Bigmax = map[x][0];
        tmp = 1;
        for(int y=1;y<w;y++){
            if(map[x][y] <= min && map[x][y] >= min-2){
                tmp++;
                min = map[x][y];
            }
            else{
                min = map[x][y];
                Bigmax = len>tmp ? max : Bigmax;
                len = len>tmp ? len : tmp;
                tmp = 1;
            }
        }
        if(len == 2 || Bigmax == 0)continue;
        //debug("2 max:%d len:%d",Bigmax,len);
        sum += len * Bigmax;
    }

    for(int y=0;y<w;y++){
        max = map[0][y];
        Bigmax = map[0][y];
        tmp = 1;
        for(int x=1;x<h;x++){
            if(map[x][y] >= max && map[x][y] <= max+2){
                tmp++;
                max = map[x][y];
            }
            else{
                max = map[x][y];
                Bigmax = len>tmp ? max : Bigmax;
                len = len>tmp ? len : tmp;
                tmp = 1;
            }
        }
        if(len == 2 || Bigmax == 0)continue;
        //debug("3 max:%d len:%d",Bigmax,len);
        sum += len * Bigmax;
    }

    for(int y=0;y<w;y++){
        min = map[0][y];
        Bigmax = map[0][y];
        tmp = 1;
        for(int x=1;x<h;x++){
            if(map[x][y] <= min && map[x][y] >= min-2){
                tmp++;
                min = map[x][y];
            }
            else{
                min = map[x][y];
                Bigmax = len>tmp ? max : Bigmax;
                len = len>tmp ? len : tmp;
                tmp = 1;
            }
        }
        if(len == 2 || Bigmax == 0)continue;
        //debug("4 max:%d len:%d",Bigmax,len);
        sum += len * Bigmax;
    }
    //debug("sum:%d",sum);
    return sum;
}


static int AICheckAll(const int(*mNextMap)[5],int w,int h){
    float v = 0;
    float sum = 0;
    int max = AIFindMaxNum(mNextMap,w,h);
    // All number
    v = (AICheckAllCornerValue(mNextMap,w,h) * WeightTable[WEIGHT_CORNER_VALUE][max]);
    sum += v;
    //debug("WEIGHT_CORNER_VALUE: %g",v);

    v = AICheckAllInSide(mNextMap,w,h) * WeightTable[WEIGHT_ALL_INSIDE][max];
    sum += v;
    //debug("WEIGHT_ALL_INSIDE: %g",v);

    v = AICheckAllAround(mNextMap,w,h) * WeightTable[WEIGHT_ALL_AROUND][max];
    sum += v;
    //debug("WEIGHT_ALL_AROUND: %g",v);


    // Big number
    v = AICheckBigNumInCorner(mNextMap,w,h) * WeightTable[WEIGHT_BIG_INCORNER][max];
    sum += v;
    //debug("WEIGHT_BIG_INCORNER: %g",v);

    v = AICheckBigNumInSide(mNextMap,w,h) * WeightTable[WEIGHT_BIG_INSIDE][max];
    sum += v;
    //debug("WEIGHT_BIG_INSIDE: %g",v);

    v = AICheckBigNumAround(mNextMap,w,h) * WeightTable[WEIGHT_BIG_AROUND][max];
    sum += v;
    //debug("WEIGHT_BIG_AROUND: %g",v);

    v = AICheckBigNear(mNextMap,w,h) * WeightTable[WEIGHT_BIG_NEAR][max];
    sum += v;
    //debug("WEIGHT_BIG_NEAR: %g",v);


    // mormal
    v = AIFindMaxNum(mNextMap,w,h) * WeightTable[WEIGHT_MAXNUM][max];
    sum += v;
    //debug("WEIGHT_MAXNUM: %g",v);

    v = AICheckBlank(mNextMap,w,h) * WeightTable[WEIGHT_BLANK][max];
    sum += v;
    //debug("WEIGHT_BLANK: %g",v);

    v = AICheckNeighbor(mNextMap,w,h) * WeightTable[WEIGHT_NEIGHBOR][max];
    sum += v;
    //debug("WEIGHT_NEIGHBOR: %g",v);

    v = AICheckAllNear(mNextMap,w,h) * WeightTable[WEIGHT_ALL_NEAR][max];
    sum += v;
    //debug("WEIGHT_ALL_NEAR: %g",v);

    v = AICheckBlankAround(mNextMap,w,h) * WeightTable[WEIGHT_BLANK_AROUND][max];
    sum += v;
    //debug("WEIGHT_BLANK_NEAR_2_OR_4: %g",v);

    v = AICheckWave(mNextMap,w,h) * WeightTable[WEIGHT_WAVE][max];
    sum += v;
    //debug("WEIGHT_WAVE: %g",v);

    return (int)sum;
}

static int AICheckTree(const int(*map)[5],int w,int h,int depth){
    if(depth-- == 0)return 0;
    //debug("check tree!");

    int x[5] = {-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF};
    int mNextMap[5][5];
    memcpy(mNextMap,map,sizeof(mNextMap));
    for(int i=1;i<=4;i++){
        memcpy(mNextMap,map,sizeof(mNextMap));
        float v = AICheckIfDir(mNextMap,w,h,i) * WEIGHT_MERGE;
        if(v)
        {
            int max = AIFindMaxNum(mNextMap,w,h);

            x[i] = 0;

            x[i] += v;

            //v = AICheckAllNeighbor(mNextMap,w,h) * WeightTable[WEIGHT_NEIGHBOR][max];
            //x[i] += v;
            //
            //v = AICheckAllAround(mNextMap,w,h) * WeightTable[WEIGHT_ALL_AROUND][max];
            //x[i] += v;

            //v = AICheckBigNumInCorner(mNextMap,w,h) * WeightTable[WEIGHT_BIG_INCORNER][max];
            //x[i] += v;
            //debug("WEIGHT_BIG_INCORNER: %g",v);
            //
            //v = AICheckBigNumInSide(mNextMap,w,h) * WeightTable[WEIGHT_BIG_INSIDE][max];
            //x[i] += v;
            //debug("WEIGHT_BIG_INSIDE: %g",v);
            //
            //v = AICheckBigNumAround(mNextMap,w,h) * WeightTable[WEIGHT_BIG_AROUND][max];
            //x[i] += v;
            //debug("WEIGHT_BIG_AROUND: %g",v);
            //
            //v = AICheckBigNear(mNextMap,w,h) * WeightTable[WEIGHT_BIG_NEAR][max];
            //x[i] += v;
            //debug("WEIGHT_BIG_NEAR: %g",v);
            //
            //v = AICheckBlankNear2or4(mNextMap,w,h) * WeightTable[WEIGHT_BLANK_NEAR_2_OR_4][max];
            //x[i] += v;
            //
            //v = AICheckWave(mNextMap,w,h) * WeightTable[WEIGHT_WAVE][max];
            //x[i] += v;

            //x[i] += AICheckTree(mNextMap,w,h,depth);

            //if(depth==0){
            //    v = AICheckAll(mNextMap,w,h);
            //    debug("%g",v);
            //    x[i] = v;
            //}else{
            //    x[i] = AICheckTree(mNextMap,w,h,depth);
            //}

            v = AICheckAll(mNextMap,w,h);
            x[i] += v;

            x[i] += AICheckTree(mNextMap,w,h,depth-1);
        }
    }

    int maxpoint = -0xFFFF;
    int dir = 0;
    for(int i=1;i<=4;i++){
        if(x[i] == -0xFFFF){
            //debug("x[%d] get null points",i);
            continue;
        }
        if(maxpoint < x[i]){
            maxpoint = x[i];
            dir = i;
        }
        //debug("x[%d] get %d points",i,x[i]);
    }

    //int max = AIFindMaxNum(mNextMap,w,h);

    return x[dir];
    //if(depth-- == 0)return 0;
}

static int AICheckCornerNeighbor(int (*map)[5],int w,int h){
    int sum = 0;
    if(map[1][0] == map[0][0])sum += map[0][0];
    if(map[0][1] == map[0][0])sum += map[0][0];
    if(map[1][w-1] == map[0][w-1])sum += map[0][w-1];
    if(map[0][w-2] == map[0][w-1])sum += map[0][w-1];
    if(map[h-1][1] == map[h-1][0])sum += map[h-1][0];
    if(map[h-2][0] == map[h-1][0])sum += map[h-1][0];
    if(map[h-2][w-1] == map[h-1][w-1])sum += map[h-1][w-1];
    if(map[h-1][w-2] == map[h-1][w-1])sum += map[h-1][w-1];
    return sum;
}

static int AICheckSideGTMid(int (*map)[5],int w,int h){
    int c[4] = {0};
    c[0] = map[0][0]  +map[0][1]  +map[1][0]  +map[1][1];               // l-t
    c[1] = map[h-1][0]+map[h-1][1]+map[h-2][0]+map[h-2][1];             // l-b
    c[2] = map[0][w-1]+map[0][w-2]+map[1][w-1]+map[1][w-2];             // r-t
    c[3] = map[h-1][w-1]+map[h-1][w-2]+map[h-2][w-1]+map[h-2][w-2];     // r-b
    int max = -1;
    int d = -1;
    forp(i,4){
        if(c[i] > max){
            d = i;
            max = c[i];
        }
    }
    int sum = 0;
    switch(d){
    case 0:
        for(int x=1;x<h-1;x++){
            if(map[x][0] > 4 && map[x][0] >= map[x][1]){
                sum += map[x][0];
            }
            else{
                //sum -= map[x][1];
            }
        }
        for(int y=1;y<w-1;y++){
            if(map[0][y] > 4 && map[0][y] >= map[1][y]){
                sum += map[0][y];
            }else{
                //sum -= map[1][y];
            }
        }
        break;
    case 1:
        for(int x=1;x<h-1;x++){
            if(map[x][0] > 4 && map[x][0] >= map[x][1]){
                sum += map[x][0];
            }
            else{
                //sum -= map[x][1];
            }
        }
        for(int y=1;y<w-1;y++){
            if(map[h-1][y] > 4 && map[h-1][y] >= map[h-2][y]){
                sum += map[h-1][y];
            }else{
                //sum -= map[h-2][y];
            }
        }
        break;
    case 2:
        for(int x=1;x<h-1;x++){
            if(map[x][w-1] > 4 && map[x][w-1] >= map[x][w-2]){
                sum += map[x][w-1];
            }
            else{
                //sum -= map[x][w-2];
            }
        }
        for(int y=1;y<w-1;y++){
            if(map[0][y] > 4 && map[0][y] >= map[1][y]){
                sum += map[0][y];
            }
            else{
                //sum -= map[1][y];
            }
        }
        break;
    case 3:
        for(int x=1;x<h-1;x++){
            if(map[x][w-1] > 4 && map[x][w-1] >= map[x][w-2]){
                sum += map[x][w-1];
            }
            else{
                //sum -= map[x][w-2];
            }
        }
        for(int y=1;y<w-1;y++){
            if(map[h-1][y] > 4 && map[h-1][y] >= map[h-2][y]){
                sum += map[h-1][y];
            }
            else{
                //sum -= map[h-2][y];
            }
        }
        break;
    }
    return sum;
}

static int AICheckSideLTCorner(int (*map)[5],int w,int h){
    int sum = 0;
    sum += AIIsFit(map[0][0],map[0][1]);
    sum += AIIsFit(map[0][0],map[1][0]);
    sum += AIIsFit(map[h-1][0],map[h-2][0]);
    sum += AIIsFit(map[h-1][0],map[h-1][1]);
    sum += AIIsFit(map[0][w-1],map[0][w-2]);
    sum += AIIsFit(map[0][w-1],map[1][w-1]);
    sum += AIIsFit(map[h-1][w-1],map[h-1][w-2]);
    sum += AIIsFit(map[h-1][w-1],map[h-2][w-1]);
    return sum;
}

static int AICheckBigLineNeighbor(int (*map)[5],int w,int h){
    int x[20] = {0},y[20] = {0};
    int sum = 0;
    int len = AIFindALLNumXY(map,w,h,&x,&y,AIFindMaxNum(map,w,h));
    forp(i,len){
        forp(j,h){
            sum += AICheckOneNeighbor(map,w,h,j,y[i]);
        }
        forp(j,w){
            sum += AICheckOneNeighbor(map,w,h,x[i],j);
        }
    }
    return sum;
}

/*
// form big to small
static int AICheckBigWave_helper(int a,int b,int c,int d){
    int sum = 0;
    //if(a<b || b<c || c<d)return 0;
    sum += (a-b)+(b-c)+(c-d);
    return a*4-sum;
}

static int AIMax(int a,int b){
    return a>b?a:b;
}

static int AICheckBigWave(int (*map)[5],int w,int h){
    int v[4] = {map[0][0],map[h-1][0],map[0][w-1],map[h-1][w-1]};
    int max = -1;
    int d;
    forp(i,4){
        if(max < v[i]){
            max = v[i];
            d = i;
        }
    }
    switch(d)
    {
    case 0:
        return AIMax(   AICheckBigWave_helper(map[0][0],map[0][1],map[0][2],map[0][3]),
                        AICheckBigWave_helper(map[0][0],map[1][0],map[2][0],map[3][0]));
    case 1:
        return AIMax(   AICheckBigWave_helper(map[h-1][0],map[h-1][1],map[h-1][2],map[h-1][3]),
                        AICheckBigWave_helper(map[0][0],map[1][0],map[2][0],map[3][0]));
    case 2:
        return AIMax(   AICheckBigWave_helper(map[0][0],map[0][1],map[0][2],map[0][3]),
                        AICheckBigWave_helper(map[0][w-1],map[1][w-1],map[2][w-1],map[3][w-1]));
    case 3:
        return AIMax(   AICheckBigWave_helper(map[h-1][0],map[h-1][1],map[h-1][2],map[h-1][3]),
                        AICheckBigWave_helper(map[0][w-1],map[1][w-1],map[2][w-1],map[3][w-1]));
    }
    return 0;
}
*/

static int AICheckBigWave(int(*map)[5],int w,int h){
    int max = AIFindMaxNum(map,w,h);
    int sum = 0;
    if(map[0][0] == max){
        //if(
    }
    elif(map[h-1][0] == max){

    }
    elif(map[0][w-1] == max){

    }
    elif(map[h-1][w-1] == max){

    }
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

static float AIBigBlock(const int(*map)[5],int w,int h){
    float sum = 0;
    int max = AIFindMaxNum(map,w,h);
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] != 0){
            int fBlock = 0;
            if(AICheckInRegionXY(x-1,y,w,h) && AICheckInRegionXY(x+1,y,w,h) && 
               map[x-1][y]<map[x][y] && map[x+1][y]<map[x][y] &&
               map[x-1][y]!=0 && map[x+1][y]!=0){
                fBlock += map[x-1][y] + map[x+1][y];
            }
            if(AICheckInRegionXY(x,y-1,w,h) && AICheckInRegionXY(x,y+1,w,h) &&
               map[x][y-1]<map[x][y] && map[x][y+1]<map[x][y] &&
               map[x][y-1]!=0 && map[x][y+1]!=0){
                fBlock += map[x][y-1] + map[x][y+1];
            }

            if(fBlock){
                sum += fBlock;
            }

        }
    }
    return -sum/2;
}

static int AICheckSmallNumAround(const int(*map)[5],int w,int h){
    int sum = 0;
    int max = AIFindMaxNum(map,w,h);
    //debug("%s:(%d)",__FUNCTION__,max/2);
    AICheckALLBoard(x,y,w,h){
        if(map[x][y] == 2 || map[x][y] == 4){
            int fHaveBlank = 0;
            AICheckAround(x,y,w,h){
                if(map[x+i][y+j]==0 || map[x+i][y+j]==map[x][y] || 
                   map[x+i][y+j]==map[x][y]-1 || map[x+i][y+j]==map[x][y]-2){
                    fHaveBlank = 1;
                }
            }
            //debug("[%d,%d] %d",x,y,fHaveBlank);

            if(!fHaveBlank){
                int maxnum = 0;
                AICheckAround(x,y,w,h){
                    if(map[x+i][y+j] > maxnum){
                        maxnum = map[x+i][y+j];
                    }
                }
                //debug("[%d,%d]%d %d",x,y,map[x][y],maxnum);
                sum += maxnum;
            }

        }
    }
    //debug("--");
    return -sum;
}

#define _HUGE_ENUF  (1e+300)
#define INFINITY   ((float)(_HUGE_ENUF * _HUGE_ENUF))
#define NAN        ((float)(INFINITY * 0.0F))

int AI2(int mCurMap[5][5],int w,int h){
    // super stupid AI
    //debug("--------------------AI--------------------");
    float x[5] = {NAN,NAN,NAN,NAN,NAN};
    int mNextMap[5][5];
    float v[50][5] = {0};

    int RuleUsed[] = {
        WEIGHT_BLANK,WEIGHT_NEIGHBOR,WEIGHT_MAXNUM,WEIGHT_MERGE,
        WEIGHT_BIG_INCORNER,WEIGHT_BIG_INSIDE,WEIGHT_DIE_END,
        WEIGHT_BIG_WAVE,WEIGHT_BIG_BLOCK,WEIGHT_BIG_AROUND,
        WEIGHT_SMALL_AROUND,
    };

    for(int dir=1;dir<=4;dir++){
        memcpy(mNextMap,mCurMap,sizeof(mNextMap));
        v[WEIGHT_MERGE][dir] = AICheckIfDir(mNextMap,w,h,dir);
        if(v[WEIGHT_MERGE][dir])
        {
            int max = AIFindMaxNum(mNextMap,w,h);
            v[WEIGHT_MERGE][dir] *= WeightTable[WEIGHT_MERGE][max];

            v[WEIGHT_BLANK][dir]    = AICheckBlank(mNextMap,w,h)            * WeightTable[WEIGHT_BLANK][max];
            v[WEIGHT_NEIGHBOR][dir] = AICheckNeighbor(mNextMap,w,h)      * WeightTable[WEIGHT_NEIGHBOR][max];
            v[WEIGHT_MAXNUM][dir]   = AIFindMaxNum(mNextMap,w,h)            * WeightTable[WEIGHT_MAXNUM][max];
            v[WEIGHT_BLANK_AROUND][dir] = AICheckBlankAround(mNextMap,w,h)  * WeightTable[WEIGHT_BLANK_AROUND][max];
            v[WEIGHT_ALL_NEAR][dir] = AICheckAllNear(mNextMap,w,h)          * WeightTable[WEIGHT_ALL_NEAR][max]; 
            v[WEIGHT_ALL_TRIANGLE][dir] = AICheckAllTriangle(mNextMap,w,h)  * WeightTable[WEIGHT_ALL_TRIANGLE][max]; 
            v[WEIGHT_ALL_AROUND][dir]   = AICheckAllAround(mNextMap,w,h)    * WeightTable[WEIGHT_ALL_AROUND][max];
            

            v[WEIGHT_CORNER_NEIGHBOR][dir]  = AICheckCornerNeighbor(mNextMap,w,h)   * WeightTable[WEIGHT_CORNER_NEIGHBOR][max];            
            v[WEIGHT_SIDE_GT_MID][dir]      = AICheckSideGTMid(mNextMap,w,h)        * WeightTable[WEIGHT_SIDE_GT_MID][max];            
            v[WEIGHT_SIDE_LT_CORNER][dir]   = AICheckSideLTCorner(mNextMap,w,h)     * WeightTable[WEIGHT_SIDE_LT_CORNER][max];            
            
            v[WEIGHT_BIG_AROUND][dir]   = AICheckBigNumAround(mNextMap,w,h)     * WeightTable[WEIGHT_BIG_AROUND][max];
            v[WEIGHT_BIG_INCORNER][dir] = AICheckBigNumInCorner(mNextMap,w,h)   * WeightTable[WEIGHT_BIG_INCORNER][max];
            v[WEIGHT_BIG_INSIDE][dir]   = AICheckBigNumInSide(mNextMap,w,h)     * WeightTable[WEIGHT_BIG_INSIDE][max];
            v[WEIGHT_BIG_NEAR][dir]     = AICheckBigNear(mNextMap,w,h)          * WeightTable[WEIGHT_BIG_NEAR][max];
            v[WEIGHT_BIG_LINE_NEIGHBOR][dir] = AICheckBigLineNeighbor(mNextMap,w,h) * WeightTable[WEIGHT_BIG_LINE_NEIGHBOR][max];
            v[WEIGHT_BIG_BLOCK][dir]    = AIBigBlock(mNextMap,w,h) * WeightTable[WEIGHT_BIG_BLOCK][max];

            v[WEIGHT_CORNER_VALUE][dir] = AICheckAllCornerValue(mNextMap,w,h)   * WeightTable[WEIGHT_CORNER_VALUE][max];
            v[WEIGHT_IN_BIG_SIDE][dir]  = AICheckAllInBigSide(mNextMap,w,h)     * WeightTable[WEIGHT_IN_BIG_SIDE][max];
            v[WEIGHT_WAVE][dir]         = AICheckWave(mNextMap,w,h)             * WeightTable[WEIGHT_WAVE][max];
            v[WEIGHT_SMALL_AROUND][dir] = AICheckSmallNumAround(mNextMap,w,h)   * WeightTable[WEIGHT_SMALL_AROUND][max];

            v[WEIGHT_DIE_END][dir]  = AICheckDieEnd(mNextMap,w,h)       * WeightTable[WEIGHT_DIE_END][max];

            v[WEIGHT_BIG_WAVE][dir] = AICheckBigWave(mNextMap,w,h) * WeightTable[WEIGHT_BIG_WAVE][max];

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
            //x[dir] += AICheckTree(mNextMap,w,h,1);
        }
    }

#ifdef _DEBUG
    printf("\t\t\t   �� \t   �� \t   �� \t   �� \n");
    forp(i,WEIGHT_END){
        if(i == WEIGHT_BIG_BLOCK)SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0E);
        forp(j,arraylen(RuleUsed))
        {
            //debug("(%d %d) %d",j,RuleUsed[j],i);
            if(i==RuleUsed[j])
            {
                printf("%-16s",WeightTableName[i]);
                for(int k=1;k<=4;k++){
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
    for(int i=1;i<=4;i++){
        if(x[i]==x[i]){
            printf("\t%6.2f",x[i]);
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
    //debug("--------------------AI--------------------");
    return AIDebugPrintDir("Goto",dir);
}

/*******************************************************************************
2048 seek:
23995
26656
*******************************************************************************/