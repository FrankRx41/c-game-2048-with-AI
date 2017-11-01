#ifndef _OPTION_H_
#define _OPTION_H_

#include <Windows.h>

enum{
    /*GS_OP,GS_ED,GS_HEIGHT_SCORE,*/
    GS_RUNNING  = 0x0001,
    GS_OVER     = 0x0000,
    GS_ONAI     = 0x4000,
    GS_PAUSE    = 0x8000,
};

typedef struct OPTION{
    // Game Setting
    unsigned int nWidth;
    unsigned int nHeight;
    unsigned int nCurScore;
    unsigned int iLevel;
    unsigned int nStep;
    int nMap[5][5];

    int iGameState;
    int AI[3];
    int iAISleep;

    // System Setting
    int fSound;
    int fFullScreen;                    // XXX: delete it?
    int fSuggestion;
    int fGaryMode;
    int fAnimation;

    HWND    hWnd;
    int     nColorTable[21];
    int     nBgColor;
    int     nTextColor;
    char    hFontName[30];
    int     iFontSize;
    int     cxClient;
    int     cyClient;
    int     nPadding;
    int     nCorrect;
    char    sSaveDate[30];

    // Key
    int     key_up;
    int     key_down;
    int     key_left;
    int     key_right;
    int     key_esc;
    int     key_pause;

    // score
    unsigned int nScore[3];
}* LPOPTION, OPTION;

void PrintOption(LPOPTION lpOption);
int ReadOption(LPOPTION lpOption);
int SaveOption(LPOPTION lpOption);

#endif