#include <stdio.h>
//#include "..\c-game-2048-with-AI\Game.h"
//#include "..\c-game-2048-with-AI\WeightTable.h"
//#include "..\c-game-2048-with-AI\Game.c"

#include "..\c-game-2048-with-AI\AI1.c"
//#include "..\c-game-2048-with-AI\AI2.c"
#include "..\c-game-2048-with-AI\AI3.c"
#include "..\c-game-2048-with-AI\AI4.c"
#include "..\c-game-2048-with-AI\AI5.c"
#include "..\c-game-2048-with-AI\AI6.c"


int ShowWeight(){
    forp(j,arraylen(WeightTable)){
        forp(i,WEIGHT_LEN){
            printf("%-5g ",WeightTable[j][i]);
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

int PlayGame(int ai){
    OPTION Option = {0};
    LPOPTION lpOption = &Option;
    
    static int time = 1;
    lpOption->iRandseek = time++;

    GameInit(lpOption,4,4);

    while(lpOption->iGameState != GS_OVER){
        int map[5][5];
        memcpy(map,lpOption->mMap,sizeof(map));

        switch(ai){
        case 1:
            GameDirKey(lpOption,AI1(map,4,4));
            break;
        case 2:
            //GameDirKey(lpOption,AI2(map,4,4));
            break;
        case 3:
            GameDirKey(lpOption,AI3(map,4,4));
            break;
        case 4:
            GameDirKey(lpOption,AI4(map,4,4));
            break;
        case 5:
            GameDirKey(lpOption,AI5(map,4,4));
            break;
        case 6:
            GameDirKey(lpOption,AI6(map,4,4));
            break;
        default:
            printf("AI (%d) error!\n",ai);
            getchar();
            return 0;
        }
    }
    return AIFindMaxNum(lpOption->mMap,4,4);
    //GameWatchMap(lpOption);
}

#define MAX_HIGH_SCORE  15
#define TOTAL_RUN       250

int ShowStatist(int *v){
    forp(i,MAX_HIGH_SCORE){
        printf("%-3d %4d (%5.1f%% )\n",i,v[i],(double)v[i]/TOTAL_RUN*100);
    }
}

#ifdef _DEBUG
#error Use release mode to build this project!
#endif

int main(){
    printf("NOTICE: Use Release Mode to build this project to hide debug info\n");
    //printf("Weight Table:\n");
    //if(!ReadWeight()){
    //    return 0;
    //}
    //ShowWeight();
    //system("pause");

    
    for(int ai=1;ai<=6;ai++){
        if(ai==1 || ai==3 || ai==5)continue;
        if(ai != 6)continue;
        printf("Now in AI %d play, total play %d times\n",ai,TOTAL_RUN);
        
        int v[MAX_HIGH_SCORE] = {0};

        printf("Now in statist, please wait (00%%)");
        forp(i,TOTAL_RUN){

            printf("\b\b\b\b%02d%%)",(int)(i*100/TOTAL_RUN));

            v[PlayGame(ai)]++;
        }
        printf("\b\b\b\b\b(100%%)\n");
        ShowStatist(v);
        //system("pause");
    }

    return 0;
}