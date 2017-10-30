#include <windows.h>
#include <stdio.h>

#define debug(str,...)  printf(str"\n",__VA_ARGS__)

#define W   4
#define H   4

#define padding__ 100

int map[W][H];

#define forloop(i,time) for(int i=0;i<(time);i++)

int CreatNewBlock(){
    int x = rand()%W;
    int y = rand()%H;
    while(map[x][y]!=0){
        x = rand()%W;
        y = rand()%H;
    }
    int $ = (rand()%2+1)<<1;
    if(map[x][y] == 0){
        map[x][y] = $;
    }
    debug("create new %d in (%d,%d)",$,x,y);
}



int GameKeyHandle(int key){
    int haveblank = 0;
    int hadmove = 0;

    switch(key){
    case VK_UP:
        forloop(y,W){
            for(int x=0;x<H-1;x++){
                int *F = &map[x][y];
                int *S = NULL;
                
                for(int i=x+1;i<H;i++){
                    S = &map[i][y];
                    debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    if(*S != 0){
                        if(*F == 0){
                            *F = *S;
                            *S = 0;
                            hadmove = 1;
                        }
                        else if(*F == *S){
                            *F = *F<<1;
                            *S = 0;
                            hadmove = 1;
                            break;
                        }
                        else break;
                    }
                }

                debug("up (%d,%d)",x,y);
            }
        }
        break;
    case VK_DOWN:
        forloop(y,W){
            for(int x=H-1;x>0;x--){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=x-1;i>=0;i--){
                    S = &map[i][y];
                    debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    if(*S != 0){
                        if(*F == 0){
                            *F = *S;
                            *S = 0;
                            hadmove = 1;
                        }
                        else if(*F == *S){
                            *F = *F<<1;
                            *S = 0;
                            hadmove = 1;
                            break;
                        }
                        else break;
                    }
                }

                debug("down (%d,%d)",x,y);
            }
        }
        break;
    case VK_LEFT:
        forloop(x,H){
            for(int y=0;y<W-1;y++){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y+1;i<W;i++){
                    S = &map[x][i];
                    debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    if(*S != 0){
                        if(*F == 0){
                            *F = *S;
                            *S = 0;
                            hadmove = 1;
                        }
                        else if(*F == *S){
                            *F = *F<<1;
                            *S = 0;
                            hadmove = 1;
                            break;
                        }
                        else break;
                    }
                }

                debug("down (%d,%d)",x,y);
            }
        }
        break;
    case VK_RIGHT:
        forloop(x,H){
            for(int y=W-1;y>0;y--){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y-1;i>=0;i--){
                    S = &map[x][i];
                    debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
                    if(*S != 0){
                        if(*F == 0){
                            *F = *S;
                            *S = 0;
                            hadmove = 1;
                        }
                        else if(*F == *S){
                            *F = *F<<1;
                            *S = 0;
                            hadmove = 1;
                            break;
                        }
                        else break;
                    }
                }

                debug("down (%d,%d)",x,y);
            }
        }
        break;
    }

    // every time you move, you will get one new 2 or 4
    
    //if(haveblank){
    //    CreatNewBlock();
    //}
    //else for(int x=0;x<W;x++){
    //        for(int y=0;y<H;y++){
    //            if(map[x][y] == 0){
    //                haveblank = 1;
    //                CreatNewBlock();
    //                break;
    //            }
    //        }
    //}

    for(int y=0;y<W;y++){
        debug("%4d %4d %4d %4d",map[y][0],map[y][1],map[y][2],map[y][3]);
    }
    return hadmove;
}

/* Copy: http://www.kumei.ne.jp/c_lang/sdk/sdk_27.htm */
HFONT SetMyFont(HDC hdc,LPCTSTR face,int h,int angle)
{
    static HFONT oldFont = NULL;
    if(oldFont){
        DeleteObject(oldFont);
    }
    HFONT hFont;
    hFont = CreateFont(h,    //フォント高さ
                       0,                    //文字幅
                       angle,                    //テキストの角度
                       0,                    //ベースラインとｘ軸との角度
                       FW_REGULAR,            //フォントの重さ（太さ）
                       FALSE,                //イタリック体
                       FALSE,                //アンダーライン
                       FALSE,                //打ち消し線
                       SHIFTJIS_CHARSET,    //文字セット
                       OUT_DEFAULT_PRECIS,    //出力精度
                       CLIP_DEFAULT_PRECIS,//クリッピング精度
                       PROOF_QUALITY,        //出力品質
                       FIXED_PITCH | FF_MODERN,//ピッチとファミリー
                       face);    //書体名
    oldFont = SelectObject(hdc,hFont);
    return hFont;
}

int ColorTable[] = {0xFFFFFF,0xF5F5DC,0x8A2BE2,0xD2691E,0x00FFFF,0x006400,0x9932CC};

int GetTableColor(int x){
	int i=0;
	while(x>>=1,x!=0){
		i++;
	}
	return ColorTable[i];
}

int GameDraw(HDC hdc){
    char s[3] = {0};
    RECT rt = {0};
    int correct = 10;
    HBRUSH hBrush;

    SetBkColor(hdc,RGB(0,100,231));
    SetBkMode(hdc,1);

    for(int x=0;x<W;x++){
        for(int y=0;y<H;y++){
            int padding = 100;
            rt.left     = y*padding+correct;
            rt.right    = y*padding+padding__-correct;
            rt.top      = x*padding+correct;
            rt.bottom   = x*padding+padding__-correct;

            SetMyFont(hdc, (LPCTSTR)"Sans-serif", 30, 0);

            sprintf(s,"%d",map[x][y]);
            //debug("color: %x",GetTableColor(map[x][y]));
            hBrush = CreateSolidBrush(GetTableColor(map[x][y]));
            FillRect(hdc,&rt,hBrush);
            DrawText(hdc,s,-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            //TextOut(hdc,x*100,y*100,s,strlen(s));
            DeleteObject(hBrush);
        }
    }
}

long __stdcall WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    static int  cxClient,cyClient;
    HDC         hdc;
    PAINTSTRUCT ps;

    switch(message){
    case WM_CREATE:
        //map[rand()%W][rand()%H] = (rand()%2+1)<<1;
        map[0][0] = 4;
        map[1][0] = 2;
        map[2][0] = 4;
        map[3][0] = 2;
        return 0;
    case WM_TIMER:
        CreatNewBlock();
        KillTimer(hWnd,0);
        InvalidateRect(hWnd,NULL,TRUE);
        return 0;
    case WM_KEYDOWN:
        switch(wParam){
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            if(GameKeyHandle(wParam)){
                InvalidateRect(hWnd,NULL,TRUE);
                SetTimer(hWnd,0,300,0);
            }
            return 0;
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        break;
    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        debug("width: %d height: %d",cxClient,cyClient);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd,&ps);
        GameDraw(hdc);
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

#ifdef _DEBUG
    AllocConsole();
    freopen("CON","w",stdout);
#endif

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
