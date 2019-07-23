#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "v1.h"

#define SIZE_ROWS  9
#define SIZE_COLS  9
#define ARR_SIZE 27
#define ONE 1
int totalRes[ARR_SIZE];
int soduku[SIZE_ROWS][SIZE_COLS];
pthread_t arrThreads[ARR_SIZE];

int main(int argc, char* argv[]) {
	if (argc > 1) {
		readSodukuFromFile(argv[1]);
	} else {
		insertNumberToSoduku();
	}
	createThreads();
	checkRes(totalRes);
	return 0;
}

void readSodukuFromFile(char* fileName) {
	FILE *fp;
	int i, j;
	fp = fopen(fileName, "rt");
	for (i = 0; i < SIZE_ROWS; i++) {
		for (j = 0; j < SIZE_COLS; j++) {
			fscanf(fp, "%d", &soduku[i][j]);
		}
	}
}

void insertNumberToSoduku() {
	int i, j;
	printf(
			"Please enter numbers for your Soduku, press enter after every number:\n");
	for (i = 0; i < SIZE_ROWS; i++) {
		for (j = 0; j < SIZE_COLS; j++) {
			scanf("%d", &soduku[i][j]);
		}
	}
}

void checkRes(int *totalRes) {
	int i;
	int sum = 0;
	for (i = 0; i < ARR_SIZE; i++) {
		sum += totalRes[i];
		if (totalRes[i] == 0) {
			printf("solution is not legal\n");
			return;
		}
	}
	printf("solution is legal\n");
}

void* createThreads() {
	int i;
	CheckerStruct** checkerStrucktArr = (CheckerStruct**) malloc(
			sizeof(CheckerStruct*) * SIZE_ROWS);
	for (i = 0; i < SIZE_ROWS; i++) {
		CheckerStruct* checkerStruct = (CheckerStruct*) malloc(
				sizeof(CheckerStruct));
		//adding checkerStruck to arr
		checkerStrucktArr[i] = checkerStruct;
		checkerStruct->numOfNode = i;
		pthread_create(&arrThreads[i], NULL, checkRow, checkerStruct);
		pthread_create(&arrThreads[i + SIZE_ROWS], NULL, checkCol,
				checkerStruct);
	}
	int subMatIndex = SIZE_ROWS + i;
	SubMat** subMatArr = createSubMatThreads(subMatIndex);
	mainThreadWait();
	freeAll(checkerStrucktArr, subMatArr);
	return 0;
}

SubMat** createSubMatThreads(int threadIndex) {
	int x, y, numOfSubMat = 0;
	int k = 0;
	SubMat** subMatArr = (SubMat**) malloc(sizeof(SubMat*) * SIZE_ROWS);
	for (x = 0; x < SIZE_ROWS; x += 3) {
		for (y = 0; y < SIZE_COLS; y += 3) {
			SubMat* subMat = (SubMat*) malloc(sizeof(SubMat));
			//Adding subMat to arr
			subMatArr[k++] = subMat;
			subMat->address = &soduku[x][y];
			subMat->numOfSubMat = numOfSubMat;
			pthread_create(&arrThreads[threadIndex], NULL, readSquare, subMat);
			threadIndex++;
			numOfSubMat++;
		}
	}
	return subMatArr;
}

void* checkRow(void* temp) {
	CheckerStruct* checkerStruct = (CheckerStruct*) temp;
	int res[9] = { 0 };
	int i;
	for (i = 0; i < 9; i++) {
		int n = (soduku[checkerStruct->numOfNode][i]) - 1;
		if (res[n] == 0) {
			res[(soduku[checkerStruct->numOfNode][i]) - 1]++;
		} else {

			return NULL;
		}
	}

	totalRes[checkerStruct->numOfNode]++;
	return NULL;
}

void* checkCol(void* temp) {
	CheckerStruct* checkerStruct = (CheckerStruct*) temp;
	int res[9] = { 0 };
	int i;
	for (i = 0; i < 9; i++) {
		if (res[soduku[i][checkerStruct->numOfNode] - 1] == 0) {
			res[soduku[i][checkerStruct->numOfNode] - 1]++;
		} else {
			return NULL;
		}
	}
	totalRes[checkerStruct->numOfNode + SIZE_COLS]++;
	return NULL;
}

void* readSquare(void* temp) {
	SubMat* subMat = (SubMat*) temp;
	totalRes[(subMat->numOfSubMat) + (SIZE_ROWS * 2)] = checkSquare(
			subMat->address);
	return 0;
}

int checkSquare(int* mat) {
	int i, j;
	int res[9] = { 0 };
	for (i = 0; i < SIZE_ROWS / 3; i++) {
		for (j = 0; j < SIZE_COLS / 3; j++) {
			if (res[*(mat + (SIZE_ROWS * i + j)) - 1] == 0) {
				res[*(mat + (SIZE_ROWS * i + j)) - 1]++;
			} else {
				return 0;
			}
		}
	}
	return 1;
}

void mainThreadWait() {
	int i;
	for (i = 0; i < ARR_SIZE; i++) {
		//while is terminated...
		pthread_join(arrThreads[i], NULL);
	}
}

void freeAll(CheckerStruct** checkerStrucktArr, SubMat** subMatArr) {
	int i;

	//free CheckerStructs
	for (i = 0; i < SIZE_ROWS; i++) {
		free(checkerStrucktArr[i]);
	}
	free(checkerStrucktArr);
	//free all subMats
	for (i = 0; i < SIZE_ROWS; i++) {
		free(subMatArr[i]);
	}
	free(subMatArr);
}

