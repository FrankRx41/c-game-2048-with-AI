#include "Window.h"
#include "Game.h"
#include "Macro.h"
#include <stdio.h>

long __stdcall WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rt;

    static OPTION   Option = {0};
    static LPOPTION lpOption = &Option;

    switch(message){
    case WM_CREATE:
        WinSysInit(lpOption,hWnd);
        GameInit(lpOption,4,4);
        return 0;
    case WM_COMMAND:
        if(WinOnMenu(lpOption,wParam)){
            InvalidateRect(hWnd,NULL,FALSE);
        }
        return 0;
    case WM_TIMER:
        switch(wParam){
        case TIMER_AI:
            if(WinOnTimerAI(lpOption)){
                InvalidateRect(hWnd,NULL,FALSE);
            }
            return 0;
        case TIMER_ANIMATION:
            hdc = GetDC(hWnd);
            DrawAnimation(lpOption,hdc);
            ReleaseDC(hWnd,hdc);
            return 0;
        }
        break;
    case WM_KEYDOWN:
        switch(wParam){
        // debug key
        case VK_F9:
            PrintOption(lpOption);
            return 0;
        case VK_F8:
            GameWatchMap(lpOption);
            return 0;
        // game or system key
        default:
            if(WinKeyHandle(lpOption,wParam)){
                InvalidateRect(hWnd,NULL,FALSE);
                // you can not save when you die
                WinMenuReset(lpOption);
            }
            return 0;
        }
        break;
    case WM_MOVE:
        GetWindowRect(hWnd,&rt);
        // i save window pos to config file
        lpOption->nWinPosX = rt.left;
        lpOption->nWinPosY = rt.top;
        return 0;
    case WM_SIZE:
        lpOption->cxClient = LOWORD(lParam);
        lpOption->cyClient = HIWORD(lParam);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd,&ps);
        WinDraw(lpOption,hdc);
        EndPaint(hWnd,&ps);
        return 0;
    case WM_DESTROY:
        SaveOption(lpOption);        
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
    wcex.lpszClassName  = CLASSNAME;
    wcex.lpszMenuName   = CLASSNAME;
    wcex.style          = CS_OWNDC;
    wcex.hIcon          = LoadIcon(hInstance,CLASSNAME);
    RegisterClassEx(&wcex);

#ifdef _DEBUG
    AllocConsole();
    freopen("CON","w",stdout);
#endif

    // center window
    int x,y,w = 400,h = 400;
    RECT rt = {0,0,w,h};
    AdjustWindowRectEx(&rt,WS_OVERLAPPEDWINDOW,TRUE,NULL);
    w = rt.right-rt.left;
    h = rt.bottom-rt.top;
    SystemParametersInfo(SPI_GETWORKAREA,NULL,&rt,NULL);
    x = rt.left + (rt.right - rt.left - w) / 2;
    y = rt.top  + (rt.bottom - rt.top - h) / 2;

    HWND hWnd = CreateWindow(CLASSNAME,TITLENAME,WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX,
                            x,y,0,0,0,0,hInstance,0);
    ShowWindow(hWnd,nCmdShow);
    MSG msg = {0};
    HANDLE hAccel = LoadAccelerators(hInstance,CLASSNAME);

    while(GetMessage(&msg,0,0,0))
    {
        if(!TranslateAccelerator(hWnd,hAccel,&msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    UnregisterClass(CLASSNAME,hInstance);

    return msg.wParam;
}