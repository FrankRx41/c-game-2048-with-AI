#ifndef _GAME_H_
#define _GAME_H_

#include "Option.h"

#define DIR_UP      1
#define DIR_DOWN    2
#define DIR_LEFT    3
#define DIR_RIGHT   4

int GameInit(LPOPTION lpOption,int w,int h);
int GameDirKey(LPOPTION lpOption,int dir);

int GameSave(LPOPTION lpOption);
int GameLoad(LPOPTION lpOption);
int GamePause(LPOPTION lpOption);
int GameOver(LPOPTION lpOption);

// debug only
int GameWatchMap(LPOPTION lpOption);

/*
Copy Function "GameDirKey" for your AI, DO NOT direct use it!
*/

#endif