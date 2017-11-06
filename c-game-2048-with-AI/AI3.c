#include "Macro.h"
#include "Game.h"

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
                                    fUniting = 1;           \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }

static int AICheckIfDir(int (*map)[5],int w,int h,int dir){
    int fHadmove = 0;
    int fUniting = 0;
    switch(dir){
    case DIR_UP:
        forp(i,w){
            for(int j=0;j<h-1;j++){
                int *F = &map[j][i];
                int *S = NULL;

                for(int k=j+1;k<h;k++){
                    S = &map[k][i];
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
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    }
    return fHadmove;
}

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
    return AIDebugPrintGo(dir);
}



