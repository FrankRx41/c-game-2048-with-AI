#include "Option.h"
#include "Macro.h"
#include <stdio.h>

FILE * fp;

void PrintOption(LPOPTION lpOption){
    printf("----------------------------------------------------------------\n");
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

int SetOptionValue(int *pointer,int value){
    fprintf(fp,"%X\n",value);
    //debug("value = %d",value);
    *pointer = value;
}
int GetOptionValue(int *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    //debug("str: %s",str);
    sscanf(str,"%X",pointer);
    //debug("m3(d) = 0x%X",*pointer);
}

int SetOptionString(char *pointer,char *value){
    fprintf(fp,"%s\n",value);
    strcpy(pointer,value);
}
int GetOptionString(char *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    //debug("str: %s",str);
    strncpy(pointer,str,strlen(str)-1);
    //debug("m3(s) = %s",pointer);
}


int FindHeader(char *m1){
    char str[MAX];
    while(fgets(str,MAX,fp))
    {
        if(str[0]=='[')break;
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
    fprintf(fp,"%s=",m1);
    return 0;
}

int SetOptionVal(char *header,int *pointer,int value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        GetOptionValue(pointer);
    }
    else{
        SetOptionValue(pointer,value);
    }
    fclose(fp);
}

int SetOptionStr(char *header,char *pointer,char *value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        GetOptionString(pointer);
    }
    else{
        SetOptionString(pointer,value);
    }
    fseek(fp,0,SEEK_SET);
    fclose(fp);
}

int ReadSetting(LPOPTION lpOption){
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

    SetOptionStr("FomtName",lpOption->hFontName,    "Impact");
    SetOptionStr("SaveDate",lpOption->sSaveDate,    "2048.dat");
    SetOptionVal("FontSize",&lpOption->iFontSize,   0x22);

    SetOptionVal("0",       &lpOption->nColorTable[0],      0x1A2A3A);
    SetOptionVal("2",       &lpOption->nColorTable[1],      0x106475);
    SetOptionVal("4",       &lpOption->nColorTable[2],      0x106C7D);
    SetOptionVal("8",       &lpOption->nColorTable[3],      0x106485);
    SetOptionVal("16",      &lpOption->nColorTable[4],      0x10598D);
    SetOptionVal("32",      &lpOption->nColorTable[5],      0x205B95);
    SetOptionVal("64",      &lpOption->nColorTable[6],      0x204D9D);
    SetOptionVal("128",     &lpOption->nColorTable[7],      0x2040A5);
    SetOptionVal("256",     &lpOption->nColorTable[8],      0x2052AD);
    SetOptionVal("512",     &lpOption->nColorTable[9],      0x3034B5);
    SetOptionVal("1024",    &lpOption->nColorTable[10],     0x3036BD);
    SetOptionVal("2048",    &lpOption->nColorTable[11],     0x3035C5);
    SetOptionVal("4096",    &lpOption->nColorTable[12],     0x402ACD);
    SetOptionVal("8192",    &lpOption->nColorTable[13],     0x402CD5);
    SetOptionVal("16384",   &lpOption->nColorTable[14],     0x4B1EDD);
    SetOptionVal("32768",   &lpOption->nColorTable[15],     0x4D11E5);
    SetOptionVal("65536",   &lpOption->nColorTable[16],     0x5E12ED);
    SetOptionVal("131072",  &lpOption->nColorTable[17],     0x5E12ED);
    SetOptionVal("262144",  &lpOption->nColorTable[18],     0x5E12ED);
    SetOptionVal("524288",  &lpOption->nColorTable[19],     0x5E12ED);
    SetOptionVal("10485796",&lpOption->nColorTable[20],     0x5E12ED);

    SetOptionVal("bgColor",     &lpOption->nBgColor,        0x111240);
    SetOptionVal("textColor",   &lpOption->nTextColor,      0x9A9C9B);

    SetOptionVal("padding",     &lpOption->nPadding,        100);
    SetOptionVal("correct",     &lpOption->nCorrect,        5);

    SetOptionVal("key_up",      &lpOption->key_up,          VK_UP);
    SetOptionVal("key_down",    &lpOption->key_down,        VK_DOWN);
    SetOptionVal("key_left",    &lpOption->key_left,        VK_LEFT);
    SetOptionVal("key_right",   &lpOption->key_right,       VK_RIGHT);
    SetOptionVal("key_esc",     &lpOption->key_esc,         VK_ESCAPE);
    SetOptionVal("key_pause",   &lpOption->key_pause,       VK_SPACE);
    
    SetOptionVal("score3",  &lpOption->nScore[0],           0);
    SetOptionVal("score4",  &lpOption->nScore[1],           0);
    SetOptionVal("score5",  &lpOption->nScore[2],           0);

    SetOptionVal("sound",       &lpOption->fSound,          1);
    SetOptionVal("FullScreen",  &lpOption->fFullScreen,     0);
    SetOptionVal("Suggestion",  &lpOption->fSuggestion,     0);
    SetOptionVal("GaryMode",    &lpOption->fGaryMode,       0);
    SetOptionVal("Animation",   &lpOption->fAnimation,      1);
    
    return 0;
}