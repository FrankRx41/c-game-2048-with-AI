#include <windows.h>
#include <stdio.h>
#include "Option.h"
#include "Game.h"
#include "AI.h"
#include "resource.h"
#include "Macro.h"

int WinKeyHandle(int key,LPOPTION lpOption){
    if(key == lpOption->key_esc)
    {
        PostQuitMessage(0);
        return -1;
    }
    if(key == lpOption->key_pause){
        lpOption->iGameState ^= GS_PAUSE;
        CheckMenuItem(GetMenu(lpOption->hWnd),MENU_GAME_PAUSE,lpOption->iGameState&GS_PAUSE?MF_CHECKED:0);
    }
    else if(lpOption->iGameState & GS_ONAI || lpOption->iGameState & GS_PAUSE)
    {
        return 0;
    }
    else if(key == lpOption->key_up)
    {
        return GameDirKey(DIR_UP,lpOption);
    }
    else if(key == lpOption->key_down)
    {
        return GameDirKey(DIR_DOWN,lpOption);
    }        
    else if(key == lpOption->key_left)
    {
        return GameDirKey(DIR_LEFT,lpOption);
    }
    else if(key == lpOption->key_right)
    {
        return GameDirKey(DIR_RIGHT,lpOption);
    }
    debug("inefficacy key %d %X",key);
    return 0;
}

/* Copy: http://www.kumei.ne.jp/c_lang/sdk/sdk_27.htm */
HFONT SetMyFont(HDC hdc,LPCTSTR face,int h,int angle)
{
    //debug("FontName: [%s]",face);
    static HFONT oldFont = NULL;
    DeleteObject(oldFont);
    HFONT hFont = CreateFont(h,0,angle,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_OUTLINE_PRECIS,
               CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,VARIABLE_PITCH,face);
    oldFont = SelectObject(hdc,hFont);
    return hFont;
}

int GetTableColor(int x,LPOPTION lpOption){
    int i = 0;
    while(x >>= 1,x != 0){
        i++;
    }
    return lpOption->nColorTable[i];
}

int WinDraw(HDC srchdc,LPOPTION lpOption){
    char s[MAX] = {0};
    int correct = lpOption->nCorrect;
    int padding = lpOption->nPadding;
    HBRUSH  hBrush;
    int (*map)[5] = lpOption->nMap;
    int w = lpOption->nWidth,h = lpOption->nHeight;
    int cxClient = lpOption->cxClient, cyClient = lpOption->cyClient;

    HDC hdc = CreateCompatibleDC(srchdc);
    HBITMAP hBmp;
    if(lpOption->fGaryMode){
        hBmp = CreateCompatibleBitmap(hdc,cxClient,cyClient);
    }else{
        hBmp = CreateCompatibleBitmap(srchdc,cxClient,cyClient);
    }
    SelectObject(hdc,hBmp);

    hBrush = CreateSolidBrush(lpOption->nBgColor);
    RECT rt = {0,0,cxClient,cyClient};
    FillRect(hdc,&rt,hBrush);
    DeleteObject(hBrush);

    SetBkMode(hdc,1);

    for(int x=0;x<h;x++){
        for(int y=0;y<w;y++){

            rt.left     = y*padding+correct;
            rt.right    = y*padding+padding-correct;
            rt.top      = x*padding+correct;
            rt.bottom   = x*padding+padding-correct;

            SetTextColor(hdc,lpOption->nTextColor);
            SetMyFont(hdc, (LPCTSTR)lpOption->hFontName,lpOption->iFontSize,0);

            //debug("color: %x",GetTableColor(map[x][y]));
            hBrush = CreateSolidBrush(GetTableColor(map[x][y],lpOption));
            //FillRect(hdc,&rt,hBrush);
            SelectObject(hdc,hBrush);
            SelectObject(hdc,GetStockObject(NULL_PEN));
            RoundRect(hdc,rt.left,rt.top,rt.right,rt.bottom,20,20);

            if(map[x][y] != 0){
                sprintf(s,"%d",map[x][y]);
                DrawText(hdc,s,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            }
            DeleteObject(hBrush);
        }
    }

    SetTextColor(hdc,RGB(0xFF,0xFF,0xFF));
    sprintf(s," score: %d",lpOption->nCurScore);
    TextOut(hdc,0,h*padding,s,strlen(s));
    sprintf(s," Step: %d",lpOption->nStep);
    TextOut(hdc,lpOption->cxClient/2,h*padding,s,strlen(s));

    if(lpOption->iGameState == GS_OVER){
        rt.left     = 0;
        rt.right    = cxClient;
        rt.top      = 0;
        rt.bottom   = cxClient;
        SetMyFont(hdc,(LPCTSTR)lpOption->hFontName,lpOption->iFontSize*2,0);
        sprintf(s,"%s","Game Over");
        DrawText(hdc,s,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    BitBlt(srchdc,0,0,cxClient,cyClient,hdc,0,0,SRCCOPY);

    DeleteObject(hBmp);
    DeleteDC(hdc);
}

int WinOnMenu(HWND hWnd,WPARAM wParam,LPOPTION lpOption){
    switch(LOWORD(wParam)){
    case MENU_NEW_3:
        debug("game start 3 * 4");
        GameInit(lpOption,4,3);
        return 0;
    case MENU_NEW_4:
        debug("game start 4 * 4");
        GameInit(lpOption,4,4);
        return 0;
    case MENU_NEW_5:
        debug("game start 5 * 5");
        GameInit(lpOption,4,5);
        return 0;
    case MENU_NEW_CUSTOMIZE:
        debug("game start customize");
        return 0;

    case MENU_GAME_SAVE:
        GameSave(lpOption);
        debug("save");
        //return 0;
        // fall throught
    case MENU_GAME_LOAD:
        GameLoad(lpOption);
        debug("load");
        EnableMenuItem(GetMenu(hWnd),MENU_GAME_LOAD,MF_ENABLED);
        InvalidateRect(hWnd,NULL,FALSE);
        return 0;
    case MENU_GAME_PAUSE:
        lpOption->iGameState ^= GS_PAUSE;
        CheckMenuItem(GetMenu(hWnd),MENU_GAME_PAUSE,lpOption->iGameState&GS_PAUSE?MF_CHECKED:0);
        break;

    case MENU_GAME_EXIT:
        PostQuitMessage(0);
        return -1;

    case MENU_OPTION_SOUND:
        lpOption->fSound = !lpOption->fSound;
        CheckMenuItem(GetMenu(hWnd),LOWORD(wParam),lpOption->fSound?MF_CHECKED:0);
        return 0;
    case MENU_OPTION_FULLSCREEN:
        lpOption->fFullScreen = !lpOption->fFullScreen;
        CheckMenuItem(GetMenu(hWnd),LOWORD(wParam),lpOption->fFullScreen?MF_CHECKED:0);
        return 0;
    case MENU_OPTION_SUGGESTION:
        lpOption->fSuggestion = !lpOption->fSuggestion;
        CheckMenuItem(GetMenu(hWnd),LOWORD(wParam),lpOption->fSuggestion?MF_CHECKED:0);
        return 0;
    case MENU_OPTION_GARYMODE:
        lpOption->fGaryMode = !lpOption->fGaryMode;
        CheckMenuItem(GetMenu(hWnd),LOWORD(wParam),lpOption->fGaryMode?MF_CHECKED:0);
        InvalidateRect(hWnd,NULL,FALSE);
        return 0;
    case MENU_OPTION_ANIMATION:
        lpOption->fAnimation = !lpOption->fAnimation;
        CheckMenuItem(GetMenu(hWnd),LOWORD(wParam),lpOption->fAnimation?MF_CHECKED:0);
        return 0;

    case MENU_AI_0:
    case MENU_AI_1:
    case MENU_AI_2:
        if(lpOption->AI[LOWORD(wParam) - MENU_AI_0] != 1){
            forloop(i,3)
            lpOption->AI[i] = 0;
            lpOption->AI[LOWORD(wParam) - MENU_AI_0] = 1;
            lpOption->iGameState |= GS_ONAI;
            //SetTimer(hWnd,LOWORD(wParam) - MENU_AI_0,lpOption->iAISleep,0);
            SetTimer(hWnd,0,lpOption->iAISleep,0);
            debug("start AI: %d",LOWORD(wParam) - MENU_AI_0);
        }else{
            KillTimer(hWnd,0);
            lpOption->AI[LOWORD(wParam) - MENU_AI_0] = 0;
            lpOption->iGameState -= GS_ONAI;
            debug("stop AI: %d",LOWORD(wParam) - MENU_AI_0);
        }
        //debug("AI: %d",LOWORD(wParam) - MENU_AI_0);
        CheckMenuItem(GetMenu(hWnd),MENU_AI_0,lpOption->AI[0]?MF_CHECKED:0);
        CheckMenuItem(GetMenu(hWnd),MENU_AI_1,lpOption->AI[1]?MF_CHECKED:0);
        CheckMenuItem(GetMenu(hWnd),MENU_AI_2,lpOption->AI[2]?MF_CHECKED:0);
        break;

    case MENU_HELP_ABOUT:
        return MessageBox(0,"EiSnow\n\n(C)CopyRight  2017.10.31","2048",0);
    }
}

int WinMenuInit(LPOPTION lpOption){
    HWND hWnd = lpOption->hWnd;
    HMENU hMenu = GetMenu(hWnd);
    int  cMenuItems;
    FILE  * fp;

    hMenu = GetSubMenu(hMenu,0);
    cMenuItems = GetMenuItemCount(hMenu);
    debug("cMenuItems: %d",cMenuItems);

    for(int nPos = 0; nPos < cMenuItems; nPos++)
    {
        UINT id = GetMenuItemID(hMenu,nPos);

        switch(id)
        {
        case MENU_OPTION_SOUND:
            CheckMenuItem(hMenu,id,lpOption->fSound?MF_CHECKED:0);
            debug("1");
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
            //CheckMenuItem(hMenu,id,lpOption->fAnimation?MF_CHECKED:0);
            EnableMenuItem(hMenu,id,MF_GRAYED);
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

    //hMenu = GetSubMenu(hMenu,4);
    //cMenuItems = GetMenuItemCount(hMenu);
    //for(int nPos = 0; nPos < cMenuItems; nPos++)
    //{
    //    UINT id = GetMenuItemID(hMenu,nPos);
    //
    //    switch(id){
    //    case MENU_AI_1:
    //    case MENU_AI_2:
    //    case MENU_AI_3:
    //        EnableMenuItem(hMenu,id,MF_GRAYED);
    //        break;
    //    }
    //}
}

int WinSysInit(HWND hWnd,LPOPTION lpOption){
    lpOption->hWnd = hWnd;
    lpOption->iGameState = GS_RUNNING;
    ReadSetting(lpOption);
    WinMenuInit(lpOption);
}

long __stdcall WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    HDC         hdc;
    PAINTSTRUCT ps;

    static OPTION   Option;
    static LPOPTION lpOption = &Option;
    int map[5][5];

    switch(message){
    case WM_CREATE:
        WinSysInit(hWnd,lpOption);
        GameInit(lpOption,4,4);
        //GameInit(lpOption);
        return 0;
    case WM_COMMAND:
        WinOnMenu(hWnd,wParam,lpOption);
        break;
    case WM_TIMER:
        // AI timer:
        if(lpOption->iGameState & GS_PAUSE)return 0;
        memcpy(map,lpOption->nMap,sizeof(map));
        debug("On AI %d %d %d command",lpOption->AI[0],lpOption->AI[1],lpOption->AI[2]);
        if(lpOption->AI[0]){
            GameDirKey(AI0(map,lpOption->nWidth,lpOption->nHeight),lpOption);
        }
        else if(lpOption->AI[1]){
            GameDirKey(AI1(map,lpOption->nWidth,lpOption->nHeight),lpOption);
        }
        else if(lpOption->AI[2]){
            GameDirKey(AI2(map,lpOption->nWidth,lpOption->nHeight),lpOption);
        }
        InvalidateRect(hWnd,NULL,FALSE);

        //CreatNewBlock(lpOption);
        //KillTimer(hWnd,0);
        //InvalidateRect(hWnd,NULL,TRUE);
        return 0;
    case WM_KEYDOWN:
        switch(wParam){
        case VK_F3:
            PrintOption(lpOption);
            return 0;
        default:
            if(WinKeyHandle(wParam,lpOption)){
                InvalidateRect(hWnd,NULL,FALSE);
                //SetTimer(hWnd,0,300,0);
            }
            if(lpOption->iGameState == GS_OVER){
                WinMenuInit(lpOption);
            }
            return 0;
        }
        break;
    case WM_SIZE:
        lpOption->cxClient = LOWORD(lParam);
        lpOption->cyClient = HIWORD(lParam);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd,&ps);
        WinDraw(hdc,lpOption);
        EndPaint(hWnd,&ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}

int __stdcall WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine,int nCmdShow){
    WNDCLASSEX wcex = {0};
    wcex.cbSize         = sizeof(wcex);
    wcex.hCursor        = LoadCursor(0,IDC_ARROW);
    wcex.hInstance      = hInstance;
    wcex.hbrBackground  = NULL;
    wcex.lpfnWndProc    = WinProc;
    wcex.lpszClassName  = TEXT("2048GAMEWITHAI");
    wcex.lpszMenuName   = TEXT("2048GAMEWITHAI");
    wcex.style          = CS_OWNDC;
    RegisterClassEx(&wcex);

#ifdef _DEBUG
    AllocConsole();
    freopen("CON","w",stdout);
#endif

    int x,y,w = 400,h = 400;
    RECT rt = {0,0,w,h};
    AdjustWindowRectEx(&rt,WS_OVERLAPPEDWINDOW,TRUE,NULL);
    w = rt.right-rt.left;
    h = rt.bottom-rt.top;
    SystemParametersInfo(SPI_GETWORKAREA,NULL,&rt,NULL);
    x = rt.left + (rt.right - rt.left - w) / 2;
    y = rt.top  + (rt.bottom - rt.top - h) / 2;

    HWND hWnd = CreateWindow(TEXT("2048GAMEWITHAI"),TEXT("2048 with AI"),WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX,
                            x,y,w,h,0,0,hInstance,0);
    ShowWindow(hWnd,nCmdShow);
    MSG msg = {0};
    HANDLE hAccel = LoadAccelerators(hInstance,TEXT("2048GAMEWITHAI"));
    while(GetMessage(&msg,0,0,0)){
        if(!TranslateAccelerator(hWnd,hAccel,&msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    UnregisterClass("WndClass",hInstance);

    return msg.wParam;
}