#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Option.h"

// timer list
#define TIMER_AI            0
#define TIMER_ANIMATION     1

int WinKeyHandle(LPOPTION lpOption,int key);
int WinDraw(LPOPTION lpOption,HDC srchdc);
int DrawAnimation(LPOPTION lpOption,HDC hdc);
int WinOnMenu(LPOPTION lpOption,WPARAM wParam);
int WinMenuReset(LPOPTION lpOption);
int WinSysInit(LPOPTION lpOption,HWND hWnd);
int WinOnTimerAI(LPOPTION lpOption);

#endif