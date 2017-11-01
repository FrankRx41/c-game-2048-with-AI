#include "Macro.h"
#include "Game.h"

static int CheckBlank(const int(*map)[5],int w,int h){
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
    debug("--------------------AI--------------------");
    return k;
}


