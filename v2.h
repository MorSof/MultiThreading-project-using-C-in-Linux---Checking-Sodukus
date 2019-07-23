/*
 * v2.h
 *
 *  Created on: 24 May 2019
 *      Author: mor
 */

#ifndef V2_H_
#define V2_H_

typedef struct {

	int threadNum;
	int numberOfTasksPerThread;

} ThreadInfo;

typedef struct {

	int numOfNode;

} CheckerStruct;

char** createList();
void readRows(int count, int numOfRow);
void createTasks();
void createSubTasks(int offset, char* str);
void* makeTask();
void whatToDo(char* task);
void incrementSharedResult();
void loadSubMatAddress();
void createThreads();
void readSodukuFromFile(char* fileName);
void insertNumberToSoduku();
void checkRow(CheckerStruct* checkerStruct);
void checkCol(CheckerStruct* checkerStruct);
int checkSquare(int* mat);
void destroyAll();

#endif /* V2_H_ */
