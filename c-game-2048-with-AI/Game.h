#ifndef _GAME_H_
#define _GAME_H_

#include "Option.h"

#define DIR_UP      1
#define DIR_DOWN    2
#define DIR_LEFT    3
#define DIR_RIGHT   4

int GameSave(LPOPTION lpOption);
int GameLoad(LPOPTION lpOption);
int GamePause(LPOPTION lpOption);
int GameDirKey(int dir,LPOPTION lpOption);
int GameInit(LPOPTION lpOption,int w,int h);
int GameCreatNewBlock(LPOPTION lpOption);

int CheckBlank(int (*)[5]);
int CheckNearby(int (*map)[5],int w,int h,int x,int y,int dir);
int CheckALLDirNearby(int (*map)[5],int w,int h,int dir);

int CheckIfDir(int (*)[5],int w,int h,int dir);

#endif