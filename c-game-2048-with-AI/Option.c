#include "Option.h"
#include "Macro.h"
#include <stdio.h>

FILE * fp;

void PrintOption(LPOPTION lpOption){
    printf("----------------------------------------------------------------\n");
    printf("GameState:%04X\n",lpOption->iGameState);
    printf("Sound:%d   FullScreen:%d   Suggestion:%d   GaryMode:%d\n",lpOption->fSound,lpOption->fFullScreen,lpOption->fSuggestion,lpOption->fGaryMode);
    printf("Width:%d   Height:%d\n",lpOption->nWidth,lpOption->nHeight);
    printf("Score:%d   %d   %d\n",lpOption->nScore[0],lpOption->nScore[1],lpOption->nScore[2]);
    printf("Level:%d \n",lpOption->iLevel);
    //printf("ColorTable:\n");
    //for(int i=0;i<sizeof(lpOption->nColorTable)/sizeof(lpOption->nColorTable[0]);i++){
    //    printf("[%d] 0x%06X\t",1<<i,lpOption->nColorTable[i]);
    //}
    printf("BgColor:0x%X\n",lpOption->nBgColor);
    //printf("\n");
    printf("FontName:%s   FontSize:%d\n",lpOption->hFontName,lpOption->iFontSize);
    printf("----------------------------------------------------------------\n");
}

int WriteOptionHex(int *pointer,int value){
    fprintf(fp,"%#X\n",value);
    *pointer = value;
}
int ReadOptionHex(int *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    sscanf(str,"%X",pointer);
    debug("%s:[%x]",__FUNCTION__,*pointer);
}

int WriteOptionDec(int *pointer,int value){
    fprintf(fp,"%d\n",value);
    *pointer = value;
}
int ReadOptionDec(int *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    sscanf(str,"%d",pointer);
}

int WriteOptionStr(char *pointer,char *value){
    fprintf(fp,"%s\n",value);
    strcpy(pointer,value);
}
int SkipSpace(char * str){
    int i=0;
    while(str[i]==' ' || str[i]=='\t'){
        i++;
    }
    return i;
}
int ReadOptionStr(char *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    int i = SkipSpace(str);
    strncpy(pointer,str+i,strlen(str)-1-i);
    //debug("%s:[%s]",__FUNCTION__,pointer);
}

int FindHeader(char *m1){
    char str[MAX];
    while(fgets(str,MAX,fp))
    {
        if(str[0]=='\n' || str[0]=='#')continue;

        char str1[MAX];
        strcpy(str1,str);

        char *str2 = strtok(str1,"=");
        if(strncmp(m1,str2,strlen(m1)) == 0){
            int back = strlen(str2)-strlen(str);
            fseek(fp,back,SEEK_CUR);
            return 1;
        }
    }
    fseek(fp,0,SEEK_END);
    //debug("no find m2: %s!",m2);
    fprintf(fp,"%s\t= ",m1);
    return 0;
}

int OptionHex(char *header,int *pointer,int value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        ReadOptionHex(pointer);
    }
    else{
        WriteOptionHex(pointer,value);
    }
    fclose(fp);
}

int OptionDec(char *header,int *pointer,int value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        ReadOptionDec(pointer);
    }
    else{
        WriteOptionDec(pointer,value);
    }
    fclose(fp);
}

int OptionStr(char *header,char *pointer,char *value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        ReadOptionStr(pointer);
    }
    else{
        WriteOptionStr(pointer,value);
    }
    fseek(fp,0,SEEK_SET);
    fclose(fp);
}

int ReadOption(LPOPTION lpOption){
    char filename[] = "config.ini";
    fp = fopen(filename,"r+");
    if(!fp){
        debug("new file");
        fp = fopen(filename,"w+");
        if(!fp){
            MessageBox(0,"Create Config File Error!",0,0);
            PostQuitMessage(0);
        }
    }
    fprintf(fp,"# this file create by 2048\n");
    fclose(fp);

    OptionStr("FomtName",   &lpOption->hFontName,       "Impact");
    OptionStr("SaveDate",   &lpOption->sSaveDate,       "2048.dat");
    OptionDec("FontSize",   &lpOption->iFontSize,       34);

    OptionHex("0",          &lpOption->nColorTable[0],  0x1A2A3A);
    OptionHex("2",          &lpOption->nColorTable[1],  0x106475);
    OptionHex("4",          &lpOption->nColorTable[2],  0x106C7D);
    OptionHex("8",          &lpOption->nColorTable[3],  0x106485);
    OptionHex("16",         &lpOption->nColorTable[4],  0x10598D);
    OptionHex("32",         &lpOption->nColorTable[5],  0x205B95);
    OptionHex("64",         &lpOption->nColorTable[6],  0x204D9D);
    OptionHex("128",        &lpOption->nColorTable[7],  0x2040A5);
    OptionHex("256",        &lpOption->nColorTable[8],  0x2052AD);
    OptionHex("512",        &lpOption->nColorTable[9],  0x3034B5);
    OptionHex("1024",       &lpOption->nColorTable[10], 0x3036BD);
    OptionHex("2048",       &lpOption->nColorTable[11], 0x3035C5);
    OptionHex("4096",       &lpOption->nColorTable[12], 0x402ACD);
    OptionHex("8192",       &lpOption->nColorTable[13], 0x402CD5);
    OptionHex("16384",      &lpOption->nColorTable[14], 0x4B1EDD);
    OptionHex("32768",      &lpOption->nColorTable[15], 0x4D11E5);
    OptionHex("65536",      &lpOption->nColorTable[16], 0x5E12ED);
    OptionHex("131072",     &lpOption->nColorTable[17], 0x5E12ED);
    OptionHex("262144",     &lpOption->nColorTable[18], 0x5E12ED);
    OptionHex("524288",     &lpOption->nColorTable[19], 0x5E12ED);
    OptionHex("10485796",   &lpOption->nColorTable[20], 0x5E12ED);

    OptionHex("bgColor",    &lpOption->nBgColor,        0x111240);
    OptionHex("textColor",  &lpOption->nTextColor,      0x9A9C9B);

    OptionDec("padding",    &lpOption->nPadding,        100);
    OptionDec("correct",    &lpOption->nCorrect,        5);

    OptionDec("key_up",     &lpOption->key_up,          VK_UP);
    OptionDec("key_down",   &lpOption->key_down,        VK_DOWN);
    OptionDec("key_left",   &lpOption->key_left,        VK_LEFT);
    OptionDec("key_right",  &lpOption->key_right,       VK_RIGHT);
    OptionDec("key_esc",    &lpOption->key_esc,         VK_ESCAPE);
    OptionDec("key_pause",  &lpOption->key_pause,       VK_SPACE);
    
    OptionDec("score3",     &lpOption->nScore[0],       0);
    OptionDec("score4",     &lpOption->nScore[1],       0);
    OptionDec("score5",     &lpOption->nScore[2],       0);

    OptionDec("sound",      &lpOption->fSound,          1);
    OptionDec("FullScreen", &lpOption->fFullScreen,     0);
    OptionDec("Suggestion", &lpOption->fSuggestion,     0);
    OptionDec("GaryMode",   &lpOption->fGaryMode,       0);
    OptionDec("Animation",  &lpOption->fAnimation,      1);

    OptionDec("AISleep",    &lpOption->iAISleep,        300);
    
    return 0;
}

int SaveOptionHex(char *header,int *pointer){
    OptionHex(header,pointer,*pointer);
}
int SaveOptionDec(char *header,int *pointer){
    OptionDec(header,pointer,*pointer);
}
int SaveOptionStr(char *header,int *pointer){
    OptionStr(header,pointer,pointer);
}

int SaveOption(LPOPTION lpOption){
    char filename[] = "config.ini";
    fp = fopen(filename,"w");
    if(!fp){
        MessageBox(0,"Open Config File Error!",0,0);
    }
    fprintf(fp,"# this file create by 2048\n");
    fclose(fp);

    SaveOptionStr("FomtName",   &lpOption->hFontName);
    SaveOptionStr("SaveDate",   &lpOption->sSaveDate);
    SaveOptionDec("FontSize",   &lpOption->iFontSize);

    SaveOptionHex("0",          &lpOption->nColorTable[0]);
    SaveOptionHex("2",          &lpOption->nColorTable[1]);
    SaveOptionHex("4",          &lpOption->nColorTable[2]);
    SaveOptionHex("8",          &lpOption->nColorTable[3]);
    SaveOptionHex("16",         &lpOption->nColorTable[4]);
    SaveOptionHex("32",         &lpOption->nColorTable[5]);
    SaveOptionHex("64",         &lpOption->nColorTable[6]);
    SaveOptionHex("128",        &lpOption->nColorTable[7]);
    SaveOptionHex("256",        &lpOption->nColorTable[8]);
    SaveOptionHex("512",        &lpOption->nColorTable[9]);
    SaveOptionHex("1024",       &lpOption->nColorTable[10]);
    SaveOptionHex("2048",       &lpOption->nColorTable[11]);
    SaveOptionHex("4096",       &lpOption->nColorTable[12]);
    SaveOptionHex("8192",       &lpOption->nColorTable[13]);
    SaveOptionHex("16384",      &lpOption->nColorTable[14]);
    SaveOptionHex("32768",      &lpOption->nColorTable[15]);
    SaveOptionHex("65536",      &lpOption->nColorTable[16]);
    SaveOptionHex("131072",     &lpOption->nColorTable[17]);
    SaveOptionHex("262144",     &lpOption->nColorTable[18]);
    SaveOptionHex("524288",     &lpOption->nColorTable[19]);
    SaveOptionHex("10485796",   &lpOption->nColorTable[20]);

    SaveOptionHex("bgColor",    &lpOption->nBgColor);
    SaveOptionHex("textColor",  &lpOption->nTextColor);
 
    SaveOptionDec("padding",    &lpOption->nPadding);
    SaveOptionDec("correct",    &lpOption->nCorrect);
  
    SaveOptionDec("key_up",     &lpOption->key_up);
    SaveOptionDec("key_down",   &lpOption->key_down);
    SaveOptionDec("key_left",   &lpOption->key_left);
    SaveOptionDec("key_right",  &lpOption->key_right);
    SaveOptionDec("key_esc",    &lpOption->key_esc);
    SaveOptionDec("key_pause",  &lpOption->key_pause);

    SaveOptionDec("score3",     &lpOption->nScore[0]);
    SaveOptionDec("score4",     &lpOption->nScore[1]);
    SaveOptionDec("score5",     &lpOption->nScore[2]);

    SaveOptionDec("sound",      &lpOption->fSound);
    SaveOptionDec("FullScreen", &lpOption->fFullScreen);
    SaveOptionDec("Suggestion", &lpOption->fSuggestion);
    SaveOptionDec("GaryMode",   &lpOption->fGaryMode);
    SaveOptionDec("Animation",  &lpOption->fAnimation);

    SaveOptionDec("AISleep",&lpOption->iAISleep);
}