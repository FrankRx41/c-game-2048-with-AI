#include "Macro.h"
#include "Game.h"
#include "AI.h"

int AI1(int map[5][5],int w,int h){
    // random AI
    int dir = rand()%4+1;
    int map2[5][5];
    memcpy(map2,map,sizeof(map2));
    while(1){
        int v = AICheckIfDir(map2,w,h,dir);
        debug("random dir: %d v:%d",dir,v);
        if(v){
            break;
        }
        memcpy(map2,map,sizeof(map2));
        dir = rand()%4+1;
    }
    return dir;
}
