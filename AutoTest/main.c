#include <stdio.h>
#include <Windows.h>
#include <time.h>

#include "..\c-game-2048-with-AI\Game.h"
#include "..\c-game-2048-with-AI\Macro.h"
#include "..\c-game-2048-with-AI\AI.h"

#include "..\c-game-2048-with-AI\AI1.c"
//#include "..\c-game-2048-with-AI\AI2.c"
#include "..\c-game-2048-with-AI\AI3.c"
#include "..\c-game-2048-with-AI\AI4.c"
#include "..\c-game-2048-with-AI\AI5.c"
#include "..\c-game-2048-with-AI\AI6.c"
//#include "..\c-game-2048-with-AI\AI7.c"

//extern int AI1(int map[5][5],int w,int h);
//extern int AI2(int map[5][5],int w,int h);
//extern int AI3(int map[5][5],int w,int h);
//extern int AI4(int map[5][5],int w,int h);
//extern int AI5(int map[5][5],int w,int h);
//extern int AI6(int map[5][5],int w,int h);
//extern int AI7(int map[5][5],int w,int h);

//int ShowWeight(){
//    forp(j,arraylen(WeightTable)){
//        forp(i,WEIGHT_LEN){
//            printf("%-5g ",WeightTable[j][i]);
//        }
//        printf("\n");
//    }
//}
//
//int ReadWeight(){
//    FILE *fp = fopen("WeightTable.txt","r");
//    if(fp == 0){
//        perror("Open file Error:");
//        return 0;
//    }else{
//        int j = 0;
//        while(!feof(fp)){
//            forp(i,WEIGHT_LEN){
//                fscanf(fp,"%f",&WeightTable[j][i]);
//            }
//            j++;
//        }
//        fclose(fp);
//    }
//    return 1;
//}


int PlayGame(int ai,int seek){
    OPTION Option = {0};
    LPOPTION lpOption = &Option;
    
    lpOption->iRandseek = seek+time(NULL);

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
        case 7:
            //GameDirKey(lpOption,AI7(map,4,4));
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
#define TOTAL_RUN       10000

int ShowStatist(int *v){
    forp(i,MAX_HIGH_SCORE){
        if(i<7)continue;
        printf("%-5d %4d  (%5.1f%% )\n",1<<i,v[i],(double)v[i]/TOTAL_RUN*100);
        //if(i%4==3)printf("\n");
    }
}

#ifdef _DEBUG
#error Use release mode to build this project!
#endif

int main(){
    //printf("NOTICE: Use Release Mode to build this project to hide debug info\n");
    //printf("Weight Table:\n");
    //if(!ReadWeight()){
    //    return 0;
    //}
    //ShowWeight();
    //system("pause");

    
    for(int ai=1;ai<=7;ai++){
        //if(ai==1 || ai==3 || ai==5)continue;
        //if(ai != 6)continue;
        if(ai==1 || ai==2 || ai==3 || ai==4 || ai==5 || ai==-6 || ai==-7)continue;
        unsigned __int64 time = GetTickCount();
        printf("In AI %d statist, total play %d times.\n(00%%)",ai,TOTAL_RUN);
        
        int v[MAX_HIGH_SCORE] = {0};

        forp(i,TOTAL_RUN){
            
            printf("\b\b\b\b\b\b\b\b\b");
            printf("(%.1f%%)",(float)(i*100.0/TOTAL_RUN));

            v[PlayGame(ai,i)]++;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b\b(100.0%%)  ");

        printf("%lld(s) used.\n",(GetTickCount()-time)/1000);

        ShowStatist(v);

        int total = 0;
        forp(i,MAX_HIGH_SCORE){
            if(i>10)total += v[i];
        }
        printf("2048: (%5.1f%% )",(double)total/TOTAL_RUN*100);
        printf("\n\n");
        //system("pause");
    }

    return 0;
}