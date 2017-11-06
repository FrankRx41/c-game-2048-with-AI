#include <windows.h>
#include <stdio.h>
#include "Option.h"
#include "Game.h"
#include "Window.h"
#include "AI.h"
#include "resource.h"
#include "Macro.h"

// return 1 mean window need update
int WinKeyHandle(LPOPTION lpOption,int key){
    // system key
    if(key == lpOption->vKeyEscape)
    {
        DestroyWindow(lpOption->hWnd);
        return 0;
    }
    else if(key == lpOption->vKeyAISpeedDown)
    {
        lpOption->iAISleep -= (lpOption->iAISleep>0) ? 150 : 0;
        return 1;
    }
    else if(key == lpOption->vKeyAISpeedUp)
    {
        lpOption->iAISleep += 150;
        return 1;
    }
    else if(key == lpOption->vKeyPause)
    {
        GamePause(lpOption);
        WinMenuReset(lpOption);
        return 1;
    }
    
    // you can't control game when AI play or game pause
    if(lpOption->iGameState & GS_AIPLAY)
    {
        return 0;
    }
    if(lpOption->iGameState & GS_PAUSE)
    {
        return 0;
    }
    
    // game key
    if(key == lpOption->vKeyUp)
    {
        return GameDirKey(lpOption,DIR_UP);
    }
    else if(key == lpOption->vKeyDown)
    {
        return GameDirKey(lpOption,DIR_DOWN);
    }        
    else if(key == lpOption->vKeyLeft)
    {
        return GameDirKey(lpOption,DIR_LEFT);
    }
    else if(key == lpOption->vKeyRight)
    {
        return GameDirKey(lpOption,DIR_RIGHT);
    }
    //debug("KEY: inefficacy (%d) (%X)",key);
    return 0;
}

static int CreateAndSelectFont(HDC hdc,LPCTSTR face,int high,int angle)
{
    static HFONT oldFont = NULL;
    DeleteObject(oldFont);
    HFONT hFont = CreateFont(high,0,angle,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_OUTLINE_PRECIS,
               CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,VARIABLE_PITCH,face);
    oldFont = SelectObject(hdc,hFont);
    return hFont;
}

static int GetTableColor(LPOPTION lpOption,int x){
    return lpOption->nColorTable[x];
}

static int DrawBackGround(LPOPTION lpOption,HDC hdc){
    int cxClient = lpOption->cxClient;
    int cyClient = lpOption->cyClient;
    RECT rt = {0,0,cxClient,cyClient};
    HBRUSH  hBrush;

    hBrush = CreateSolidBrush(lpOption->nBgColor);
    FillRect(hdc,&rt,hBrush);
    DeleteObject(hBrush);
}

static int DrawTiles(LPOPTION lpOption,HDC hdc){
    char szString[MAX] = {0};
    int cxClient = lpOption->cxClient;
    int cyClient = lpOption->cyClient;
    int margin  = lpOption->nMargin;
    int width   = lpOption->nTileWidth;
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;
    int (*map)[5] = lpOption->mMap;
    RECT rt = {0,0,cxClient,cyClient};
    HBRUSH  hBrush;

    SetBkMode(hdc,1);
    SetTextColor(hdc,lpOption->nTextColor);

    for(int x=0;x<h;x++)
    for(int y=0;y<w;y++)
    {
        rt.left     = y * width + margin;
        rt.right    = y * width + width - margin;
        rt.top      = x * width + margin;
        rt.bottom   = x * width + width - margin;

        hBrush = CreateSolidBrush(GetTableColor(lpOption,map[x][y]));
        SelectObject(hdc,hBrush);
        SelectObject(hdc,GetStockObject(NULL_PEN));
        RoundRect(hdc,rt.left,rt.top,rt.right,rt.bottom,lpOption->nRound,lpOption->nRound);

        if(map[x][y] != 0){
            CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
            sprintf(szString,"%d",1<<map[x][y]);
            DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }
        DeleteObject(hBrush);
    }
}

static int DrawHightScore(LPOPTION lpOption,HDC hdc){
#ifndef _DEBUG
    char szString[MAX] = {0};
    int cxClient = lpOption->cxClient,cyClient = lpOption->cyClient;
    int margin = lpOption->nMargin;
    int width = lpOption->nTileWidth;
    int w = lpOption->nWidth,h = lpOption->nHeight;
    RECT rt = {0,0,cxClient,cyClient};
    HBRUSH  hBrush;
    
    SetTextColor(hdc,RGB(240,225,0));
    CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
    rt.top      += h;
    rt.bottom   += h;
    sprintf(szString,"%s","Height  Score");
    DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    h = 25;
    rt.top      += h;
    rt.bottom   += h;
    //SetMyFont(hdc,TEXT("MSYaHei"),lpOption->iFontSize,0);
    forp(i,3){
        rt.top      += h;
        rt.bottom   += h;

        sprintf(szString,"%d  *  4  :             ",i+3);
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

        sprintf(szString,"                 %d",lpOption->nScore[i]);
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }
#endif
}

static int DrawInfo(LPOPTION lpOption,HDC hdc){
    char szString[MAX] = {0};
    int cxClient = lpOption->cxClient;
    int cyClient = lpOption->cyClient;
    int margin  = lpOption->nMargin;
    int width   = lpOption->nTileWidth;
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;
    RECT rt = {0,0,cxClient,cyClient};
    
    SetTextColor(hdc,RGB(230,230,230));
    sprintf(szString," Score: %d",lpOption->nCurScore);
    TextOut(hdc,0,cyClient-lpOption->nInfoBarHeigh,szString,strlen(szString));
    sprintf(szString," Steps: %d",lpOption->nStep);
    TextOut(hdc,lpOption->cxClient/2,cyClient-lpOption->nInfoBarHeigh,szString,strlen(szString));

    if(lpOption->iGameState == GS_OVER)
    {
        SetTextColor(hdc,RGB(250,35,45));
        //SetTextColor(hdc,RGB(230,120,155));
        rt.left     = 0;
        rt.right    = cxClient;
        rt.top      = 0;
        rt.bottom   = cyClient/2;
        CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize*2,0);
        sprintf(szString,"%s","Game Over");
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawHightScore(lpOption,hdc);
    }

    if(lpOption->iGameState & GS_PAUSE)
    {
        rt.left     = 0;
        rt.right    = cxClient;
        rt.top      = 0;
        rt.bottom   = cyClient - lpOption->nInfoBarHeigh;
        CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);

        sprintf(szString,"%s","Pause");
        SetTextColor(hdc,RGB(10,10,240));
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    if(lpOption->iGameState & GS_AIPLAY)
    {
        rt.left     = 0;
        rt.right    = cxClient;
        rt.top      = 0;
        rt.bottom   = 30;

        CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
        sprintf(szString,"%s%d%s","AI ",lpOption->iCurAI," Play");
        SetTextColor(hdc,RGB(240,10,10));
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

        CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize/2,0);
        sprintf(szString," AI sleep:%-4d",lpOption->iAISleep);
        SetTextColor(hdc,RGB(240,10,10));
        DrawText(hdc,szString,-1,&rt,NULL);
    }
}

static int DrawOneTileAnimation(LPOPTION lpOption,TILE tile,float deep,HDC hdc){
    int margin  = lpOption->nMargin;
    int width   = lpOption->nTileWidth;
    int(*map)[5] = lpOption->mMap;
    RECT rt = {0};
    HBRUSH  hBrush;
    int bg,fg,r,g,b;
    int x = tile.x, y = tile.y;
    char szString[MAX] = {0};

    if(x < 0 || y < 0)return 1;

    SetBkMode(hdc,1);
    SetTextColor(hdc,lpOption->nTextColor);

    rt.left     = y * width + margin;
    rt.right    = y * width + width - margin;
    rt.top      = x * width + margin;
    rt.bottom   = x * width + width - margin;


    bg = GetTableColor(lpOption,0);
    fg = GetTableColor(lpOption,map[x][y]);
    r = GetRValue(bg) - GetRValue(fg);
    g = GetGValue(bg) - GetGValue(fg);
    b = GetBValue(bg) - GetBValue(fg);
    hBrush = CreateSolidBrush( RGB( ((int)(r*deep)+GetRValue(fg)),
                                    ((int)(g*deep)+GetGValue(fg)),
                                    ((int)(b*deep)+GetBValue(fg))) );
    SelectObject(hdc,hBrush);
    SelectObject(hdc,GetStockObject(NULL_PEN));
    RoundRect(hdc,rt.left,rt.top,rt.right,rt.bottom,lpOption->nRound,lpOption->nRound);

    if(map[x][y] != 0){
        CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
        sprintf(szString,"%d",1<<map[x][y]);
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }
    DeleteObject(hBrush);
    //debug("on animation");
}

int DrawAnimation(LPOPTION lpOption,HDC hdc){
    static float deep = 1;

    KillTimer(lpOption->hWnd,TIMER_ANIMATION);

    DrawOneTileAnimation(lpOption,lpOption->tLast,deep,hdc);
    //DrawOneTileAnimation(lpOption,lpOption->tMergeFrom,deep,hdc);

    deep -= 0.05;

    if(deep > 0 && lpOption->fAnimation){
        SetTimer(lpOption->hWnd,TIMER_ANIMATION,lpOption->iAnimationSpeed,NULL);
    }else{
        deep = 1;
    }
}

int WinDraw(LPOPTION lpOption,HDC srchdc){
    int cxClient = lpOption->cxClient,cyClient = lpOption->cyClient;

    // create double buffer
    HDC hdc = CreateCompatibleDC(srchdc);
    HBITMAP hBmp;
    if(lpOption->fGaryMode){
        hBmp = CreateCompatibleBitmap(hdc,cxClient,cyClient);
    }else{
        hBmp = CreateCompatibleBitmap(srchdc,cxClient,cyClient);
    }
    SelectObject(hdc,hBmp);

    // main draw
    DrawBackGround(lpOption,hdc);
    DrawTiles(lpOption,hdc);
    DrawInfo(lpOption,hdc);
    if(lpOption->fAnimation){
        DrawAnimation(lpOption,hdc);
    }

    BitBlt(srchdc,0,0,cxClient,cyClient,hdc,0,0,SRCCOPY);

    DeleteObject(hBmp);
    DeleteDC(hdc);
    return 0;
}

static int __stdcall WinAboutDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam){
    switch(message){
    case WM_INITDIALOG:
        return 1;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg,0);
            return 1;
        }
        break;
    }
    return 0;
}

static int WinCheckAIMenu(LPOPTION lpOption){
    CheckMenuItem(GetMenu(lpOption->hWnd),MENU_AI_1,lpOption->iCurAI == 1 ? MF_CHECKED : 0);
    CheckMenuItem(GetMenu(lpOption->hWnd),MENU_AI_2,lpOption->iCurAI == 2 ? MF_CHECKED : 0);
    CheckMenuItem(GetMenu(lpOption->hWnd),MENU_AI_3,lpOption->iCurAI == 3 ? MF_CHECKED : 0);
    CheckMenuItem(GetMenu(lpOption->hWnd),MENU_AI_4,lpOption->iCurAI == 4 ? MF_CHECKED : 0);
    CheckMenuItem(GetMenu(lpOption->hWnd),MENU_AI_5,lpOption->iCurAI == 5 ? MF_CHECKED : 0);
}

// return 1 mean window need update
int WinOnMenu(LPOPTION lpOption,WPARAM wParam){
    switch(LOWORD(wParam)){
    case MENU_NEW_3:
    case MENU_NEW_4:
    case MENU_NEW_5:
        KillTimer(lpOption->hWnd,TIMER_AI);
        GameInit(lpOption,4,LOWORD(wParam) - MENU_NEW_3 + 3);
        return 1;
    case MENU_NEW_CUSTOMIZE:
        KillTimer(lpOption->hWnd,TIMER_AI);
        return 1;

    case MENU_GAME_SAVE:
        GameSave(lpOption);
        WinMenuReset(lpOption);
        debug("save");
        return 0;
    case MENU_GAME_LOAD:
        GameLoad(lpOption);
        debug("load");
        return 1;

    case MENU_GAME_PAUSE:
        // don't be confuse about this, because you check or uncheck the item before the game state update.
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),!(lpOption->iGameState&GS_PAUSE)?MF_CHECKED:0);
        return GamePause(lpOption);

    case MENU_GAME_EXIT:
        DestroyWindow(lpOption->hWnd);
        return 0;

    case MENU_OPTION_SOUND:
        lpOption->fSound = !lpOption->fSound;
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),lpOption->fSound?MF_CHECKED:0);
        return 0;
    case MENU_OPTION_FULLSCREEN:
        lpOption->fFullScreen = !lpOption->fFullScreen;
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),lpOption->fFullScreen?MF_CHECKED:0);
        return 0;
    case MENU_OPTION_SUGGESTION:
        lpOption->fSuggestion = !lpOption->fSuggestion;
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),lpOption->fSuggestion?MF_CHECKED:0);
        return 0;
    case MENU_OPTION_GARYMODE:
        lpOption->fGaryMode = !lpOption->fGaryMode;
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),lpOption->fGaryMode?MF_CHECKED:0);
        return 1;
    case MENU_OPTION_ANIMATION:
        lpOption->fAnimation = !lpOption->fAnimation;
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),lpOption->fAnimation?MF_CHECKED:0);
        return 0;

    case MENU_AI_1:
    case MENU_AI_2:
    case MENU_AI_3:
    case MENU_AI_4:
    case MENU_AI_5:
        wParam = LOWORD(wParam) + 1;
        if(lpOption->iGameState == GS_OVER)return 0;

        // stop AI
        if(lpOption->iCurAI == LOWORD(wParam) - MENU_AI_1){
            lpOption->iCurAI = -1;
            lpOption->iGameState -= GS_AIPLAY;
            KillTimer(lpOption->hWnd,0);
            debug("stop AI: %d",LOWORD(wParam) - MENU_AI_1);
        }
        // start AI or change AI
        else{
            lpOption->iCurAI = LOWORD(wParam) - MENU_AI_1;
            lpOption->iGameState |= GS_AIPLAY;
            SetTimer(lpOption->hWnd,TIMER_AI,lpOption->iAISleep,NULL);
            debug("start AI: %d",LOWORD(wParam) - MENU_AI_1);
        }
        //debug("AI: %d",LOWORD(wParam) - MENU_AI_0);
        WinCheckAIMenu(lpOption);
        return 1;

    case MENU_HELP_ABOUT:
        DialogBox(NULL,TEXT("ABOUTBOX"),lpOption->hWnd,WinAboutDlgProc);
        //return MessageBox(0,"EiSnow\n\n(C)CopyRight  2017.10.31","2048",0);
        return 0;
    default:
        ErrorMsg("UnDefine Menu Message!");
        return -1;
    }
}

int WinMenuReset(LPOPTION lpOption){
    HWND hWnd = lpOption->hWnd;
    HMENU hMenu = GetMenu(hWnd);
    int  cMenuItems;
    FILE  * fp;

    hMenu = GetSubMenu(hMenu,0);
    cMenuItems = GetMenuItemCount(hMenu);

    for(int nPos = 0; nPos < cMenuItems; nPos++){
        UINT id = GetMenuItemID(hMenu,nPos);

        switch(id){
        case MENU_OPTION_SOUND:
            CheckMenuItem(hMenu,id,lpOption->fSound?MF_CHECKED:0);
            break;
        case MENU_OPTION_FULLSCREEN:
            CheckMenuItem(hMenu,id,lpOption->fFullScreen?MF_CHECKED:0);
            EnableMenuItem(hMenu,id,MF_GRAYED);
            break;
        case MENU_OPTION_SUGGESTION:
            CheckMenuItem(hMenu,id,lpOption->fSuggestion?MF_CHECKED:0);
            EnableMenuItem(hMenu,id,MF_GRAYED);
            break;
        case MENU_OPTION_GARYMODE:
            CheckMenuItem(hMenu,id,lpOption->fGaryMode?MF_CHECKED:0);
            break;
        case MENU_OPTION_ANIMATION:
            CheckMenuItem(hMenu,id,lpOption->fAnimation?MF_CHECKED:0);
            break;

        case MENU_GAME_PAUSE:
            CheckMenuItem(hMenu,id,lpOption->iGameState&GS_PAUSE?MF_CHECKED:0);
            break;

        case MENU_GAME_SAVE:
            if(lpOption->iGameState == GS_OVER){
                EnableMenuItem(hMenu,id,MF_GRAYED);
            }else{
                EnableMenuItem(hMenu,id,MF_ENABLED);
            }
            break;

        case MENU_GAME_LOAD:
            fp = fopen(lpOption->sSaveDate,"r");
            if(fp == NULL){
                EnableMenuItem(hMenu,id,MF_GRAYED);
            }
            else{
                fclose(fp);
            }
            break;

        }
    }
    WinCheckAIMenu(lpOption);

    return 0;
}

int WinSysInit(LPOPTION lpOption,HWND hWnd){
    lpOption->hWnd = hWnd;
    lpOption->iGameState = GS_RUNNING;
    ReadOption(lpOption);
    WinMenuReset(lpOption);
    if(lpOption->nWinPosX!=-1 && lpOption->nWinPosY!=-1){
        MoveWindow(hWnd,lpOption->nWinPosX,lpOption->nWinPosY,0,0,FALSE);
    }
    return 1;
}

int WinOnTimerAI(LPOPTION lpOption){
    if(lpOption->iGameState & GS_PAUSE){
        return 0;
    }
    if(lpOption->iGameState == GS_OVER){
        KillTimer(lpOption->hWnd,TIMER_AI);
        lpOption->iCurAI = 0;
        WinMenuReset(lpOption);
        return 0;
    }

    // a new map for AI
    int map[5][5];

    memcpy(map,lpOption->mMap,sizeof(map));
    KillTimer(lpOption->hWnd,TIMER_AI);

    switch(lpOption->iCurAI){
    case 1:
        GameDirKey(
            lpOption,
            AI1(map,lpOption->nWidth,lpOption->nHeight)
            );
        break;
    case 2:
        GameDirKey(
            lpOption,
            AI2(map,lpOption->nWidth,lpOption->nHeight)
            );
        break;
    case 3:
        GameDirKey(
            lpOption,
            AI3(map,lpOption->nWidth,lpOption->nHeight)
            );
        break;
    case 4:
        GameDirKey(
            lpOption,
            AI4(map,lpOption->nWidth,lpOption->nHeight)
            );
        break;
    case 5:
        GameDirKey(
            lpOption,
            AI5(map,lpOption->nWidth,lpOption->nHeight)
            );
        break;
    default:
        ErrorMsg("AI Select Error!");
        KillTimer(lpOption->hWnd,TIMER_AI);
        return 0;
    }

    SetTimer(lpOption->hWnd,TIMER_AI,lpOption->iAISleep,NULL);
    return 1;
}