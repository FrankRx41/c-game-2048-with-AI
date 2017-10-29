#include <windows.h>
#include <stdio.h>

#define debug(str,...)  printf(str"\n",__VA_ARGS__)

long __stdcall WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    static int  cxClient,cyClient;
    HDC         hdc;
    PAINTSTRUCT ps;

    switch(message){
    case WM_CREATE:
        return 0;
    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd,&ps);
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
    wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpfnWndProc    = WndProc;
    wcex.lpszClassName  = TEXT("WndClass");
    RegisterClassEx(&wcex);

    //AllocConsole();
    freopen("CON","w",stdout);

    int x,y,w = 400,h = 400;
    RECT rt = {0,0,w,h};
    AdjustWindowRectEx(&rt,WS_OVERLAPPEDWINDOW,FALSE,NULL);
    w = rt.right-rt.left;
    h = rt.bottom-rt.top;
    SystemParametersInfo(SPI_GETWORKAREA,NULL,&rt,NULL);
    x = rt.left + (rt.right - rt.left - w) / 2;
    y = rt.top  + (rt.bottom - rt.top - h) / 2;

    HWND hWnd = CreateWindow(TEXT("WndClass"),TEXT("2048 with AI"),WS_OVERLAPPEDWINDOW,
                            x,y,w,h,0,0,hInstance,0);

    ShowWindow(hWnd,nCmdShow);
    MSG msg = {0};
    while(GetMessage(&msg,0,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}