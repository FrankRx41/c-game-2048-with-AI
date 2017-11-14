#include <stdio.h>
#include <windows.h>

int main(){
    system("title 2048 data creator (4*4)");
    int w,h,score,level,step,map[5][5];

    w = 4;
    h = 4;
    score = -1;
    step = -1;
    level = 4;
    while(1)
    {

        printf("input your tile[4][4]:(Use Ctrl+C to exit)\n");
        for(int x=0;x<h;x++){
            for(int y=0;y<w;y++){
                scanf("%2d",&map[x][y]);
            }
        }

        FILE * fp = fopen("2048.dat","wb");
        if(!fp){
            perror("can not open file: ");
            return 0;
        }
        fwrite(&w,sizeof(w),1,fp);
        fwrite(&h,sizeof(h),1,fp);
        fwrite(&score,sizeof(score),1,fp);
        fwrite(&level,sizeof(level),1,fp);
        fwrite(&step,sizeof(step),1,fp);
        fwrite(&map,sizeof(map),1,fp);
        fclose(fp);

        //printf("width: %d  height: %d\n",w,h);
        //printf("score: %d  step: %d\n",score,step);
        for(int x=0;x<h;x++){
            for(int y=0;y<w;y++){
                printf("%d\t",map[x][y]);
            }
            printf("\n");
        }
    }
    system("pause");
}