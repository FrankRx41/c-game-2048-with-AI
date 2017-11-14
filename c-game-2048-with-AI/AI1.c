#include "Macro.h"
#include "Game.h"
#include "AI.h"

// https://www.zhihu.com/question/54217135
/*
三个权值矩阵如下：
16,15,14,13
9 ,10,11,12
8 ,7 ,6 ,5 
1 ,2 ,3 ,4 
16,15,12,4 
14,13,11,3
10,9 ,8 ,2 
7 ,6 ,5 ,1
16,15,14,4
13,12,11,3
10,9 ,8 ,2
7 ,6 ,5 ,1
*/

#define GameBlockUnite(F,S) if(*S != 0){                    \
                                if(*F == 0){                \
                                    *F = *S;                \
                                    *S = 0;                 \
                                    fHadmove = 1;           \
                                }                           \
                                else if(*F == *S){          \
                                    *F = *F+1;              \
                                    *S = 0;                 \
                                    fHadmove = 1;           \
                                    fUniting = 1;           \
                                    break;                  \
                                }                           \
                                else break;                 \
                            }


static int AIDebugPrintDir0(char *str, int dir) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
	switch (dir) {
	case DIR_UP:
		debug("AI %s: Up", str);
		break;
	case DIR_DOWN:
		debug("AI %s: Down", str);
		break;
	case DIR_LEFT:
		debug("AI %s: Left", str);
		break;
	case DIR_RIGHT:
		debug("AI %s: Right", str);
		break;
	default:
		debug("AI %s error!", str);
		break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
	return dir;
}

static int AICheckIfDir0(int(*map)[5], int w, int h, int dir) {
	int fHadmove = 0;
	int fUniting = 0;
	switch (dir) {
	case DIR_UP:
		forp(i, w) {
			for (int j = 0; j<h - 1; j++) {
				int *F = &map[j][i];
				int *S = NULL;

				for (int k = j + 1; k<h; k++) {
					S = &map[k][i];
					//debug("(map[%d][%d]: %d)find map[%d][%d]: %d",j,i,map[j][i],k,i,map[k][i]);
					GameBlockUnite(F, S);
				}
			}
		}
		break;
	case DIR_DOWN:
		forp(y, w) {
			for (int x = h - 1; x>0; x--) {
				int *F = &map[x][y];
				int *S = NULL;

				for (int i = x - 1; i >= 0; i--) {
					S = &map[i][y];
					//debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
					GameBlockUnite(F, S);
				}
			}
		}
		break;
	case DIR_LEFT:
		forp(x, h) {
			for (int y = 0; y<w - 1; y++) {
				int *F = &map[x][y];
				int *S = NULL;

				for (int i = y + 1; i<w; i++) {
					S = &map[x][i];
					//debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
					GameBlockUnite(F, S);
				}
			}
		}
		break;
	case DIR_RIGHT:
		forp(x, h) {
			for (int y = w - 1; y>0; y--) {
				int *F = &map[x][y];
				int *S = NULL;

				for (int i = y - 1; i >= 0; i--) {
					S = &map[x][i];
					//debug("(map[%d][%d]: %d)find map[%d][%d]: %d",x,y,map[x][y],i,y,map[i][y]);
					GameBlockUnite(F, S);
				}
			}
		}
		break;
	}
	return fHadmove;
}

static int AIFindMaxNum0(const int(*map)[5], int w, int h) {
	int max = 0;
	forp(x, h)forp(y, w) {
		if (map[x][y] > max) {
			max = map[x][y];
		}
	}
	return max;
}



int MapWeight[5][5] = { 0 };
int NumWeight[20] = { 0 };
int BetweenNumsWeight[20][20] = { 0 };
int BestMapWeight[5][5] = { 0 };
int BestNumWeight[20] = { 0 };
int BestBetweenNumsWeight[20][20] = { 0 };
int highestscores = -999;


int MapWay1Weight1[5][5] =
{ 16,15,14,13,0,
9 ,10,11,12,0,
8 , 7 ,6 ,5,0,
1 , 2 ,3 ,4 };
int MapWay1Weight2[5][5] = { 0 };
int MapWay1Weight3[5][5] = { 0 };
int MapWay1Weight4[5][5] = { 0 };
int MapWay1Weight5[5][5] = { 0 };
int MapWay1Weight6[5][5] = { 0 };
int MapWay1Weight7[5][5] = { 0 };
int MapWay1Weight8[5][5] = { 0 };

int MapWay2Weight1[5][5] =
{ 16,15,12,4,0,
14,13,11,3,0,
10,9 ,8 ,2,0,
7 ,6 ,5 ,1 };
int MapWay2Weight2[5][5] = { 0 };
int MapWay2Weight3[5][5] = { 0 };
int MapWay2Weight4[5][5] = { 0 };
int MapWay2Weight5[5][5] = { 0 };
int MapWay2Weight6[5][5] = { 0 };
int MapWay2Weight7[5][5] = { 0 };
int MapWay2Weight8[5][5] = { 0 };

int MapWay3Weight1[5][5] =
{ 16,15,14,4,0,
13,12,11,3,0,
10,9 ,8 ,2,0,
7 ,6 ,5 ,1 };
int MapWay3Weight2[5][5] = { 0 };
int MapWay3Weight3[5][5] = { 0 };
int MapWay3Weight4[5][5] = { 0 };
int MapWay3Weight5[5][5] = { 0 };
int MapWay3Weight6[5][5] = { 0 };
int MapWay3Weight7[5][5] = { 0 };
int MapWay3Weight8[5][5] = { 0 };

int findArrMax(int arr[], int n) {
	int max = -999;
	for (int i = 0; i < n; i++) {
		if (max < arr[i]) {
			max = arr[i];
		}
	}
	return max;
}
int ArrSum(int arr[], int n) {
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += arr[i];
	}
	return sum;
}
int outmap(int map[5][5]) {
	printf("\n---------------\n");
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			printf("%d\t", map[i][j]);
		}
		printf("\n");
	}
	printf("\n---------------\n");
}
void CicleMap(int map1[5][5], int map2[5][5], int w, int h) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			map2[i][j] = map1[w - 1 - j][i];
		}
	}
}


void SymmetricMap(int map1[5][5], int map2[5][5], int w, int h) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			map2[i][j] = map1[i][w - 1 - j];
		}
	}
}
int mapweight(int mapweight) {
	int weight[20] = { 0 };
	weight[0] = 1;
	//for (int i = 0; i < 19; i++) weight[i + 1] = 3 * weight[i] + 10;//512
	for (int i = 0; i < 19; i++) weight[i + 1] = 2 * weight[i] + 1;
	//	for (int i = 9; i < 19; i++) weight[i + 1] = 2 * weight[i] + 100;

	return weight[mapweight];
}
int numweight(int num) {
	int weight[20] = { 0 };
	weight[0] = 1;
	//for (int i = 0; i < 19; i++) weight[i + 1] = weight[i] * weight[i] + 2;//512
	for (int i = 0; i < 19; i++) weight[i + 1] = 2 * weight[i] + 2;
	//for (int i = 9; i < 19; i++) weight[i + 1] = 2 * weight[i] + 100;

	return weight[num];
}
void GiveMapWeight() {
	/*debug("\n=============giveweight_begin=============\n");
	debug("\n---------wei1------------\n");*/
	CicleMap(MapWay1Weight1, MapWay1Weight2, 4, 4);
	CicleMap(MapWay1Weight2, MapWay1Weight3, 4, 4);
	CicleMap(MapWay1Weight3, MapWay1Weight4, 4, 4);
	SymmetricMap(MapWay1Weight1, MapWay1Weight5, 4, 4);
	CicleMap(MapWay1Weight5, MapWay1Weight6, 4, 4);
	CicleMap(MapWay1Weight6, MapWay1Weight7, 4, 4);
	CicleMap(MapWay1Weight7, MapWay1Weight8, 4, 4);

	/*outmap(MapWay1Weight1); outmap(MapWay1Weight2);
	outmap(MapWay1Weight3); outmap(MapWay1Weight4);
	outmap(MapWay1Weight5); outmap(MapWay1Weight6);
	outmap(MapWay1Weight7); outmap(MapWay1Weight8);*/


	//debug("\n---------wei2--------------\n");

	CicleMap(MapWay2Weight1, MapWay2Weight2, 4, 4);
	CicleMap(MapWay2Weight2, MapWay2Weight3, 4, 4);
	CicleMap(MapWay2Weight3, MapWay2Weight4, 4, 4);
	SymmetricMap(MapWay2Weight1, MapWay2Weight5, 4, 4);
	CicleMap(MapWay2Weight5, MapWay2Weight6, 4, 4);
	CicleMap(MapWay2Weight6, MapWay2Weight7, 4, 4);
	CicleMap(MapWay2Weight7, MapWay2Weight8, 4, 4);

	/*outmap(MapWay2Weight1); outmap(MapWay2Weight2);
	outmap(MapWay2Weight3); outmap(MapWay2Weight4);
	outmap(MapWay2Weight5); outmap(MapWay2Weight6);
	outmap(MapWay2Weight7); outmap(MapWay2Weight8);*/

	//debug("\n---------wei3--------------\n");

	CicleMap(MapWay3Weight1, MapWay3Weight2, 4, 4);
	CicleMap(MapWay3Weight2, MapWay3Weight3, 4, 4);
	CicleMap(MapWay3Weight3, MapWay3Weight4, 4, 4);
	SymmetricMap(MapWay3Weight1, MapWay3Weight5, 4, 4);
	CicleMap(MapWay3Weight5, MapWay3Weight6, 4, 4);
	CicleMap(MapWay3Weight6, MapWay3Weight7, 4, 4);
	CicleMap(MapWay3Weight7, MapWay3Weight8, 4, 4);

	//outmap(MapWay3Weight1); outmap(MapWay3Weight2);
	//outmap(MapWay3Weight3); outmap(MapWay3Weight4);
	//outmap(MapWay3Weight5); outmap(MapWay3Weight6);
	//outmap(MapWay3Weight7); outmap(MapWay3Weight8);
	//debug("===============giveweight_end================\n");
}
int AICheckMapWayWeight(int map[5][5], int w, int h, int mapwayweight[5][5]) {
	int sum = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			sum += (mapweight(mapwayweight[i][j]) * numweight(map[i][j]));
		}
	}
	return sum;
}


int AICheckAllMapWay1Weight(int map[5][5], int w, int h) {
	int v[8] = { 0 };
	v[0] = AICheckMapWayWeight(map, w, h, MapWay1Weight1);
	v[1] = AICheckMapWayWeight(map, w, h, MapWay1Weight2);
	v[2] = AICheckMapWayWeight(map, w, h, MapWay1Weight3);
	v[3] = AICheckMapWayWeight(map, w, h, MapWay1Weight4);
	v[4] = AICheckMapWayWeight(map, w, h, MapWay1Weight5);
	v[5] = AICheckMapWayWeight(map, w, h, MapWay1Weight6);
	v[6] = AICheckMapWayWeight(map, w, h, MapWay1Weight7);
	v[7] = AICheckMapWayWeight(map, w, h, MapWay1Weight8);
	return findArrMax(v, 8);
}
int AICheckAllMapWay2Weight(int map[5][5], int w, int h) {
	int v[8] = { 0 };
	v[0] = AICheckMapWayWeight(map, w, h, MapWay2Weight1);
	v[1] = AICheckMapWayWeight(map, w, h, MapWay2Weight2);
	v[2] = AICheckMapWayWeight(map, w, h, MapWay2Weight3);
	v[3] = AICheckMapWayWeight(map, w, h, MapWay2Weight4);
	v[4] = AICheckMapWayWeight(map, w, h, MapWay2Weight5);
	v[5] = AICheckMapWayWeight(map, w, h, MapWay2Weight6);
	v[6] = AICheckMapWayWeight(map, w, h, MapWay2Weight7);
	v[7] = AICheckMapWayWeight(map, w, h, MapWay2Weight8);
	return findArrMax(v, 8);
}
int AICheckAllMapWay3Weight(int map[5][5], int w, int h) {
	int v[8] = { 0 };
	v[0] = AICheckMapWayWeight(map, w, h, MapWay3Weight1);
	v[1] = AICheckMapWayWeight(map, w, h, MapWay3Weight2);
	v[2] = AICheckMapWayWeight(map, w, h, MapWay3Weight3);
	v[3] = AICheckMapWayWeight(map, w, h, MapWay3Weight4);
	v[4] = AICheckMapWayWeight(map, w, h, MapWay3Weight5);
	v[5] = AICheckMapWayWeight(map, w, h, MapWay3Weight6);
	v[6] = AICheckMapWayWeight(map, w, h, MapWay3Weight7);
	v[7] = AICheckMapWayWeight(map, w, h, MapWay3Weight8);
	return findArrMax(v, 8);
}
int AICheckAllMapWaysWeight(int map[5][5], int w, int h) {
	int v[4] = { 0 };

	v[0] = AICheckAllMapWay1Weight(map, w, h);
	v[1] = AICheckAllMapWay2Weight(map, w, h);
	v[2] = AICheckAllMapWay3Weight(map, w, h);

	return findArrMax(v, 3);
}












int AICheckAll0(const int(*mNextMap)[5], int w, int h) {

	//int max = AIFindMaxNum(mNextMap, w, h);

	int sum = 0;
	//sum += AICheckAllRandWeight(mNextMap, w, h);
	sum += AICheckAllMapWaysWeight(mNextMap, w, h);
	//debug("WEIGHT_BLANK: %g", v);

	return sum;
}

int AI1(int mCurMap[5][5], int w, int h) {

	int x[5][5] =
	{ -0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF ,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF ,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF };

	int mNextMap[5][5]; int mNextMap1[5][5];
	int v = 0;
	GiveMapWeight();

	for (int dir = 1; dir <= 4; dir++) {
		memcpy(mNextMap, mCurMap, sizeof(mNextMap));
		v = AICheckIfDir0(mNextMap, w, h, dir);
		if (v) {
			x[dir][0] += AICheckAll0(mNextMap, w, h);//x[1-4][0]

			for (int dir1 = 1; dir1 <= 4; dir1++) {
				memcpy(mNextMap1, mNextMap, sizeof(mNextMap1));
				v = AICheckIfDir0(mNextMap1, w, h, dir1);
				x[dir][dir1] += AICheckAll0(mNextMap1, w, h);//x[1-4][1-4]
			}
		}

	}
	int max = -0xFFFF;
	for (int i = 1; i <= 4; i++) {
		if (x[i][0] == -0xFFFF) {
			continue;
		}

		max = -0xFFFF;
		for (int j = 1; j <= 4; j++) {
			if (max < x[i][j]) {
				max = x[i][j];
			}
		}
		x[i][0] += max;

	}

	int maxpoint = -0xFFFF;
	int dir = 0;

	for (int i = 1; i <= 4; i++) {
		if (x[i][0] == -0xFFFF) {
			continue;
		}
		if (maxpoint < x[i][0]) {
			maxpoint = x[i][0];
			dir = i;
		}
	}

	return AIDebugPrintDir("Goto",dir);
}