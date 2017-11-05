#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Option.h"

// timer list
#define TIMER_AI  0

int WinKeyHandle(LPOPTION lpOption,int key);
int WinDraw(LPOPTION lpOption,HDC srchdc);
int WinOnMenu(LPOPTION lpOption,WPARAM wParam);
int WinMenuReset(LPOPTION lpOption);
int WinSysInit(LPOPTION lpOption,HWND hWnd);
int WinOnTimerAI(LPOPTION lpOption);

#endif