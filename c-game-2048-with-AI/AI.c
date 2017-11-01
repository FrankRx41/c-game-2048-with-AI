#include "Macro.h"
#include "Game.h"
#include "AI.h"

int AI0(int map[5][5],int w,int h){
    // random AI
    int dir = rand()%4+1;
    int map2[5][5];
    memcpy(map2,map,sizeof(map2));
    while(1){
        int v = CheckIfDir(map2,w,h,dir);
        debug("random dir: %d v:%d",dir,v);
        if(v){
            break;
        }
        memcpy(map2,map,sizeof(map2));
        dir = rand()%4+1;
    }
    return dir;
}

int AI1(int map[5][5],int w,int h){
    // TODO: add your code here
    // example: 
    return rand()%4;
}

int AI2(int map[5][5],int w,int h){
    // some AI, I hadn't named it :)
    int x[5] = {-1,-1,-1,-1,-1};
    int map2[5][5];

    memcpy(map2,map,sizeof(map2));
    if(CheckIfDir(map2,w,h,DIR_UP)){
        x[1] = CheckBlank(map2,w,h);
    }

    memcpy(map2,map,sizeof(map2));
    if(CheckIfDir(map2,w,h,DIR_DOWN)){
        x[2] = CheckBlank(map2,w,h);
    }

    memcpy(map2,map,sizeof(map2));
    if(CheckIfDir(map2,w,h,DIR_LEFT)){
        x[3] = CheckBlank(map2,w,h);
    }

    memcpy(map2,map,sizeof(map2));
    if(CheckIfDir(map2,w,h,DIR_RIGHT)){
        x[4] = CheckBlank(map2,w,h);
    }

    int max = x[0];
    int k = 0;
    for(int i=1;i<5;i++){
        if(max < x[i]){
            max = x[i];
            k = i;
        }
        debug("x[%d]=%d k=%d",i,x[i],k);
    }
    debug("AI go: %d",k);
    return k;
}


