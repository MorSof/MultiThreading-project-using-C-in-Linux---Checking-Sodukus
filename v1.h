/*
 * v1.h
 *
 *  Created on: 24 May 2019
 *      Author: mor
 */

#ifndef V1_H_
#define V1_H_

typedef struct {
	int numOfSubMat;
	int* address;
} SubMat;

typedef struct {

	int numOfNode;

} CheckerStruct;

extern int soduku[9][9];

void checkRes(int *totalRes);
void* createThreads();
void readSoduku(int** soduku);
void* checkRow(void* temp);
void* checkCol(void* temp);
void* readSquare(void* temp);
int checkSquare(int* mat);
SubMat** createSubMatThreads(int threadIndex);
void insertNumberToSoduku();
void readSodukuFromFile(char* fileName);
void mainThreadWait();
void freeAll(CheckerStruct** checkerStrucktArr, SubMat** subMatArr);

#endif /* V1_H_ */
