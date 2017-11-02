#include "Game.h"
#include "Macro.h"
#include <stdio.h>

#pragma comment(lib,"winmm.lib")

int GameWatchMap(LPOPTION lpOption){
    int (*map)[5] = lpOption->nMap;
    for(int y=0;y<lpOption->nWidth;y++){
        debug("[  %-4.d %-4.d %-4.d %-4.d]",map[y][0],map[y][1],map[y][2],map[y][3]);
    }
}

int GameSave(LPOPTION lpOption){
    FILE * fp = fopen(lpOption->sSaveDate,"wb");
    if(!fp){
        MessageBox(0,"Create Save File Error!",0,0);
        return 0;
    }

    fwrite(&lpOption->nWidth,sizeof(lpOption->nWidth),1,fp);
    fwrite(&lpOption->nHeight,sizeof(lpOption->nHeight),1,fp);
    fwrite(&lpOption->nCurScore,sizeof(lpOption->nCurScore),1,fp);
    fwrite(&lpOption->iLevel,sizeof(lpOption->iLevel),1,fp);
    fwrite(&lpOption->nStep,sizeof(lpOption->nStep),1,fp);
    fwrite(&lpOption->nMap,sizeof(lpOption->nMap),1,fp);
    fclose(fp);
    WinMenuInit(lpOption);
}

int GameLoad(LPOPTION lpOption){
    FILE * fp = fopen(lpOption->sSaveDate,"rb");
    if(!fp){
        MessageBox(0,"Read Save File Error!",0,0);
        return 0;
    }
    int iLevel = lpOption->iLevel;
    //debug("iLevel: %d",iLevel);
    fread(&lpOption->nWidth,sizeof(lpOption->nWidth),1,fp);
    fread(&lpOption->nHeight,sizeof(lpOption->nHeight),1,fp);
    fread(&lpOption->nCurScore,sizeof(lpOption->nCurScore),1,fp);
    fread(&lpOption->iLevel,sizeof(lpOption->iLevel),1,fp);
    fread(&lpOption->nStep,sizeof(lpOption->nStep),1,fp);
    fread(&lpOption->nMap,sizeof(lpOption->nMap),1,fp);
    //debug("lp->iLevel: %d",lpOption->iLevel);
    if(lpOption->iLevel != iLevel){
        int padding = lpOption->nTileWidth;
        int w = lpOption->nWidth * padding;
        int h = lpOption->nHeight * padding + 50;
        RECT rt = {0,0,w,h};
        AdjustWindowRectEx(&rt,WS_OVERLAPPEDWINDOW,TRUE,NULL);
        w = rt.right-rt.left;
        h = rt.bottom-rt.top;

        int x,y;
        RECT rect = {NULL};
        if(GetWindowRect(lpOption->hWnd,&rect)) {
            x = rect.left;
            y = rect.top;
        }
        MoveWindow(lpOption->hWnd,x,y,w,h,FALSE);
    }
    lpOption->iGameState |= GS_RUNNING;
    fclose(fp);
    WinMenuInit(lpOption);
}

static int GameCreatNewBlock(LPOPTION lpOption){
    int (*map)[5] = lpOption->nMap;
    int x = rand()%lpOption->nWidth;
    int y = rand()%lpOption->nHeight;

    while(map[x][y]!=0){
        x = rand()%lpOption->nWidth;
        y = rand()%lpOption->nHeight;
    }

    int v = rand()%2+1;
    /*if(rand()%5==0){
        v = 2;
    }else{
        v = 1;
    }*/
    if(map[x][y] == 0){
        map[x][y] = v;
    }
    debug("create new (%d) in [%d,%d]",v,x,y);
    //lpOption->nCurScore += v;
    
    GameWatchMap(lpOption);

    if(!CheckALLDirNearby(map,lpOption->nWidth,lpOption->nHeight,0) && !CheckBlank(map,lpOption->nWidth,lpOption->nHeight))
    {
        GameOver(lpOption);
    }

}

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
                                    lpOption->nCurScore += 1<<*F;   \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }

int GameDirKey(int dir,LPOPTION lpOption){

    int fHadmove = 0;
    int fUniting = 0;
    int(*map)[5] = lpOption->nMap;
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;

    switch(dir){
    case DIR_UP:
        forp(i,lpOption->nWidth){
            for(int j=0;j<lpOption->nHeight-1;j++){
                int *F = &map[j][i];
                int *S = NULL;

                for(int k=j+1;k<lpOption->nHeight;k++){
                    S = &map[k][i];
                    //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",j,i,map[j][i],k,i,map[k][i]);
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_DOWN:
        forp(y,lpOption->nWidth){
            for(int x=lpOption->nHeight-1;x>0;x--){
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
        forp(x,lpOption->nHeight){
            for(int y=0;y<lpOption->nWidth-1;y++){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y+1;i<lpOption->nWidth;i++){
                    S = &map[x][i];
                    //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_RIGHT:
        forp(x,lpOption->nHeight){
            for(int y=lpOption->nWidth-1;y>0;y--){
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

    if(fHadmove){
        GameCreatNewBlock(lpOption);
        lpOption->nStep++;
        if(lpOption->fSound){
            if(fUniting){
                //debug("playsound");
                PlaySound(TEXT("MERGESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }else{
                //PlaySound(TEXT("3.wav"),0,SND_FILENAME|SND_ASYNC);
                PlaySound(TEXT("CREATSOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }
        }
    }
    return fHadmove;
}

int GamePause(LPOPTION lpOption){
    if(lpOption->iGameState == GS_OVER)return 0;

    lpOption->iGameState ^= GS_PAUSE;
    WinMenuInit(lpOption);
    InvalidateRect(lpOption->hWnd,NULL,FALSE);
    if(lpOption->iGameState & GS_PAUSE){
        PlaySound(TEXT("PAUSESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
    }
}

int GameInit(LPOPTION lpOption,int w,int h){
    lpOption->nWidth    = w;
    debug("--------------------init--------------------");
    debug("game start %d * %d",w,h);

    lpOption->nHeight   = h;
    lpOption->iLevel    = h-3;
    lpOption->iGameState = GS_RUNNING;
    int (*map)[5] = lpOption->nMap;

    for(int i=0;i<lpOption->nWidth;i++){
        for(int j=0;j<lpOption->nHeight;j++){
            map[j][i] = 0;
        }
    }
    
    int padding = lpOption->nTileWidth;
    w = w * padding;
    h = h * padding + 50;
    RECT rt = {0,0,w,h};
    AdjustWindowRectEx(&rt,WS_OVERLAPPEDWINDOW,TRUE,NULL);
    w = rt.right-rt.left;
    h = rt.bottom-rt.top;

    int x,y;
    RECT rect = {NULL};
    if(GetWindowRect(lpOption->hWnd,&rect)) {
        x = rect.left;
        y = rect.top;
    }
    MoveWindow(lpOption->hWnd,x,y,w,h,FALSE);

#ifdef _DEBUG1
    /*map[0][0] = 2048;
        map[1][0] = 16    ;
        map[2][0] = 4     ;
        map[3][0] = 2     ;
        map[0][1] = 4     ;
        map[1][1] = 4     ;
        map[2][1] = 2     ;
        map[3][1] = 16    ;
        map[0][2] = 2     ;
        map[1][2] = 32    ;
        map[2][2] = 64    ;
        map[3][2] = 2     ;
        map[0][3] = 2     ;
        map[1][3] = 16    ;
        map[2][3] = 4     ;
        map[3][3] = 32    ;
    /*
    map[0][0] = 0;
    map[1][0] = 4;
    map[2][0] = 8;
    map[3][0] = 16;
    map[0][1] = 32;
    map[1][1] = 64;
    map[2][1] = 128;
    map[3][1] = 256;
    map[0][2] = 512;
    map[1][2] = 1024;
    map[2][2] = 2048;
    map[3][2] = 4096;
    map[0][3] = 8192;
    map[1][3] = 16384;
    map[2][3] = 32768;
    map[3][3] = 65536;*/
#else
    srand(lpOption->iRandseek);
    loop(2){
        while(x=rand()%lpOption->nHeight,y=rand()%lpOption->nWidth,map[x][y]!=0);
        int v = rand()%2+1;
        map[x][y] = v;
        debug("create new (%d) in [%d,%d]",v,x,y);
    }
#endif
    lpOption->nCurScore = 0;
    lpOption->nStep = 0;

    lpOption->AI[0] = 0;
    lpOption->AI[1] = 0;
    lpOption->AI[2] = 0;
    WinMenuInit(lpOption);

    InvalidateRect(lpOption->hWnd,NULL,TRUE);
    debug("--------------------init--------------------");
}

int GameOver(LPOPTION lpOption){
    if((lpOption->iGameState & 0x0F) & GS_RUNNING){

        lpOption->iGameState = GS_OVER;
        if(lpOption->nCurScore > lpOption->nScore[lpOption->iLevel]){
            lpOption->nScore[lpOption->iLevel] = lpOption->nCurScore;
        }
    }
    debug("______________GAME_OVER________________");

    SaveOption(lpOption);
}


/*
int GameMove(int dir,LPOPTION lpOption){
    if(CheckCanMove(dir,lpOption))
    {
        int w = lpOption->nWidth;
        int h = lpOption->nHeight;
        int (*map)[5] = lpOption->nMap;

        for(int i=0;i<w;i++){
            for(int j=0;j<h;j++){


                int *F = &map[j][i];
                int *S = NULL;

                for(int k=j+1;k<h;k++){
                    S = &map[k][i];
                    if(*S != 0){
                        if(*F == 0){
                            *F = *S;
                        }
                        else if(*F == *S){
                            *F = *F<<1;
                            *S = 0;
                            break;
                        }
                        else break;
                    }
                }

            }
        }


        return 1;
    }
    else{
        return 0;
    }
}
*/

static int CheckBlank(const int (*map)[5],int w,int h){
    int sum = 0;
    for(int x=0;x<h;x++){
        for(int y=0;y<w;y++){
            if(map[x][y] == 0)
                sum ++;
        }
    }
    //debug("CheckBlank: %d",sum);
    return sum;
}

static int CheckNearby(const int (*map)[5],int w,int h,int x,int y,int dir){
    if(map[x][y] == 0)return 4;

    int sum = 0;
    int i;
    switch(dir){
    case DIR_UP:
        i = 1;
        while(x-i >= 0 && map[x-i][y] == 0)i++;
        if(x-i >= 0 && map[x-i][y] == map[x][y])sum++;
        break;
    case DIR_DOWN:
        i = 1;
        while(x+i < h && map[x+i][y] == 0)i++;
        if( x+i < h && map[x+i][y] == map[x][y])sum++;
        break;
    case DIR_LEFT:
        i = 1;
        while(y-i >= 0 && map[x][y-i] == 0)i++;
        if( y-i >= 0 && map[x][y-i] == map[x][y])sum++;
        break;
    case DIR_RIGHT:
        i = 1;
        while(y+i < w && map[x][y+i] == 0)i++;
        if( y+i < w && map[x][y+i] == map[x][y])sum++;
        break;
    default:
        i = 1;
        while(x-i >= 0 && map[x-i][y] == 0)i++;
        if(x-i >= 0 && map[x-i][y] == map[x][y])sum++;
        i = 1;
        while(x+i < h && map[x+i][y] == 0)i++;
        if(x+i > h && map[x+i][y] == map[x][y])sum++;
        i = 1;
        while(y-i >= 0 && map[x][y-i] == 0)i++;
        if(y-i >= 0 && map[x][y-i] == map[x][y])sum++;
        i = 1;
        while(y+i < w && map[x][y+i] == 0)i++;
        if(y+i < w && map[x][y+i] == map[x][y])sum++;
        break;
    }
    //debug("dir: %d sum: %d",dir,sum);
    return sum;
}

static int CheckALLDirNearby(const int (*map)[5],int w,int h,int dir){
    int sum = 0;
    for(int i=0;i<h;i++)
    for(int j=0;j<w;j++)
        sum += CheckNearby(map,w,h,i,j,dir);
    //debug("CheckALLDirNearby: %d",sum);
    return sum;
}