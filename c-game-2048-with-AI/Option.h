#ifndef _OPTION_H_
#define _OPTION_H_

#include <Windows.h>

#define TITLENAME   TEXT("2048 with AI")
#define CLASSNAME   TEXT("2048GAMEWITHAI")

enum{
    GS_RUNNING  = 0x0001,
    GS_OVER     = 0x0000,
    GS_AIPLAY   = 0x4000,
    GS_PAUSE    = 0x8000,
};

typedef struct TILE{
    int x;
    int y;
    int v;
}TILE;

typedef struct OPTION{
    // game play info
    unsigned int nWidth;                // must be 4
    unsigned int nHeight;               // 3 - 5
    unsigned int nCurScore;
    unsigned int iLevel;
    unsigned int nStep;
    int mMap[5][5];

    // game loop info
    int iGameState;

    // about AI
    int iAISleep;
    int iCurAI;

    // game setting
    int fSound;
    int fFullScreen;                    // abandon
    int fSuggestion;
    int fGaryMode;
    int fAnimation;

    // UI
    int     nColorTable[21];
    int     nBgColor;
    int     nTextColor;
    char    hFontName[30];
    int     iFontSize;

    int     nTileWidth;
    int     nMargin;
    int     nRound;
    int     nInfoBarHeigh;

    long    iRandseek;
    char    sSaveDate[30];

    // window setting
    HWND    hWnd;
    int     cxClient;
    int     cyClient;
    int     nWinPosX;
    int     nWinPosY;
    
    // Key
    int     vKeyUp;
    int     vKeyDown;
    int     vKeyLeft;
    int     vKeyRight;
    int     vKeyEscape;
    int     vKeyPause;
    int     vKeyAISpeedUp;
    int     vKeyAISpeedDown;

    // score
    unsigned int nScore[3];

    // animation
    TILE    tLast;
    TILE    tMergeForm[10];
    TILE    tMergeTo[10];
    int     iAnimationSpeed;

}* LPOPTION, OPTION;

int PrintOption(LPOPTION lpOption);
int ReadOption(LPOPTION lpOption);
int SaveOption(LPOPTION lpOption);

#endif