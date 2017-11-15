#include "Macro.h"
#include "Game.h"
#include "AI.h"

int AI5(int map[5][5],int w,int h)
{
    int v[] = {1,4,2,3};
    static int i = 0;
    i++;
    i%=4;
    return AIDebugPrintDir("Goto",v[i]);
}