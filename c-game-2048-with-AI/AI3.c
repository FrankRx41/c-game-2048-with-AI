#include "Macro.h"
#include "Game.h"
#include "AI.h"

int AI3(int map[5][5],int w,int h){
    debug("--------------------AI--------------------");
    // min blank
    int x[5] = {0,0,0,0,0};
    int map2[5][5];

    for(int i=1;i<=4;i++){
        memcpy(map2,map,sizeof(map2));
        if(AICheckIfDir(map2,w,h,i)){
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
    return AIDebugPrintDir("",dir);
}



