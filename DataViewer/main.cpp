#include <stdio.h>
#include <windows.h>

int main(int argc,char **argv){
    system("title 2048 data viewer");
    if(argc == 1){
        printf("Drag your savedata here and viewer it.\n");
        system("pause");
        return 0;
    }
    FILE * fp = fopen(argv[1],"rb");
    if(!fp){
        perror("can not open file: ");
        return 0;
    }

    int w,h,score,level,step,map[5][5];

    fread(&w,sizeof(w),1,fp);
    fread(&h,sizeof(h),1,fp);
    fread(&score,sizeof(score),1,fp);
    fread(&level,sizeof(level),1,fp);
    fread(&step,sizeof(step),1,fp);
    fread(&map,sizeof(map),1,fp);

    fclose(fp);

    printf("width: %d  height: %d\n",w,h);
    printf("score: %d  step: %d\n",score,step);
    for(int x=0;x<h;x++){
        for(int y=0;y<w;y++){
            printf("%d\t",map[x][y]);
        }
        printf("\n");
    }

    system("pause");
}