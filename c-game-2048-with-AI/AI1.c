/*******************************************************************************
author: 	WalkerShu
date:		2017.11.16
*******************************************************************************/

#include "Macro.h"
#include "Game.h"
#include "AI.h"

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

static int AICheckIfDir1(int(*map)[5], int w, int h, int dir) {
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
					GameBlockUnite(F, S);
				}
			}
		}
		break;
	}
	return fHadmove;
}

static int isfirstrun = 1;
static int whichweightmat = 0;
static int res[26] = { 0 };
static int pmat[25][5][5] = { 0 };
static int mapweight[25] = { 0 };
static int numweight[25] = { 0 };

static int FindMaxNumInArr(int arr[], int n) {
	int max = -0xffff;
	for (int i = 0; i < n; i++) {
		if (max < arr[i]) {
			max = arr[i];
		}
	}
	return max;
}

static void RotateMat(int map1[5][5], int map2[5][5]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			map2[i][j] = map1[4 - 1 - j][i];
		}
	}
}
static void SymmetricMat(int map1[5][5], int map2[5][5]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			map2[i][j] = map1[i][4 - 1 - j];
		}
	}
}

static int GiveValueToMat(int values[16], int pmat[5][5]) {
	for (int i = 0; i < 4; i++) {
		pmat[0][i] = values[i];
		pmat[1][i] = values[i + 4];
		pmat[2][i] = values[i + 8];
		pmat[3][i] = values[i + 12];
	}
	return 1;
}
static void GiveValuesToWeightMats() {
	int matvalues1[16] = {
		16,15,14,13,
		9 ,10,11,12,
		8 , 7 ,6 ,5,
		1 , 2 ,3 ,4
	};
	int matvalues2[16] = {
		16,15,12,4,
		14,13,11,3,
		10,9 ,8 ,2,
		7 ,6 ,5 ,1
	};
	int matvalues3[16] = {
		16,15,14,4,
		13,12,11,3,
		10,9 ,8 ,2,
		7 ,6 ,5 ,1
	};

	GiveValueToMat(matvalues1, pmat[0]);
	RotateMat(pmat[0], pmat[1]);
	RotateMat(pmat[1], pmat[2]);
	RotateMat(pmat[2], pmat[3]);
	SymmetricMat(pmat[0], pmat[4]);
	RotateMat(pmat[4], pmat[5]);
	RotateMat(pmat[5], pmat[6]);
	RotateMat(pmat[6], pmat[7]);

	GiveValueToMat(matvalues2, pmat[0 + 8]);
	RotateMat(pmat[0 + 8], pmat[1 + 8]);
	RotateMat(pmat[1 + 8], pmat[2 + 8]);
	RotateMat(pmat[2 + 8], pmat[3 + 8]);
	SymmetricMat(pmat[0 + 8], pmat[4 + 8]);
	RotateMat(pmat[4 + 8], pmat[5 + 8]);
	RotateMat(pmat[5 + 8], pmat[6 + 8]);
	RotateMat(pmat[6 + 8], pmat[7 + 8]);

	GiveValueToMat(matvalues3, pmat[0 + 16]);
	RotateMat(pmat[0 + 16], pmat[1 + 16]);
	RotateMat(pmat[1 + 16], pmat[2 + 16]);
	RotateMat(pmat[2 + 16], pmat[3 + 16]);
	SymmetricMat(pmat[0 + 16], pmat[4 + 16]);
	RotateMat(pmat[4 + 16], pmat[5 + 16]);
	RotateMat(pmat[5 + 16], pmat[6 + 16]);
	RotateMat(pmat[6 + 16], pmat[7 + 16]);

}

static void GiveMapWeight() {
	mapweight[0] = 1;
	for (int i = 0; i < 19; i++) mapweight[i + 1] = 2 * mapweight[i] + 1;
}

static void GiveNumWeight() {
	numweight[0] = 1;
	for (int i = 0; i < 19; i++) numweight[i + 1] = 2 *numweight[i] + 2;
}
static int CheckWeightsSum(int map[5][5], int mapwayweight[5][5]) {
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			sum += (mapweight[mapwayweight[i][j]] * numweight[map[i][j]]);
		}
	}

	return sum;
}
static int WorstBlock(int map[5][5], int weight[5][5]) {
	int big = 16;
	while (big) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (big == weight[i][j] && map[i][j] == 0) {
					map[i][j] = 1;
					return 1;
				}
			}
		}
		big--;
	}
	return 0;
}

static int AICheckWeightMat1(int map[5][5]) {
	int v[8] = { 0 };

	for (int i = 0; i <= 7; i++) {
		v[i] = CheckWeightsSum(map, pmat[i]);
		res[i] = v[i];
	}
	return FindMaxNumInArr(v, 8);
}
static int AICheckWeightMat2(int map[5][5]) {
	int v[8] = { 0 };

	for (int i = 0; i <= 7; i++) {
		v[i] = CheckWeightsSum(map, pmat[i + 8]);
		res[i + 8] = v[i];
	}
	return FindMaxNumInArr(v, 8);
}
static int AICheckWeightMat3(int map[5][5]) {
	int v[8] = { 0 };

	for (int i = 0; i <= 7; i++) {
		v[i] = CheckWeightsSum(map, pmat[i + 16]);
		res[i + 16] = v[i];
	}
	return FindMaxNumInArr(v, 8);
}
static int AICheckAllWeightMats(int map[5][5]) {
	int v[4] = { 0 };

	v[0] = AICheckWeightMat1(map);
	v[1] = AICheckWeightMat2(map);
	v[2] = AICheckWeightMat3(map);
	int max = FindMaxNumInArr(v, 3);

	for (int i = 0; i <= 23; i++) {
		if (max == res[i]) {
			whichweightmat = i;
		}
	}
	return max;
}


int AI1(int mCurMap[5][5], int w, int h) {

	float tree[5][5] =
	{   
        -0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
		-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,-0xFFFF,
    };

	int mNextMap[5][5]  = { 0 };
	int mNextMap1[5][5] = { 0 };
	int v = 0;

	if (isfirstrun) {
		GiveMapWeight();
		GiveNumWeight();
		GiveValuesToWeightMats();
		isfirstrun = 0;
	}

	for (int dir = 1; dir <= 4; dir++) {

		memcpy(mNextMap, mCurMap, sizeof(mNextMap));
		v = AICheckIfDir1(mNextMap, w, h, dir);
		if (v) {
            //第一层结果保存在x[1-4][0]
			tree[dir][0] += AICheckAllWeightMats(mNextMap);
			WorstBlock(mNextMap, pmat[whichweightmat]);

			for (int dir1 = 1; dir1 <= 4; dir1++) {

				memcpy(mNextMap1, mNextMap, sizeof(mNextMap1));
				v = AICheckIfDir1(mNextMap1, w, h, dir1);
				if (v) {
                    //第二层结果在x[1-4][1-4]
					tree[dir][dir1] += AICheckAllWeightMats(mNextMap1, w, h);
				}
			}
		}
	}

	int max = -0xFFFF;
	for (int i = 1; i <= 4; i++) {
		if (tree[i][0] != -0xFFFF) {

			max = -0xFFFF;
			for (int j = 1; j <= 4; j++) {
				if (max < tree[i][j]) {
					max = tree[i][j];
				}
			}
			if (max != -0xFFFF) {
                //第二层结果最大值与第一层结果相加，保存在第一层作为最终结果
				tree[i][0] += max;
			}
		}

	}

	int maxpoint = -0xFFFF;
	int dir = 0;

	for (int i = 1; i <= 4; i++) {
		if (tree[i][0] == -0xFFFF) {
			continue;
		}
		if (maxpoint < tree[i][0]) {
			maxpoint = tree[i][0];
			dir = i;
		}

	}

	return AIDebugPrintDir("Goto",dir);
}
