#include "Macro.h"
#include "Game.h"

/*
per blank + 1 point
per nearby + 1 point
*/

#define WEIGHT_BLANK    1
#define WEIGHT_NEARBY   0.5

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

static int AICheckInRegion(int x,int y,int w,int h){

}

#define AICheckInRegion(x,h)    ((x)<(h) && (x)>=0)

static int AICheckOneNearby(const int(*map)[5],int w,int h,int x,int y){
    int sum = 0;
    sum += (AICheckInRegion(x-1,h) && map[x-1][y] == map[x][y]);
    sum += (AICheckInRegion(x+1,h) && map[x+1][y] == map[x][y]);
    sum += (AICheckInRegion(y-1,w) && map[x][y-1] == map[x][y]);
    sum += (AICheckInRegion(y+1,w) && map[x][y+1] == map[x][y]);
}

static int AICheckAllNearby(const int(*map)[5],int w,int h){
    int sum = 0;
    forp(x,h)forp(y,w){
        if(map[x][y]!=0)
            sum += AICheckOneNearby(map,w,h,x,y);
    }
    return sum;
}

int AI1(int map[5][5],int w,int h){
    debug("--------------------AI--------------------");
    // some AI, I hadn't named it :)
    int x[5] = {0,0,0,0,0};
    int v = 0;
    int map2[5][5];

    for(int i=1;i<=4;i++){
        memcpy(map2,map,sizeof(map2));
        if(CheckIfDir(map2,w,h,i)){
            AIDebugPrintDir("check dir",i);
            v = AICheckBlank(map2,w,h) * WEIGHT_BLANK;
            x[i] += v;
            debug("WEIGHT_BLANK: %d",v);
            v = AICheckAllNearby(map2,w,h) * WEIGHT_NEARBY;
            x[i] += v;
            debug("WEIGHT_NEARBY: %d",v);
        }
    }

    int maxpoint = 0;
    int dir = 0;
    for(int i=1;i<=4;i++){
        if(x[i] == 0)continue;
        if(maxpoint < x[i]){
            maxpoint = x[i];
            dir = i;
        }
        debug("x[%d] get %d points",i,x[i]);
    }
    debug("--------------------AI--------------------");
    return AIDebugPrintDir("Goto",dir);
}


