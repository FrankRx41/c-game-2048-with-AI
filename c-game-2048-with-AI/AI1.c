#include "Macro.h"
#include "Game.h"
#include <assert.h>

enum{
    WEIGHT_BLANK,
    WEIGHT_NEARBY,
    WEIGHT_MAXNUM,
    WEIGHT_MERGE,
    WEIGHT_BLANK_NEAR_2_OR_4,
    WEIGHT_WAVE,


    WEIGHT_CORNER_VALUE,
    WEIGHT_ALL_INSIDE,
    WEIGHT_ALL_AROUND,
    WEIGHT_ALL_NEAR,

    WEIGHT_BIG_AROUND,
    WEIGHT_BIG_INCORNER,
    WEIGHT_BIG_INSIDE,
    WEIGHT_BIG_NEAR,

    WEIGHT_DEPTH,
};

float WeightTable[][13] = { 
    //{   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    //{   1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    1.0,    },  // ALL_AROUND
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR
    //{   0.5,    0.5,    1.0,    1.0,    1.0,    1.2,    0.8,    0.5,    0.2,    0.2,    0.2,    1.0,    0.5,    },  // BIG_NEAR

    //  0       2       4       8       16      32      64      128     256     512     1024    2048    4096
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BLANK
    {   2.0,    2.0,    2.0,    2.0,    1.5,    1.2,    1.0,    .75,    0.5,    0.5,    0.5,    0.5,    0.5,    },  // NEARBY 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // MAXNUM 
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    0.5,    1.0,    1.5,    1.5,    2.5,    3.0,    },  // MERGE 
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BLANK_NEAR_2_OR_4 
    {   .25,    .25,    .25,    .25,    .25,    .20,    .15,    .10,    .10,    .05,    .05,    .01,    .25,    },  // WAVE
                                   
    {   0.0,    1.0,    2.0,    3.0,    4.0,    6.0,    8.0,    10.0,   14.,    15.,    16.,    21.,    25.,    },  // CORNER_VALUE
    {   .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    .25,    },  // ALL_INSIDE
    {   .25,    .25,    .25,    .25,    .25,    .30,    .35,    .40,    .45,    .50,    0.45,   .25,    .25,    },  // ALL_AROUND
    {   .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .50,    .10,    },  // ALL_NEAR
                       
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_AROUND
    {   2,      4,      4,      3,      3,      3,      3,      3,      3,      2.5,    2.5,    2,      2,      },  // BIG_INCORNER
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_INSIDE
    {   1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      },  // BIG_NEAR

    {   1,      .05,    .05,    .05,    .05,    .05,    .05,    .05,    1,      1,      1,      1,      1,      },  // DEPTH
};

#define AICheckInRegion(x,h)        ((x)<(h) && (x)>=0)
#define AICheckInRegionXY(x,y,w,h)  ((x)<(h) && (x)>=0 && (y)<(w) && (y)>=0)

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

#undef max
#undef min

static int AIIsHaveNum(int (*map)[5],int w,int h,int num){
    forp(x,h)forp(y,w){
        if(map[x][y] == num)return 1;
    }
    return 0;
}

#define GameBlockUnite(F,S) if(*S != 0){if(*F == 0){*F = *S;*S = 0;fHadmove = 1;}else if(*F == *S){*F = *F+1;*S = 0;fHadmove = 1;fMerge += *F;break;}else break;}

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

static int AIFindMaxNum(const int (*map)[5],int w,int h){
    int max = 0;
    forp(x,h)forp(y,w){
        if(map[x][y] > max){
            max = map[x][y];
        }
    }
    return max;
}

static int AIFindOneNumXY(const int(*map)[5],int w,int h,int *px,int *py,int max){
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
    int len = AIFindOneNumXY(map,w,h,&x,&y,AIFindMaxNum(map,w,h));
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
    int len = AIFindOneNumXY(map,w,h,&x,&y,AIFindMaxNum(map,w,h));
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

static int AICheckBigNumAround(const int (*map)[5],int w,int h){
    int sum = 0;
    int maxnum = AIFindMaxNum(map,w,h);
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
        len += AIFindOneNumXY(map,w,h,&x[len],&y[len],max--);
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
            len += AIFindOneNumXY(map,w,h,&x[len],&y[len],find--);
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

static int AICheckBlankNear2or4(int (*map)[5],int w,int h){
    int sum = 0;
    int x[40] = {0},y[40] = {0};
    int len = AIFindOneNumXY(map,w,h,x,y,0);
    forp(i,len){
        sum += AICheckAroundIsNum(map,w,h,x[i],y[i],1);
        sum += AICheckAroundIsNum(map,w,h,x[i],y[i],2);
        sum += AICheckAroundIsNum(map,w,h,x[i],y[i],0);
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

    v = AICheckAllNearby(mNextMap,w,h) * WeightTable[WEIGHT_NEARBY][max];
    sum += v;
    //debug("WEIGHT_NEARBY: %g",v);

    v = AICheckAllNear(mNextMap,w,h) * WeightTable[WEIGHT_ALL_NEAR][max];
    sum += v;
    //debug("WEIGHT_ALL_NEAR: %g",v);

    v = AICheckBlankNear2or4(mNextMap,w,h) * WeightTable[WEIGHT_BLANK_NEAR_2_OR_4][max];
    sum += v;
    //debug("WEIGHT_BLANK_NEAR_2_OR_4: %g",v);

    v = AICheckWave(mNextMap,w,h) * WeightTable[WEIGHT_WAVE][max];
    sum += v;
    //debug("WEIGHT_WAVE: %g",v);

    return (int)sum;
}

int AICheckTree(const int(*map)[5],int w,int h,int depth){
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

            //v = AICheckAllNearby(mNextMap,w,h) * WeightTable[WEIGHT_NEARBY][max];
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

int AI1(int map[5][5],int w,int h){
    debug("--------------------AI--------------------");
    // some AI, I hadn't named it :)
    int x[5] = {-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF};
    int mNextMap[5][5];
    int mCurMap[5][5];

    //assert(WeightTable[WEIGHT_CORNER_VALUE][0]==16);
    //assert(WeightTable[WEIGHT_BIG_INSIDE][0]==0);

    for(int i=1;i<=4;i++){
        memcpy(mNextMap,map,sizeof(mNextMap));
        float v = AICheckIfDir(mNextMap,w,h,i) * WEIGHT_MERGE;
        if(v)
        {
            x[i] = 0;
            AIDebugPrintDir("check dir",i);
            
            x[i] += v;
            debug("WEIGHT_MERGE: %g",v);
            
            x[i] += AICheckAll(mNextMap,w,h);
            
            int v = AICheckTree(mNextMap,w,h,2);
            x[i] += v;
            debug("Tree get %d point",v);

            //x[i] += AICheckTree(mNextMap,w,h,1);
        }
    }

    int maxpoint = -0xFFFF;
    int dir = 0;
    for(int i=1;i<=4;i++){
        if(x[i] == -0xFFFF){
            debug("x[%d] get null points",i);
            continue;
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


// 24645

// 26953