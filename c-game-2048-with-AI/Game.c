#include "Game.h"
#include "Macro.h"
#include <stdio.h>

#pragma comment(lib,"winmm.lib")

#ifdef  _DEBUG
#define _DEBUG_2_1_4_1
#define _DEBUG_SHOW
#endif

int GameWatchMap(LPOPTION lpOption){
#ifdef _DEBUG
    int (*map)[5] = lpOption->mMap;
    for(int y=0;y<lpOption->nHeight;y++)
    {
        debug("[  %-4.d %-4.d %-4.d %-4.d]",map[y][0],map[y][1],map[y][2],map[y][3]);
    }
#endif
    return 0;
}

int GameSave(LPOPTION lpOption){
    FILE * fp = fopen(lpOption->sSaveDate,"wb");
    if(!fp){
        ErrorMsg("Create Save File Error!");
        return -1;
    }

    fwrite(&lpOption->nWidth,sizeof(lpOption->nWidth),1,fp);
    fwrite(&lpOption->nHeight,sizeof(lpOption->nHeight),1,fp);
    fwrite(&lpOption->nCurScore,sizeof(lpOption->nCurScore),1,fp);
    fwrite(&lpOption->iLevel,sizeof(lpOption->iLevel),1,fp);
    fwrite(&lpOption->nStep,sizeof(lpOption->nStep),1,fp);
    fwrite(&lpOption->mMap,sizeof(lpOption->mMap),1,fp);
    fclose(fp);
    return 0;
}

int GameLoad(LPOPTION lpOption){
    FILE * fp = fopen(lpOption->sSaveDate,"rb");
    if(!fp){
        ErrorMsg("Read Save File Error!");
        return -1;
    }
    int iLevel = lpOption->iLevel;
    //debug("iLevel: %d",iLevel);
    fread(&lpOption->nWidth,sizeof(lpOption->nWidth),1,fp);
    fread(&lpOption->nHeight,sizeof(lpOption->nHeight),1,fp);    
    fread(&lpOption->nCurScore,sizeof(lpOption->nCurScore),1,fp);
    fread(&lpOption->iLevel,sizeof(lpOption->iLevel),1,fp);
    fread(&lpOption->nStep,sizeof(lpOption->nStep),1,fp);
    fread(&lpOption->mMap,sizeof(lpOption->mMap),1,fp);
    fclose(fp);

    // reset window if need
    GameSetWindow(lpOption,lpOption->nWidth,lpOption->nHeight);

    GameWatchMap(lpOption);
    lpOption->iGameState == GS_RUNNING;
    return 0;
}

static int CheckBlank(const int(*map)[5],int w,int h){
    int sum = 0;
    for(int x=0;x<h;x++){
        for(int y=0;y<w;y++){
            if(map[x][y] == 0)
                sum++;
        }
    }
    //debug("CheckBlank: %d",sum);
    return sum;
}

static int CheckNearby(const int(*map)[5],int w,int h,int x,int y,int dir){
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
        if(x+i < h && map[x+i][y] == map[x][y])sum++;
        break;
    case DIR_LEFT:
        i = 1;
        while(y-i >= 0 && map[x][y-i] == 0)i++;
        if(y-i >= 0 && map[x][y-i] == map[x][y])sum++;
        break;
    case DIR_RIGHT:
        i = 1;
        while(y+i < w && map[x][y+i] == 0)i++;
        if(y+i < w && map[x][y+i] == map[x][y])sum++;
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

static int CheckALLDirNearby(const int(*map)[5],int w,int h,int dir){
    int sum = 0;
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++)
            sum += CheckNearby(map,w,h,i,j,dir);
    //debug("CheckALLDirNearby: %d",sum);
    return sum;
}

static int CreatOneTile(LPOPTION lpOption){
    int (*map)[5] = lpOption->mMap;
    int x = 0,y = 0,v;

    while(
        x = rand()%lpOption->nWidth,
        y = rand()%lpOption->nHeight,
        map[x][y]!=0)
        ;
#ifdef _DEBUG_2_1_4_1_
    v = rand()%2+1;
#else
    v = (rand()%10) ? 1 : 2;
#endif
    map[x][y] = v;
    debug("create new (%d) in [%d,%d]",v,x,y);

    // old up score rule
    //lpOption->nCurScore += v;
    
    GameWatchMap(lpOption);

    // check is game over?
    if(!CheckALLDirNearby(map,lpOption->nWidth,lpOption->nHeight,0) && 
       !CheckBlank(map,lpOption->nWidth,lpOption->nHeight))
    {
        GameOver(lpOption);
    }
    return 1;
}

#define GameBlockUnite(F,S) if(*S != 0){                    \
                                if(*F == 0){                \
                                    *F = *S;                \
                                    *S = 0;                 \
                                    fMoved = 1;             \
                                }                           \
                                else if(*F == *S){          \
                                    *F = *F+1;              \
                                    *S = 0;                 \
                                    fMoved = 1;             \
                                    fMerge = 1;             \
                                    /* new up score rule */ \
                                    lpOption->nCurScore += 1<<*F;   \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }                               \
                            // i think this debug info will not be use any more
                            //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",j,i,map[j][i],k,i,map[k][i]);

int GameDirKey(LPOPTION lpOption,int dir){
    int fMoved = 0;
    int fMerge = 0;
    int(*map)[5] = lpOption->mMap;
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;

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

    if(fMoved){
        CreatOneTile(lpOption);
        lpOption->nStep++;
        if(lpOption->fSound){
            if(fMerge){
                //debug("playsound");
                PlaySound(TEXT("MERGESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }else{
                //PlaySound(TEXT("3.wav"),0,SND_FILENAME|SND_ASYNC);
                PlaySound(TEXT("CREATSOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }
        }
    }
    return fMoved;
}

int GamePause(LPOPTION lpOption){
    if(lpOption->iGameState == GS_OVER){
        return 0;
    }
    else{
        lpOption->iGameState ^= GS_PAUSE;
        if(lpOption->iGameState & GS_PAUSE){
            PlaySound(TEXT("PAUSESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
        }
        return 1;
    }
}

static int GameSetWindow(LPOPTION lpOption){
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;
    int padding = lpOption->nTileWidth;
    w = w * padding;
    h = h * padding + lpOption->nInfoBarHeigh;
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

static int GameSetMap(LPOPTION lpOption){
    // map init
    int (*map)[5] = lpOption->mMap;
    for(int i=0;i<lpOption->nWidth;i++){
        for(int j=0;j<lpOption->nHeight;j++){
            map[j][i] = 0;
        }
    }
#ifdef _DEBUG_SHOW_
    map[0][0] = 1;
    map[0][1] = 2;
    map[0][2] = 3;
    map[0][3] = 4;

    map[1][0] = 5;
    map[1][1] = 6;
    map[1][2] = 7;
    map[1][3] = 8;

    map[2][0] = 9;
    map[2][1] = 10;
    map[2][2] = 11;
    map[2][3] = 12;

    map[3][0] = 13;
    map[3][1] = 14;
    map[3][2] = 15;
    map[3][3] = 16;

    map[4][0] = 17;
    map[4][1] = 18;
    map[4][2] = 19;
    map[4][3] = 20;
#else
    srand(lpOption->iRandseek);
    loop(2){
        CreatOneTile(lpOption);
    }
#endif

}

int GameInit(LPOPTION lpOption,int w,int h){
    debug("game start %d * %d",w,h);

    lpOption->nWidth    = w;
    lpOption->nHeight   = h;
    lpOption->iLevel    = h-3;
    lpOption->nCurScore = 0;
    lpOption->nStep     = 0;
    lpOption->iCurAI    = -1;
    lpOption->iGameState = GS_RUNNING;

    GameSetWindow(lpOption);
    GameSetMap(lpOption);

    debug("--------------------init--------------------");
}

int GameOver(LPOPTION lpOption){
    if((lpOption->iGameState & 0x0F) & GS_RUNNING)
    {
        lpOption->iGameState = GS_OVER;
        // FIXME: statist score
        //if(lpOption->nCurScore > lpOption->nScore[lpOption->iLevel]){
        //    lpOption->nScore[lpOption->iLevel] = lpOption->nCurScore;
        //}
        //SaveOption(lpOption);
    }
    debug("--------------------over--------------------");
    return 1;
}
