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
//int GameCreatNewBlock(LPOPTION lpOption);

/*
Clac the numb of maps blank tile
Notice:
DO NOT USE for your AI, write a new one for yourself!
*******************************************************************************/
//int CheckBlank(const int (*map)[5]);

/*
Check have two same tiles near or not
return num of those tiles, use of check gameover
Notice:
DO NOT USE for your AI!
*******************************************************************************/
//int CheckALLDirNearby(const int (*map)[5],int w,int h,int dir);

/*
Check can move to this direction or not
return 0 or 1 and a new map
Notic:
for AI only!
*******************************************************************************/
int CheckIfDir(const int (*map)[5],int w,int h,int dir);

#endif