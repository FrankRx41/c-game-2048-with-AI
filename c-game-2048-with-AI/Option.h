typedef struct OPTION{
    // Game Setting
    int fSound;
    int fFullScreen;
    int fSuggestion;

    unsigned int nWidth;
    unsigned int nHeight;
    unsigned int nScore[3];
    int iLevel;

    int nMap[5][5];
    int nStep;
    unsigned long nTime;

    // System Setting
    HWND    hWnd;
    int     nColorTable[17];
    int     nBgColor;
    char    hFontName[30];
    int     iFontSize;
    int     cxClient;
    int     cyClient;
}* LPOPTION, OPTION;

void PrintOption(LPOPTION lpOption){
    printf("Sound:%d FullScreen%d Suggestion:%d\n",lpOption->fSound,lpOption->fFullScreen,lpOption->fSuggestion);
    printf("Width: %d Height: %d\n",lpOption->nWidth,lpOption->nHeight);
    printf("Score: %d %d %d\n",lpOption->nScore[0],lpOption->nScore[1],lpOption->nScore[2]);

    printf("ColorTable:\n");
    for(int i=0;i<sizeof(lpOption->nColorTable)/sizeof(lpOption->nColorTable[0]);i++){
        printf("[%d] 0x%06X\t",1<<i,lpOption->nColorTable[i]);
    }
    printf("BgColor: 0x%X",lpOption->nBgColor);
    printf("\n");
    printf("FontName: %s FontSize:%d\n",lpOption->hFontName,lpOption->iFontSize);
}


#define debug(str,...)  do{/*printf("[%d]",__LINE__);*/printf(str"\n",__VA_ARGS__);}while(0)
#define MAX     256
#define strequ(s1,s2)	(strcmp((s1),(s2))==0)
FILE * fp;

int SetOptionValue(int *pointer,int value){
    fprintf(fp,"%X\n",value);
    //debug("value = %d",value);
    *pointer = value;
}
int GetOptionValue(int *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    debug("str: %s",str);
    sscanf(str,"%X",pointer);
    debug("m3(d) = 0x%X",*pointer);
}

int SetOptionString(char *pointer,char *value){
    fprintf(fp,"%s\n",value);
    //debug("value = %s",value);
    strcpy(pointer,value);
}
int GetOptionString(char *pointer){
    char str[MAX];
    fgets(str,MAX,fp);
    //debug("str: %s",str);

    //sscanf(str,"%s",pointer);
    strncpy(pointer,str,strlen(str)-1);
    //pointer[strlen(pointer)-1] = 0;
    debug("m3(s) = %s",pointer);
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
        //printf("result is \"%s\"\n",result);
        //debug("m2: %s",str2);
        if(strncmp(m1,str2,strlen(m1)) == 0){
            int back = strlen(str2)-strlen(str);
            //debug("back: %d (%d)(%d)",back,strlen(str2),strlen(str));
            fseek(fp,back,SEEK_CUR);
            //debug("get!");
            return 1;
        }
    }
    fseek(fp,0,SEEK_END);
    //debug("no find m2: %s!",m2);
    fprintf(fp,"%s=",m1);
    return 0;
}

int SetOption(char *header,int *pointer,int value){
    fp = fopen("config.ini","r+");
    if(!fp)exit(0);
    if(FindHeader(header)){
        //debug("get3");
        GetOptionValue(pointer);
    }
    else{
        //debug("set3");
        SetOptionValue(pointer,value);
    }
    fclose(fp);
}

int SetOptionStr(char *header,char *pointer,char *value){
    fp = fopen("config.ini","r+");
    if(!fp)exit(0);
    if(FindHeader(header)){
        //debug("get3");
        GetOptionString(pointer);
    }
    else{
        //debug("set3s");
        SetOptionString(pointer,value);
    }
    fseek(fp,0,SEEK_SET);
    fclose(fp);
}

int ReadSetting(LPOPTION lpOption){
    fp = fopen("config.ini","r+");
    if(!fp){
        debug("new file");
        fp = fopen("config.ini","w+");
        //return 0;
    }
    fprintf(fp,"# this file create by 2048\n");
    fclose(fp);
    //fseek(fp,0,SEEK_SET);
    //SetOption("COLOR","haha2",&lpOption->hFontName,0x66666);
    SetOptionStr("FomtName",&lpOption->hFontName,"Impact");
    SetOption("FontSize",   &lpOption->iFontSize,0x22);
    SetOption("0",      &lpOption->nColorTable[0],      0x1A2A3A);
    SetOption("2",      &lpOption->nColorTable[1],      0x106475);
    SetOption("4",      &lpOption->nColorTable[2],      0x106C7D);
    SetOption("8",      &lpOption->nColorTable[3],      0x106485);
    SetOption("16",     &lpOption->nColorTable[4],      0x10598D);
    SetOption("32",     &lpOption->nColorTable[5],      0x205B95);
    SetOption("64",     &lpOption->nColorTable[6],      0x204D9D);
    SetOption("128",    &lpOption->nColorTable[7],      0x2040A5);
    SetOption("256",    &lpOption->nColorTable[8],      0x2052AD);
    SetOption("512",    &lpOption->nColorTable[9],      0x3034B5);
    SetOption("1024",   &lpOption->nColorTable[10],     0x3036BD);
    SetOption("2048",   &lpOption->nColorTable[11],     0x3035C5);
    SetOption("4096",   &lpOption->nColorTable[12],     0x402ACD);
    SetOption("8192",   &lpOption->nColorTable[13],     0x402CD5);
    SetOption("16384",  &lpOption->nColorTable[14],     0x4B1EDD);
    SetOption("32768",  &lpOption->nColorTable[15],     0x4D11E5);
    SetOption("65536",  &lpOption->nColorTable[16],     0x5E12ED);
    SetOption("bgColor",&lpOption->nBgColor,            0x111240);
    SetOption("score3", &lpOption->nScore[0],           0);
    SetOption("score4", &lpOption->nScore[1],           0);
    SetOption("score5", &lpOption->nScore[2],           0);

    /*fp = fopen("config.ini","r");
    if(!fp){
        perror("fopen error: ");
        return 0;
    }
    char str[MAX];
    while(fgets(str,MAX,fp)){
        printf("%s",str);
    }*/
    return 0;
}