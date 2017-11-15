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

static HFONT CreateAndSelectFont(HDC hdc,LPCTSTR face,int high,int angle)
{
    HFONT hFont = CreateFont(high,0,angle,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_OUTLINE_PRECIS,
               CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,VARIABLE_PITCH,face);
    SelectObject(hdc,hFont);
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

static int DrawOneTile(LPOPTION lpOption,RECT rt,int x,int y,float deep,HDC hdc){
    char szString[MAX] = {0};
    HBRUSH  hBrush;
    int (*map)[5] = lpOption->mMap;
    int bg,fg,r,g,b;

    bg = GetTableColor(lpOption,0);
    fg = GetTableColor(lpOption,map[x][y]);
    r = GetRValue(bg) - GetRValue(fg);
    g = GetGValue(bg) - GetGValue(fg);
    b = GetBValue(bg) - GetBValue(fg);
    hBrush = CreateSolidBrush(RGB(  ((int)(r*deep)+GetRValue(fg)),
                                    ((int)(g*deep)+GetGValue(fg)),
                                    ((int)(b*deep)+GetBValue(fg))));

    //hBrush = CreateSolidBrush(GetTableColor(lpOption,map[x][y]));

    SelectObject(hdc,hBrush);
    SelectObject(hdc,GetStockObject(NULL_PEN));
    RoundRect(hdc,rt.left,rt.top,rt.right,rt.bottom,lpOption->nRound,lpOption->nRound);

    if(map[x][y] != 0){
        HFONT hFont = CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
        sprintf(szString,"%d",1<<map[x][y]);
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DeleteObject(hFont);
    }
    DeleteObject(hBrush);

}

static int DrawTiles(LPOPTION lpOption,HDC hdc){
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;
    RECT rt = {0,0,0,0};
    int margin  = lpOption->nMargin;
    int width   = lpOption->nTileWidth;

    SetBkMode(hdc,1);
    SetTextColor(hdc,lpOption->nTextColor);

    for(int x=0;x<h;x++)
    for(int y=0;y<w;y++)
    {
        rt.left     = y * width + margin;
        rt.right    = y * width + width - margin;
        rt.top      = x * width + margin;
        rt.bottom   = x * width + width - margin;

        DrawOneTile(lpOption,rt,x,y,0,hdc);
    }
}

static int DrawHightScore(LPOPTION lpOption,HDC hdc){
    char szString[MAX] = {0};
    int cxClient = lpOption->cxClient,cyClient = lpOption->cyClient;
    int margin = lpOption->nMargin;
    int width = lpOption->nTileWidth;
    int w = lpOption->nWidth,h = lpOption->nHeight;
    RECT rt = {0,0,cxClient,cyClient};
    
    SetTextColor(hdc,RGB(240,225,0));
    HFONT hFont = CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
    rt.top      += h;
    rt.bottom   += h;
    sprintf(szString,"%s","High  Score");
    DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    h = 25;
    rt.top      += h;
    rt.bottom   += h;
    //SetMyFont(hdc,TEXT("MSYaHei"),lpOption->iFontSize,0);
    forp(i,3){
        rt.top      += h;
        rt.bottom   += h;

        //sprintf(szString,"%d  *  4  :             ",i+3);
        //DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

        //sprintf(szString,"                 %d",lpOption->nScore[i]);
        
        sprintf(szString,"%d",lpOption->nScore[lpOption->iLevel][i]);
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }
    DeleteObject(hFont);
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
    HFONT hFont;
    
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
        hFont = CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize*2,0);
        sprintf(szString,"%s","Game Over");
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DrawHightScore(lpOption,hdc);
        DeleteObject(hFont);
    }

    if(lpOption->iGameState & GS_PAUSE)
    {
        rt.left     = 0;
        rt.right    = cxClient;
        rt.top      = 0;
        rt.bottom   = cyClient - lpOption->nInfoBarHeigh;
        hFont = CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);

        sprintf(szString,"%s","Pause");
        SetTextColor(hdc,RGB(10,10,240));
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DeleteObject(hFont);
    }

    if(lpOption->iGameState & GS_AIPLAY)
    {
        rt.left     = 0;
        rt.right    = cxClient;
        rt.top      = 0;
        rt.bottom   = 30;

        hFont = CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);
        sprintf(szString,"%s%d%s","AI ",lpOption->iCurAI," Play");
        SetTextColor(hdc,RGB(240,10,10));
        DrawText(hdc,szString,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        DeleteObject(hFont);

        hFont = CreateAndSelectFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize/2,0);
        sprintf(szString," AI sleep:%-4d",lpOption->iAISleep);
        SetTextColor(hdc,RGB(240,10,10));
        DrawText(hdc,szString,-1,&rt,NULL);
        DeleteObject(hFont);
    }
}

static int DrawTileAppear(LPOPTION lpOption,TILE tile,float deep,HDC hdc){
    int margin  = lpOption->nMargin;
    int width   = lpOption->nTileWidth;
    int(*map)[5] = lpOption->mMap;
    RECT rt = {0};
    int x = tile.x, y = tile.y;
    char szString[MAX] = {0};

    if(x < 0 || y < 0)return 1;

    SetBkMode(hdc,1);
    SetTextColor(hdc,lpOption->nTextColor);

    rt.left     = y * width + margin;
    rt.right    = y * width + width - margin;
    rt.top      = x * width + margin;
    rt.bottom   = x * width + width - margin;

    DrawOneTile(lpOption,rt,x,y,deep,hdc);
}

static int DrawTilesMerge(LPOPTION lpOption,TILE from[],TILE to[],int index,float deep,HDC hdc){
    float len;
    int margin  = lpOption->nMargin;
    int width   = lpOption->nTileWidth;
    RECT rt = {0};

    //debug("Ani:%d",index);
    forp(i,index+1){
        // l - r
        if(from[i].x == to[i].x){
            len = (to[i].y-from[i].y) * (width);
            len *= (1-deep);
            //debug("len: %f",len);
            rt.left     = from[i].y * width + margin + len;
            rt.right    = from[i].y * width + width - margin + len;
            rt.top      = from[i].x * width + margin;
            rt.bottom   = from[i].x * width + width - margin;
            DrawOneTile(lpOption,rt,to[i].x,to[i].y,0,hdc);
        }
        // u - d
        else if(from[i].y == to[i].y){
            len = (to[i].x-from[i].x) * (width);
            len *= (1-deep);
            //debug("[%d,%d] len: %f",form[i].x,form[i].y,len);
            rt.left     = from[i].y * width + margin;
            rt.right    = from[i].y * width + width - margin;
            rt.top      = from[i].x * width + margin + len;
            rt.bottom   = from[i].x * width + width - margin + len;
            //debug("%d %d",rt.top,rt.bottom);
            DrawOneTile(lpOption,rt,to[i].x,to[i].y,0,hdc);
        }

    }
}

static float deep = 1;
int DrawAnimation(LPOPTION lpOption,HDC srchdc){
    int cxClient = lpOption->cxClient,cyClient = lpOption->cyClient;
    HDC hdc;
    HBITMAP hBmp;

    KillTimer(lpOption->hWnd,TIMER_ANIMATION);

    if(lpOption->iGameState == GS_OVER){
        return 0;
    }

    hdc = CreateCompatibleDC(srchdc);
    if(lpOption->fGaryMode){
        hBmp = CreateCompatibleBitmap(hdc,cxClient,cyClient);
    }
    else{
        hBmp = CreateCompatibleBitmap(srchdc,cxClient,cyClient);
    }
    SelectObject(hdc,hBmp);

    static HDC hdc2;
    static HBITMAP hBmp2;
    if(deep == 1){
        hdc2 = CreateCompatibleDC(srchdc);
        hBmp2 = CreateCompatibleBitmap(srchdc,cxClient,cyClient);
        SelectObject(hdc2,hBmp2);
        BitBlt(hdc2,0,0,cxClient,cyClient,srchdc,0,0,SRCCOPY);
        //debug("DrawAnimation");
    }
    else if(deep <= 0){
        BitBlt(hdc,0,0,cxClient,cyClient,hdc2,0,0,SRCCOPY);
        BitBlt(srchdc,0,0,cxClient,cyClient,hdc,0,0,SRCCOPY);

        DeleteObject(hBmp2);
        DeleteDC(hdc2);

        DeleteObject(hBmp);
        DeleteDC(hdc);

        deep = 1;
        lpOption->iAnimationIndex = -1;
        lpOption->tLast.x = -1;
        lpOption->tLast.y = -1;
        //debug("End DrawAnimation");
        return 0;
    }
    else{
        BitBlt(hdc,0,0,cxClient,cyClient,hdc2,0,0,SRCCOPY);
    }


    DrawTileAppear(lpOption,lpOption->tLast,deep,hdc);
    DrawTilesMerge(lpOption,lpOption->tMergeForm,lpOption->tMergeTo,lpOption->iAnimationIndex,deep,hdc);

    if( deep != 1 ){
        BitBlt(srchdc,0,0,cxClient,cyClient,hdc,0,0,SRCCOPY);
    }

    DeleteObject(hBmp);
    DeleteDC(hdc);

    if(deep > 0 && lpOption->fAnimation){
        SetTimer(lpOption->hWnd,TIMER_ANIMATION,lpOption->iAnimationSpeed,NULL);
    }

    deep -= 0.1;
    return 1;
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

    BitBlt(srchdc,0,0,cxClient,cyClient,hdc,0,0,SRCCOPY);

    if(lpOption->fAnimation && lpOption->iGameState & GS_RUNNING){
        //deep = 1;
        //DrawAnimation(lpOption,hdc);
        //DrawAnimation(lpOption,srchdc);
        if(!(lpOption->iCurAI != 0 && lpOption->iAISleep < 200)){
            SetTimer(lpOption->hWnd,TIMER_ANIMATION,lpOption->iAnimationSpeed,NULL);
        }
    }
    ReleaseDC(NULL,hdc);
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
        KillTimer(lpOption->hWnd,TIMER_AI);
        GameLoad(lpOption);
        debug("load");
        return 1;

    case MENU_GAME_PAUSE:
        GamePause(lpOption);
        CheckMenuItem(GetMenu(lpOption->hWnd),LOWORD(wParam),lpOption->iGameState&GS_PAUSE?MF_CHECKED:0);
        return 1;

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
    case MENU_AI_6:
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

    case MENU_FINAL_PHASE_1:
    case MENU_FINAL_PHASE_2:
    case MENU_FINAL_PHASE_3:
    case MENU_FINAL_PHASE_4:
    case MENU_FINAL_PHASE_5:
    case MENU_FINAL_PHASE_6:
    case MENU_FINAL_PHASE_7:
    case MENU_FINAL_PHASE_8:
    case MENU_FINAL_PHASE_9:
    case MENU_FINAL_PHASE_10:
    case MENU_FINAL_PHASE_11:
    case MENU_FINAL_PHASE_12:
    case MENU_FINAL_PHASE_13:
    case MENU_FINAL_PHASE_14:
    case MENU_FINAL_PHASE_15:
    case MENU_FINAL_PHASE_16:
    case MENU_FINAL_PHASE_17:
    case MENU_FINAL_PHASE_18:
    case MENU_FINAL_PHASE_19:
    case MENU_FINAL_PHASE_20:
    case MENU_FINAL_PHASE_21:
    case MENU_FINAL_PHASE_22:
    case MENU_FINAL_PHASE_23:
    case MENU_FINAL_PHASE_24:
    case MENU_FINAL_PHASE_25:
    case MENU_FINAL_PHASE_26:
    case MENU_FINAL_PHASE_27:
    case MENU_FINAL_PHASE_28:
    case MENU_FINAL_PHASE_29:
    case MENU_FINAL_PHASE_30:
    case MENU_FINAL_PHASE_31:
    case MENU_FINAL_PHASE_32:
    case MENU_FINAL_PHASE_33:
    case MENU_FINAL_PHASE_34:
    case MENU_FINAL_PHASE_35:
    case MENU_FINAL_PHASE_36:
    case MENU_FINAL_PHASE_37:
    case MENU_FINAL_PHASE_38:
    case MENU_FINAL_PHASE_39:
    case MENU_FINAL_PHASE_40:
    case MENU_FINAL_PHASE_41:
    case MENU_FINAL_PHASE_42:
    case MENU_FINAL_PHASE_43:
    case MENU_FINAL_PHASE_44:
    case MENU_FINAL_PHASE_45:
    case MENU_FINAL_PHASE_46:
    case MENU_FINAL_PHASE_47:
    case MENU_FINAL_PHASE_48:
    case MENU_FINAL_PHASE_49:
        KillTimer(lpOption->hWnd,TIMER_AI);
        return GameLoadPhase(lpOption,LOWORD(wParam) - MENU_FINAL_PHASE_1);

    case MENU_THEME_DEFAULT:
    case MENU_THEME_BLUE:
    case MENU_THEME_PURPLE:
    case MENU_THEME_RED:
    case MENU_THEME_GREEN:
    case MENU_THEME_ORANGE:
    case MENU_THEME_CYANINE:
    case MENU_THEME_YELLOW:
        WinSetTheme(lpOption,LOWORD(wParam));
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

int WinSetTheme(LPOPTION lpOption,int theme){
    int fR,fG,fB;
    int tR,tG,tB;


    switch(theme)
    {
    case MENU_THEME_DEFAULT:
        fR = 0x75;
        fG = 0x64;
        fB = 0x10;

        tR = 0xED;
        tG = 0x12;
        tB = 0x5E;
        lpOption->nTextColor = 0x9A9C9B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_BLUE:
        //4088ff
        fR = 0x40;
        fG = 0x88;
        fB = 0xFF;
        //00df41
        tR = 0x00;
        tG = 0xdf;
        tB = 0x41;
        lpOption->nTextColor = 0x4A4C4B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_PURPLE:
        fR = 0x99;
        fG = 0x7f;
        fB = 0xb5;

        tR = 0xFF;
        tG = 0x39;
        tB = 0x95;
        lpOption->nTextColor = 0x4A4C4B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_RED:        
        //e83229
        fR = 0xE8;
        fG = 0x32;
        fB = 0x29;
        //77007f
        tR = 0x77;
        tG = 0x00;
        tB = 0x7F;
        lpOption->nTextColor = 0x9A9C9B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_GREEN:
        //1cdf00
        //fR = 0x50;
        //fG = 0x99;
        //fB = 0x7f;
        fR = 0x1C;
        fG = 0xDF;
        fB = 0x00;
        //f3ec3e
        tR = 0xF3;
        tG = 0xEC;
        tB = 0x3E;
        lpOption->nTextColor = 0x9A9C9B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_ORANGE:
        //e94619
        fR = 0xff;
        fG = 0xbe;
        fB = 0x20;

        tR = 0xE9;
        tG = 0x46;
        tB = 0x19;
        lpOption->nTextColor = 0x9A9C9B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_CYANINE:
        fR = 0x01;
        fG = 0xBA;
        fB = 0xB1;
        //18ff00
        tR = 0x18;
        tG = 0xFF;
        tB = 0x00;
        lpOption->nTextColor = 0x4A4C4B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    case MENU_THEME_YELLOW:
        //00
        //8a8000
        tR = 0xef;
        tG = 0xff;
        tB = 0x20;

        fR = 0xAA;
        fG = 0x80;
        fB = 0x20;
        lpOption->nTextColor = 0x9A9C9B;
        lpOption->nColorTable[0] = 0X1A2A3A;
        break;
    default:
        debug("error theme");
        break;
    }
    int sR,sG,sB;
    sR = (tR - fR) / 16;
    sG = (tG - fG) / 16;
    sB = (tB - fB) / 16;

    forp(i,17){
        if(i==0)continue;
        lpOption->nColorTable[i] = RGB(fR+sR*i,fG+sG*i,fB+sB*i);
        debug("%08X",RGB(fR+sR*i,fG+sG*i,fB+sB*i));
    }
    
    return 1;
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
                EnableMenuItem(hMenu,id,MF_ENABLED);
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
    case 6:
        GameDirKey(
            lpOption,
            AI6(map,lpOption->nWidth,lpOption->nHeight)
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