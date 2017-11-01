#include "Option.h"
#include "Macro.h"
#include <stdio.h>

static FILE * fp;
static int fCreateFile  = 0;
static int fSaveOption  = 0;
static int fHaveSeek    = 1;

int PrintOption(LPOPTION lpOption){
    debug("--------------------Option--------------------");
    debug("GameState:%04X",lpOption->iGameState);
    debug("Sound:%d   FullScreen:%d   Suggestion:%d   GaryMode:%d",lpOption->fSound,lpOption->fFullScreen,lpOption->fSuggestion,lpOption->fGaryMode);
    debug("Width:%d   Height:%d   Level:%d",lpOption->nWidth,lpOption->nHeight,lpOption->iLevel);
    debug("Score:%d   %d   %d",lpOption->nScore[0],lpOption->nScore[1],lpOption->nScore[2]);
    //debug("ColorTable:\n");
    //for(int i=0;i<sizeof(lpOption->nColorTable)/sizeof(lpOption->nColorTable[0]);i++){
    //    debug("[%d] 0x%06X\t",1<<i,lpOption->nColorTable[i]);
    //}
    //debug("BgColor:0x%X",lpOption->nBgColor);
    debug("FontName:%s   FontSize:%d\n",lpOption->hFontName,lpOption->iFontSize);
    debug("--------------------Option--------------------");
}

static int SkipSpace(char * str){
    int i=0;
    while(str[i]==' ' || str[i]=='\t'){
        i++;
    }
    return i;
}
static int ReadOptionStr(char *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    int i = SkipSpace(str);
    strncpy(pointer,str+i,strlen(str)-1-i);
    //debug("%s:[%s]",__FUNCTION__,pointer);
}
static int ReadOptionHex(int *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    sscanf(str,"%X",pointer);
    //debug("%s:[%x]",__FUNCTION__,*pointer);
}
static int ReadOptionDec(int *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    if(sscanf(str,"%d",pointer) != 1){
        fHaveSeek = 0;
        srand(time(NULL));
        int x = rand();
        debug("i guess it is rand seek, i random it: %d",x);
        *pointer = x;
    }
}

static int WriteOptionStr(char *pointer,char *value){
    if(fSaveOption){
        fprintf(fp,"%s\n",pointer);
    }else{
        fprintf(fp,"%s\n",value);
        strcpy(pointer,value);
    }
}
static int WriteOptionHex(int *pointer,int value){
    if(fSaveOption){
        fprintf(fp,"%#X\n",*pointer);
    }else{
        fprintf(fp,"%#X\n",value);
        *pointer = value;
    }
}
static int WriteOptionDec(int *pointer,int value){
    if(fSaveOption){
        fprintf(fp,"%d\n",*pointer);
    }else{
        fprintf(fp,"%d\n",value);
        *pointer = value;
    }
}

static int FindHeader(char *header){
    char str[MAX];
    while(fgets(str,MAX,fp))
    {
        if(str[0]=='\n' || str[0]=='#')continue;

        char str1[MAX];
        strcpy(str1,str);

        char *str2 = strtok(str1,"=");
        if(strncmp(header,str2,strlen(header)) == 0){
            int back = strlen(str2)-strlen(str);
            fseek(fp,back,SEEK_CUR);
            return 1;
        }
    }
    fseek(fp,0,SEEK_END);
    fprintf(fp,"%s\t= ",header);
    return 0;
}

static int OptionHex(char *header,int *pointer,int value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        ReadOptionHex(pointer);
    }
    else{
        WriteOptionHex(pointer,value);
    }
    fclose(fp);
}
static int OptionDec(char *header,int *pointer,int value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        ReadOptionDec(pointer);
    }
    else{
        WriteOptionDec(pointer,value);
    }
    fclose(fp);
}
static int OptionStr(char *header,char *pointer,char *value){
    fp = fopen("config.ini","r+");
    if(FindHeader(header)){
        ReadOptionStr(pointer);
    }
    else{
        WriteOptionStr(pointer,value);
    }
    fclose(fp);
}

static int WriteComment(char *str){
    fp = fopen("config.ini","r+");
    fseek(fp,0,SEEK_END);
    if(str[0] == '[')fprintf(fp,"\n");
    fprintf(fp,"%s\n",str);
    fclose(fp);
}

static int CreateRandSeek(LPOPTION lpOption){
    fHaveSeek = 0;
    srand(time(NULL));
    int x = rand();
    debug("old random seek: %d\nnew random seek: %d",lpOption->iRandseek,x);
    lpOption->iRandseek = x;
}

static int ReadAndSaveOption(LPOPTION lpOption){
    if(fCreateFile)WriteComment("# this file create by 2048");
    if(fCreateFile)WriteComment("[FONT]");
    OptionStr("FomtName",   &lpOption->hFontName,       "Impact");
    OptionDec("FontSize",   &lpOption->iFontSize,       34);

    if(fCreateFile)WriteComment("[COLOR]");
    if(fCreateFile)WriteComment("# format: BBGGRR");
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
    OptionHex("10485..",   &lpOption->nColorTable[20], 0x5E12ED);

    OptionHex("bground",    &lpOption->nBgColor,        0x111240);
    OptionHex("text",       &lpOption->nTextColor,      0x9A9C9B);

    if(fCreateFile)WriteComment("[SIZE]");
    OptionDec("width",      &lpOption->nTileWidth,      100);
    OptionDec("margin",     &lpOption->nMargin,         5);
    OptionDec("posX",       &lpOption->nWinPosX,        -1);
    OptionDec("posY",       &lpOption->nWinPosY,        -1);

    if(fCreateFile)WriteComment("[KEY]");
    OptionDec("Up",         &lpOption->vKeyUp,          VK_UP);
    OptionDec("Down",       &lpOption->vKeyDown,        VK_DOWN);
    OptionDec("Left",       &lpOption->vKeyLeft,        VK_LEFT);
    OptionDec("Right",      &lpOption->vKeyRight,       VK_RIGHT);
    OptionDec("Exit",       &lpOption->vKeyEscape,      VK_ESCAPE);
    OptionDec("Pause",      &lpOption->vKeyPause,       VK_SPACE);
    OptionDec("AISpeedUp",  &lpOption->vKeyAISpeedUp,   VK_SUBTRACT);
    OptionDec("AISpeedDown",&lpOption->vKeyAISpeedDown, VK_ADD);

    if(fCreateFile)WriteComment("[SCORE]");
    OptionDec("score3*4",   &lpOption->nScore[0],       0);
    OptionDec("score4*4",   &lpOption->nScore[1],       0);
    OptionDec("score5*4",   &lpOption->nScore[2],       0);

    if(fCreateFile)WriteComment("[OPTION]");
    OptionStr("SaveDate",   &lpOption->sSaveDate,       "2048.dat");
    OptionDec("PlaySound",  &lpOption->fSound,          1);
    OptionDec("FullScreen", &lpOption->fFullScreen,     0);
    OptionDec("Suggestion", &lpOption->fSuggestion,     0);
    OptionDec("GaryMode",   &lpOption->fGaryMode,       0);
    OptionDec("Animation",  &lpOption->fAnimation,      1);

    OptionDec("AISleepTime",&lpOption->iAISleep,        300);

    if(fCreateFile)WriteComment("# keep rand seek null to random it");
    fp = fopen("config.ini","r+");
    if(FindHeader("RandSeek")){
        char str[MAX];
        fgets(str,MAX,fp);
        if(sscanf(str,"%d",&lpOption->iRandseek) != 1){
            CreateRandSeek(lpOption);
        }
    }
    else{
        if(fSaveOption && fHaveSeek){
            fprintf(fp,"%d\n",lpOption->iRandseek);
        }
        else{
            fprintf(fp,"\n");
            CreateRandSeek(lpOption);
        }
    }
    fclose(fp);
    
    return 0;
}

int ReadOption(LPOPTION lpOption){
    fCreateFile = 0;
    char filename[] = "config.ini";
    fp = fopen(filename,"r+");
    if(!fp){
        debug("create new config file");
        fCreateFile = 1;
        fp = fopen(filename,"w+");
        if(!fp){
            MessageBox(0,"Create Config File Error!",0,0);
            PostQuitMessage(0);
        }
    }
    fclose(fp);
    ReadAndSaveOption(lpOption);
    return 1;
}

int SaveOption(LPOPTION lpOption){
    fCreateFile = 1;
    fSaveOption = 1;
    char filename[] = "config.ini";
    fp = fopen(filename,"w");
    if(!fp){
        MessageBox(0,"Open Config File Error!",0,0);
        return 0;
    }
    //fprintf(fp,"# this file create by 2048\n");
    fclose(fp);
    ReadAndSaveOption(lpOption);
    return 1;
}