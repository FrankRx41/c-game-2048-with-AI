#ifndef _AI_H_
#define _AI_H_

int AI1(int map[5][5],int w,int h);
int AI2(int map[5][5],int w,int h);
int AI3(int map[5][5],int w,int h);
int AI4(int map[5][5],int w,int h);
int AI5(int map[5][5],int w,int h);
int AI6(int map[5][5],int w,int h);
int AI7(int map[5][5],int w,int h);

#include "Macro.h"

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
    if(fMerge)return fMerge;
    else return fHadmove;
}

static int AIDebugPrintDir(char *str,int dir){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);
    switch(dir){
        //
    case DIR_UP:
        debug("AI %s: ↑",str);
        break;
    case DIR_DOWN:
        debug("AI %s: ↓",str);
        break;
    case DIR_LEFT:
        debug("AI %s: ←",str);
        break;
    case DIR_RIGHT:
        debug("AI %s: →",str);
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

#endif