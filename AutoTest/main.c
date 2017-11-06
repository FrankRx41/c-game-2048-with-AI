#include <stdio.h>
//#include "..\c-game-2048-with-AI\Game.h"
//#include "..\c-game-2048-with-AI\WeightTable.h"
//#include "..\c-game-2048-with-AI\Game.c"

#include "..\c-game-2048-with-AI\AI1.c"


int ShowWeight(){
    forp(j,arraylen(WeightTable)){
        forp(i,WEIGHT_LEN){
            printf("%-3g ",WeightTable[j][i]);
        }
        printf("\n");
    }
}

int ReadWeight(){
    FILE *fp = fopen("WeightTable.txt","r");
    if(fp == 0){
        perror("Open file Error:");
        return 0;
    }else{
        int j = 0;
        while(!feof(fp)){
            forp(i,WEIGHT_LEN){
                fscanf(fp,"%f",&WeightTable[j][i]);
            }
            j++;
        }
        fclose(fp);
    }
    return 1;
}

int PlayGame(){
    OPTION Option = {0};
    LPOPTION lpOption = &Option;

    lpOption->iRandseek = time(NULL)+rand();
    GameInit(lpOption,4,4);

    while(lpOption->iGameState != GS_OVER){
        int map[5][5];
        memcpy(map,lpOption->mMap,sizeof(map));
        GameDirKey(lpOption,AI1(map,4,4));
    }
    printf("%d\n",AIFindMaxNum(lpOption->mMap,4,4));
    //GameWatchMap(lpOption);
}

int main(){
    if(!ReadWeight()){
        return 0;
    }
    ShowWeight();
    getchar();
    forp(i,1000){
        //printf("%d:\n",i);
        PlayGame();
    }

    return 0;
}