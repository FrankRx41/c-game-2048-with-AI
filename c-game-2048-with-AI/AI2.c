#include "Macro.h"
#include "Game.h"

/*
per blank + 1 point
*/

static int AIDebugPrintGo(int dir){
    switch(dir){
    case DIR_UP:
        debug("AI go: Up");
        break;
    case DIR_DOWN:
        debug("AI go: Down");
        break;
    case DIR_LEFT:
        debug("AI go: Left");
        break;
    case DIR_RIGHT:
        debug("AI go: Right");
        break;
    default:
        debug("AI go error!");
        break;
    }
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
    debug("%s: %d",__FUNCTION__,sum);
    return sum;
}

int AI2(int map[5][5],int w,int h){
    debug("--------------------AI--------------------");
    // some AI, I hadn't named it :)
    int x[5] = {0,0,0,0,0};
    int map2[5][5];

    for(int i=1;i<=4;i++){
        memcpy(map2,map,sizeof(map2));
        if(CheckIfDir(map2,w,h,i)){
            x[i] += AICheckBlank(map2,w,h);
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
    return AIDebugPrintGo(dir);
}



