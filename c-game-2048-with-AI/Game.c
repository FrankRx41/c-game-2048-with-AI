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
    for(int x=0;x<lpOption->nHeight;x++)
    {
        printf("[  ");
        for(int y=0;y<lpOption->nWidth;y++)
        {
            if(x==lpOption->tLast.x && y==lpOption->tLast.y){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x02);
            }
            //for(int i=0;i<10;i++){
            //    if(x==lpOption->tMergeTo[i].x && y==lpOption->tMergeTo[i].y && map[x][y]!=0){
            //        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x01);
            //    }
            //}
            printf("%-4.d ",map[x][y]);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07);
        }
        printf("]\n");
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
    GameInit(lpOption,4,4);
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
    lpOption->iGameState = GS_RUNNING;
    return 1;
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

static int CheckNeighbor(const int(*map)[5],int w,int h,int x,int y,int dir){
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
            sum += CheckNeighbor(map,w,h,i,j,dir);
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
    lpOption->tLast.x = x;
    lpOption->tLast.y = y;
    //debug("create new (%d) in [%d,%d]",v,x,y);

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

int GameEgg(LPOPTION lpOption){
    static int egged = 0;
    lpOption->fEgg = 0;
    if(egged)return 0;
    forp(x,lpOption->nHeight)forp(y,lpOption->nHeight){
        if(lpOption->mMap[x][y] == 10){
            lpOption->fEgg = 1;
            egged = 1;
        }
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
    int aIndex = lpOption->iAnimationIndex = -1;

    switch(dir){
    case DIR_UP:
        forp(i,w){
            for(int j=0;j<h-1;j++){
                int *F = &map[j][i];
                int *S = NULL;

                for(int k=j+1;k<h;k++){
                    if(map[k][i] != 0 && (map[j][i]==0 || map[k][i]==map[j][i])){
                        aIndex++;
                        lpOption->tMergeTo[aIndex].x = j;
                        lpOption->tMergeTo[aIndex].y = i;
                        lpOption->tMergeForm[aIndex].x = k;
                        lpOption->tMergeForm[aIndex].y = i;
                        //debug("[%d,%d] [%d,%d]",j,i,k,i);
                    }
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
                    if(map[i][y] != 0 && (map[x][y]==0 || map[i][y]==map[x][y])){
                        aIndex++;
                        lpOption->tMergeTo[aIndex].x = x;
                        lpOption->tMergeTo[aIndex].y = y;
                        lpOption->tMergeForm[aIndex].x = i;
                        lpOption->tMergeForm[aIndex].y = y;
                        //debug("[%d,%d] [%d,%d]",x,y,i,y);
                    }
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
                    if(map[x][i] != 0 && (map[x][y]==0 || map[x][i]==map[x][y])){
                        aIndex++;
                        lpOption->tMergeTo[aIndex].x = x;
                        lpOption->tMergeTo[aIndex].y = y;
                        lpOption->tMergeForm[aIndex].x = x;
                        lpOption->tMergeForm[aIndex].y = i;
                        //debug("[%d,%d] [%d,%d]",x,y,x,i);
                    }
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
                    if(map[x][i] != 0 && (map[x][y]==0 || map[x][i]==map[x][y])){
                        aIndex++;
                        lpOption->tMergeTo[aIndex].x = x;
                        lpOption->tMergeTo[aIndex].y = y;
                        lpOption->tMergeForm[aIndex].x = x;
                        lpOption->tMergeForm[aIndex].y = i;
                        //debug("[%d,%d] [%d,%d]",x,y,x,i);
                    }
                    S = &map[x][i];
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    }

    lpOption->iAnimationIndex = aIndex;
    //debug("aIndex %d",aIndex+1);

    GameEgg(lpOption);

    if(fMoved){
        CreatOneTile(lpOption);
        lpOption->nStep++;
        if(lpOption->fSound){
            if(lpOption->iCurAI == 0 || (lpOption->iCurAI != 0 && lpOption->iAISleep > 200)){
                if(fMerge){
                    //debug("playsound");
                    PlaySound(TEXT("MERGESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
                }else{
                    //PlaySound(TEXT("3.wav"),0,SND_FILENAME|SND_ASYNC);
                    PlaySound(TEXT("CREATSOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
                }
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
        if(lpOption->iGameState & GS_PAUSE)
        {
            if(lpOption->fSound){
                PlaySound(TEXT("PAUSESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }
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

    int x = 0,y = 0;
    RECT rect = {0};
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
    //debug("game start %d * %d",w,h);

    lpOption->nWidth    = w;
    lpOption->nHeight   = h;
    lpOption->nCurScore = 0;
    lpOption->iLevel    = h-3;
    lpOption->nStep     = 0;
    lpOption->iCurAI    = 0;
    lpOption->iGameState = GS_RUNNING;
    lpOption->fEgg      = 0;

    GameSetWindow(lpOption);
    GameSetMap(lpOption);

    // animation
    lpOption->iAnimationSpeed = 10;
    lpOption->tLast.x = -1;
    lpOption->tLast.y = -1;
    lpOption->iAnimationIndex   = -1;

    debug("--------------------init--------------------");
}

int GameOver(LPOPTION lpOption){
    if((lpOption->iGameState & 0x0F) & GS_RUNNING)
    {
        lpOption->iGameState = GS_OVER;
        // statist score
        for(int i=0;i<3;i++)
        {
            if(lpOption->nCurScore > lpOption->nScore[lpOption->iLevel][i])
            {
                for(int j=2;j>i;j--){
                    lpOption->nScore[lpOption->iLevel][j] = lpOption->nScore[lpOption->iLevel][j-1];
                }
                lpOption->nScore[lpOption->iLevel][i] = lpOption->nCurScore;
                break;
            }
        }
        SaveOption(lpOption);
    }
    debug("--------------------over--------------------");
    return 1;
}

int GameLoadPhase(LPOPTION lpOption,int phase){
    GameInit(lpOption,4,4);

    int w = lpOption->nWidth = 4;
    int h = lpOption->nHeight = 4;
    lpOption->nCurScore = 0;
    lpOption->iLevel = 1;
    lpOption->nStep = 0;
    lpOption->iRandseek = 1024;

    phase += 1;
    static int map[61][4][4] = {
        // 0
        {
            16, 15, 14, 13,
            9,  10, 11, 12,
            8,  7,  6,  5,
            1,  2,  3,  4,
        },
        // the final phase of game 
        {
            11, 3,  5,  3,
            10, 6,  3,  1,
            9,  8,  5,  4,
            1,  1,  2,  0,
        },
        {
            6,  4,  4,  2,
            8,  7,  6,  1,
            10, 8,  4,  0,
            11, 5,  1,  1,
        },
        {
            0,  3,  1,  1,
            3,  4,  4,  2,
            2,  9,  8,  6,
            10, 1,  1,  7,
        },
        {
            10, 7,  5,  3,
            8,  5,  4,  2,
            4,  3,  2,  0,
            1,  2,  1,  0,
        },
        // test board
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        // 10
        {
            10, 8,  7,  6,
            5,  6,  5,  2,
            3,  2,  1,  0,
            2,  1,  0,  0,
        },
        {
            10, 9,  8,  6,
            5,  6,  3,  2,
            4,  1,  2,  0,
            2,  2,  0,  0,
        },
        {
            10, 9,  8,  2,
            3,  5,  3,  0,
            1,  3,  0,  0,
            2,  1,  0,  0,
        },
        {
            5,  4,  2,  0,
            4,  3,  0,  0,
            3,  0,  0,  0,
            1,  0,  0,  0,
        },
        {
            7,  4,  3,  0,
            5,  3,  2,  0,
            2,  0,  0,  0,
            1,  0,  0,  0,
        },
        {
            2,  2,  4,  5,
            0,  1,  3,  3,
            0,  0,  0,  1,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            2,  0,  0,  0,
            4,  2,  0,  0,
            6,  3,  2,  1,
        },
        {
            6,  1,  0,  0,
            5,  3,  0,  0,
            5,  0,  0,  0,
            2,  3,  2,  1,
        },
        {
            0,  3,  6,  9,
            0,  1,  3,  4,
            0,  0,  2,  3,
            0,  0,  0,  1,
        },
        {
            2,  4,  8,  9,
            0,  3,  5,  7,
            0,  1,  3,  5,
            1,  0,  0,  3,
        },
        // 20
        {
            3,  3,  9,  9,
            4,  7,  0,  1,
            0,  1,  1,  0,
            0,  0,  0,  0,
        },
        {
            5,  2,  0,  0,
            5,  3,  0,  0,
            1,  3,  0,  0,
            2,  1,  0,  0,
        },
        {
            0,  0,  1,  1,
            0,  2,  3,  7,
            0,  0,  4,  7,
            1,  3,  1,  10,
        },
        {
            5,  1,  1,  2,
            0,  2,  2,  4,
            0,  6,  5,  8,
            2,  0,  9,  10,
        },
        {
            1,  3,  4,  5,
            0,  0,  3,  7,
            0,  0,  2,  7,
            0,  0,  1,  8,
        },
        {
            5,  3,  1,  0,
            7,  6,  2,  1,
            8,  3,  4,  3,
            11, 2,  1,  2,
        },
        {
            10, 8,  6,  1,
            8,  7,  5,  2,
            7,  6,  3,  3,
            5,  3,  1,  1,
        },
        {
            9,  5,  2,  0,
            3,  2,  4,  0,
            1,  4,  0,  0,
            1,  2,  0,  1,
        },
        {
            9,  4,  3,  2,
            5,  4,  3,  2,
            1,  4,  3,  2,
            1,  5,  2,  1,
        },
        {
            1,  0,  0,  0,
            0,  0,  0,  2,
            2,  6,  3,  2,
            8,  4,  4,  2,
        },
        // 30
        {
            3,  2,  0,  0,
            3,  2,  1,  2,
            2,  1,  0,  0,
            2,  0,  0,  0,
        },
        {
            4,  3,  3,  1,
            3,  2,  1,  0,
            1,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  2,  0,
            1,  0,  0,  0,
            2,  1,  2,  0,
            5,  2,  1,  0,
        },
        {
            0,  0,  0,  1,
            0,  0,  2,  3,
            0,  0,  1,  2,
            0,  3,  7,  8,
        },
        {
            0,  0,  0,  1,
            3,  1,  0,  0,
            5,  2,  0,  0,
            7,  1,  3,  2,
        },
        {
            10, 7,  7,  3,
            0,  0,  1,  4,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        // 40
        {
            9,  8,  7,  6,
            2,  3,  4,  5,
            1,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            9,  8,  7,  6,
            5,  4,  3,  2,
            1,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            9,  4,  3,  2,
            8,  7,  6,  5,
            1,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            9,  2,  0,  0,
            1,  8,  3,  4,
            0,  0,  7,  5,
            0,  0,  0,  6,
        },
        {
            9,  9,  8,  6,
            7,  6,  5,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            9,  9,  8,  8,
            7,  7,  6,  6,
            5,  4,  0,  0,
            5,  4,  0,  0,
        },
        {
            8,  9,  7,  2,
            6,  4,  5,  3,
            0,  0,  0,  1,
            0,  0,  0,  0,
        },
        {
            9,  7,  8,  5,
            6,  4,  3,  1,
            2,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            5,  9,  7,  6,
            8,  4,  2,  3,
            1,  0,  0,  0,
            0,  0,  0,  0,
        },
        {
            9,  6,  3,  1,
            7,  4,  5,  8,
            2,  0,  0,  0,
            0,  0,  0,  0,
        },
        // 50
        {
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
            0,  0,  0,  0,
        },
        
    };
    lpOption->iAISleep = 300;
    memset(lpOption->mMap,0,sizeof(lpOption->mMap));
    forp(x,h)forp(y,w){
        lpOption->mMap[x][y] = map[phase][x][y];
    }
    debug("phase: %d",phase);


    // reset window if need
    GameSetWindow(lpOption,lpOption->nWidth,lpOption->nHeight);

    GameWatchMap(lpOption);
    lpOption->iGameState = GS_RUNNING;
    return 1;
}