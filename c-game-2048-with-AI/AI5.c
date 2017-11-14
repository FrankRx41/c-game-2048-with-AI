#include "Macro.h"
#include "Game.h"
#include "AI.h"

#define GameBlockUnite(F,S) if(*S != 0){                    \
                                if(*F == 0){                \
                                    *F = *S;                \
                                    *S = 0;                 \
                                    fMoved = 1;             \
                                }                           \
                                else if(*F == *S){          \
                                    *F = *F+1;              \
                                    *S = 0;                 \
                                    fMoved = 1;             \
                                    fMerge = 1;             \
                                    /* new up score rule */ \
                                    lpOption->nCurScore += 1<<*F;   \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }                               \
                            // i think this debug info will not be use any more
                            //debug("(map[%d][%d]: %d)find map[%d][%d]: %d",j,i,map[j][i],k,i,map[k][i]);

/*
int GameDirKey(LPOPTION lpOption,int dir){
    int fMoved = 0;
    int fMerge = 0;
    int(*map)[5] = lpOption->mMap;
    int w = lpOption->nWidth;
    int h = lpOption->nHeight;

    switch(dir){
    case DIR_UP:
        forp(i,w){
            for(int j=0;j<h-1;j++){
                int *F = &map[j][i];
                int *S = NULL;

                for(int k=j+1;k<h;k++){
                    S = &map[k][i];
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_DOWN:
        forp(y,w){
            for(int x=h-1;x>0;x--){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=x-1;i>=0;i--){
                    S = &map[i][y];
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_LEFT:
        forp(x,h){
            for(int y=0;y<w-1;y++){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y+1;i<w;i++){
                    S = &map[x][i];
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    case DIR_RIGHT:
        forp(x,h){
            for(int y=w-1;y>0;y--){
                int *F = &map[x][y];
                int *S = NULL;

                for(int i=y-1;i>=0;i--){
                    S = &map[x][i];
                    GameBlockUnite(F,S);
                }
            }
        }
        break;
    }

    if(fMoved){
        CreatOneTile(lpOption);
        lpOption->nStep++;
        if(lpOption->fSound){
            if(fMerge){
                //debug("playsound");
                PlaySound(TEXT("MERGESOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }else{
                //PlaySound(TEXT("3.wav"),0,SND_FILENAME|SND_ASYNC);
                PlaySound(TEXT("CREATSOUND"),GetModuleHandle("resource.rc"),SND_RESOURCE|SND_ASYNC);
            }
        }
    }
    return fMoved;
}

double Mononess(int map[5][5],int w,int h)
{
    double total[] = {0,0,0,0};

    // 纵向
    for(int i=0;i<4;i++)
    {
        int curr = 0;
        int next = curr + 1;
        double currValue,nextValue;

        while(next < w)
        {
            while( next<w && map[i][next]==0 )next++;
            if(next == 4)next--;

            currValue = (map[i][curr] > 0) ? map[i][curr] : 0;
            nextValue = (map[i][next] > 0) ? map[i][next] : 0;

            if(currValue > nextValue){
                total[0] += nextValue - currValue;
            }
            else if(nextValue > currValue){
                total[1] += currValue - nextValue;
            }
            curr = next;
            next++;
        }
    }

    // 横向
    for(int j = 0; j < 4; j++)
    {
        int curr = 0;
        int next = curr + 1;
        double currValue,nextValue;

        while(next < h)
        {
            while( next<h && map[next][j]==0 )next++;
            if(next == 4)next--;

            currValue = (map[curr][j] > 0) ? map[curr][j] : 0;
            nextValue = (map[next][j] > 0) ? map[next][j] : 0;

            if(currValue > nextValue){
                total[2] += nextValue - currValue;
            }
            else if(nextValue > currValue){
                total[3] += currValue - nextValue;
            }
            curr = next;
            next++;
        }
    }
    return (max(total[0],total[1]) + max(total[2],total[3]));
}

double Smoothness(int map[5][5],int w,int h)
{
    double smoothness = 0;
    // 向右 向下两个方向
    for(int i = 0; i < h; i++)
    for(int j = 0; j < w; j++){
        if(map[i][j] <= 0){
            continue;
        }

        double currentvalue = map[i][j];
        double nextvalue = 0;
        // 对每一个非0格子，向下向右分别检索最近的非空格子
        // 向右
        int jj;
        for(jj = j + 1; jj < 4; jj++)
            if(map[i][jj] > 0)
                break;
        if(jj < 4)
        {
            nextvalue = map[i][jj];
            smoothness -= abs(currentvalue - nextvalue);
        }
        // 向下
        int ii;
        for(ii = i + 1; ii < 4; ii++)
            if(map[ii][j] > 0)
                break;
        if(ii < 4)
        {
            nextvalue = map[ii][j];
            smoothness -= abs(currentvalue - nextvalue);
        }
    }
    return smoothness;
}

// 最大数，后期乘以maxweight(干脆方法内乘了）
int Maxness(int map[5][5],int w,int h)
{
    int max = 0;
    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j++)
            if(map[i][j] > max)
                max = map[i][j];
    return max;
}

int Emptyness(int map[5][5],int w,int h)
{
    int blanks=0;
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++)
            if(map[i][j]==0)
                blanks++;
    return blanks;
}


//double GetScore()
//{
//    return Emptyness()*emptyweight + Math.log(Maxness()) / Math.log(2) * maxweight
//        + Smoothness() * smoothweight + Mononess()*monoweight;
//}


// 0,1,2,3分别代表：LeftReduce RightReduce UpReduce DownReduce
// changesum为true表示积分可以更改，否则保持不变（用于搜索）
int Reduce(int i)
{
    if(i==0)
        return left();
    if(i==1)
        return right();
    if(i==2)
        return up();
    if(i==3)
        return down();
    return 0;
}

int SearchMax(int map[5][5],int w,int h,int nowdeepth,double alpha_in,double beta_in)
{
    double alpha    = alpha_in;
    double beta     = beta_in;
    int temphere[5][5] = {0};
    int bestreduce = -1;// 此步所得的最佳reduce方向
    // 备份当前格局
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            temphere[i][j] = map[i][j];
    int temp=scores;

    // 叶节点，递归出口
    if(nowdeepth >= maxdeepth)
    {
        for(int i = 0; i < 4; i++)
            if(Reduce(i)!=0)
            {
                double score = GetScore();
                if(score > alpha || bestreduce == -1)
                {
                    if(score > alpha)
                        alpha = score;
                    bestreduce = i;// 修改最佳reduce方向
                }
                // 恢复原先格局
                for(int ii = 0; ii < 4; ii++)
                    for(int jj = 0; jj < 4; jj++)
                        map[ii][jj] = temphere[ii][jj];
                scores=temp;
            }
        Map.Entry<Double,Double> result = new AbstractMap.SimpleEntry<Double,Double>(alpha,beta);
        searchresult = bestreduce;
        return result;
    }
    // 非叶节点
    for(int i = 0; i < 4; i++)
    {
        if(Reduce(i)!=0)
        {
            // min节点的beta赋给父节点的alpha：若比父节点alpha大
            Map.Entry<Double,Double> entry = SearchMin2(nowdeepth + 1,alpha,beta);
            if(entry.getValue().doubleValue() > alpha || bestreduce == -1)
            {
                if(entry.getValue().doubleValue() > alpha)
                    alpha = entry.getValue().doubleValue();
                bestreduce = i;// 修改最佳reduce方向
            }
            // 恢复原先格局
            for(int ii = 0; ii < 4; ii++)
                for(int jj = 0; jj < 4; jj++)
                    map[ii][jj] = temphere[ii][jj];
            scores=temp;
            if(alpha >= beta)// alpha-beta剪枝
                break;
        }
    }
    Map.Entry<Double,Double> result = new AbstractMap.SimpleEntry<Double,Double>(alpha,beta);
    searchresult = bestreduce;
    return result;
}*/

int AI5(int map[5][5],int w,int h)
{
    int v[] = {1,4,2,3};
    static int i = 0;
    i++;
    i%=4;
    //debug("(%d)",v[i]);
    return AIDebugPrintDir("Goto",v[i]);
}

//int AI4(int map[5][5],int w,int h){
//    return 1;
//}