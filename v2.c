/*
 * QthreadB.c
 *
 *  Created on: 6 Apr 2019
 *      Author: mor
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "v2.h"

#define SIZE_ROWS  9
#define SIZE_COLS  9
#define SIZE_SUBMAT  9
#define N 9
#define SIZE_OF_TASKS 27
#define LAST_TASK_INDEX 26
#define SODUKU_FILE 1
int soduku[SIZE_ROWS][SIZE_COLS];
char tasksArr[SIZE_OF_TASKS][19];
int* addressOfSubMat[N];
int sharedResult = 0;
int sharedTasksBegin = 0;
int tasksFinished = 0;

pthread_t arrThreads[N];
pthread_mutex_t sharedResultMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t allThreadsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sharedTasksBeginMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void main(int argc, char* argv[]) {
	if (argc > 1) {
		readSodukuFromFile(argv[SODUKU_FILE]);
	} else {
		insertNumberToSoduku();
	}
	createTasks();
	destroyAll();
}

void destroyAll(){
	pthread_mutex_destroy(&sharedResultMutex);
		pthread_mutex_destroy(&allThreadsMutex);
		pthread_mutex_destroy(&sharedTasksBeginMutex);
		pthread_cond_destroy(&condition);
}

void createTasks() {

	const int ROWS_OFFSET = 0;
	const int COLS_OFFSET = 9;
	const int SUB_OFFSET = 18;

	//loading address of submat from soduku
	loadSubMatAddress();
	createSubTasks(ROWS_OFFSET, "Check row number ");
	createSubTasks(COLS_OFFSET, "Check col number ");
	createSubTasks(SUB_OFFSET, "Check sub number ");
	createThreads();
}

void createSubTasks(int offset, char* str) {
	int i;
	for (i = 0; i < SIZE_ROWS; i++) {
		strcpy(tasksArr[i + offset], str);
		char temp[4];
		sprintf(temp, "%d", i);
		tasksArr[i + offset][17] = i + '0';
		tasksArr[i + offset][18] = '\0';
	}
}

void createThreads() {
	int threadNum;
	pthread_mutex_lock(&allThreadsMutex);
	for (threadNum = 0; threadNum < N; threadNum++) {
		pthread_create(&arrThreads[threadNum], NULL, makeTask, NULL);
	}
	pthread_cond_wait(&condition, &allThreadsMutex);
	if (sharedResult == LAST_TASK_INDEX) {
		printf("solution is legal\n");
	} else {
		printf("solution is not legal\n");
	}
	pthread_mutex_unlock(&allThreadsMutex);
}

//pull a task from pool tasks
void* makeTask() {
	char* task;
	pthread_mutex_lock(&sharedTasksBeginMutex);
	if (sharedTasksBegin < LAST_TASK_INDEX) {
		task = tasksArr[sharedTasksBegin];

		sharedTasksBegin++;
	} else {

		pthread_mutex_unlock(&sharedTasksBeginMutex);
		return NULL;
	}
	pthread_mutex_unlock(&sharedTasksBeginMutex);
	whatToDo(task);
	return NULL;
}

void whatToDo(char* taskString) {
	int result;
//the index number (r\c\s) that tell me if its a row/col/subMat
	const int LETTER_COMMAND_IN_STRING = 6;
//the index number(1-9) that tell me which number of row/col/subMat to make
	const int NUM_OF_NODE_IN_STRING = 17;
	CheckerStruct* checkerStruct = (CheckerStruct*) malloc(
			sizeof(checkerStruct));
	checkerStruct->numOfNode = (atoi(&taskString[NUM_OF_NODE_IN_STRING]));
	if (taskString[LETTER_COMMAND_IN_STRING] == 'r') {
		checkRow(checkerStruct);

	} else if (taskString[LETTER_COMMAND_IN_STRING] == 'c') {
		checkCol(checkerStruct);
		//Send to col command
	} else {
		//send to sub command
		result = checkSquare(addressOfSubMat[checkerStruct->numOfNode]);
		if (result)
			incrementSharedResult();
	}
	pthread_mutex_lock(&allThreadsMutex);
	tasksFinished++;
	if (tasksFinished >= LAST_TASK_INDEX) {
		pthread_cond_signal(&condition);
		pthread_mutex_unlock(&allThreadsMutex);
	} else {
		pthread_mutex_unlock(&allThreadsMutex);
		free(checkerStruct);
		makeTask();
	}
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

void loadSubMatAddress() {

	const int OFFSET = 3;
	int x, y, k = 0;
	for (x = 0; x < SIZE_ROWS; x += OFFSET) {
		for (y = 0; y < SIZE_COLS; y += OFFSET) {
			addressOfSubMat[k++] = &soduku[x][y];
		}
	}
}

void incrementSharedResult() {
	pthread_mutex_lock(&sharedResultMutex);
	sharedResult++;
	pthread_mutex_unlock(&sharedResultMutex);
}

void checkRow(CheckerStruct* checkerStruct) {
	int res[SIZE_ROWS] = { 0 };
	int i;
	for (i = 0; i < SIZE_ROWS; i++) {
		int n = (soduku[checkerStruct->numOfNode][i]) - 1;
		if (res[n] == 0) {
			res[(soduku[checkerStruct->numOfNode][i]) - 1]++;
		} else {
			return;
		}
	}
	incrementSharedResult();
}

void checkCol(CheckerStruct* checkerStruct) {
	int res[SIZE_COLS] = { 0 };
	int i;
	for (i = 0; i < SIZE_COLS; i++) {
		if (res[soduku[i][checkerStruct->numOfNode] - 1] == 0) {
			res[soduku[i][checkerStruct->numOfNode] - 1]++;
		} else {
			return;
		}
	}
	incrementSharedResult();
}

int checkSquare(int* mat) {
	int i, j;
	const int OFFSET = 3;
	int res[SIZE_SUBMAT] = { 0 };
	for (i = 0; i < SIZE_ROWS / OFFSET; i++) {
		for (j = 0; j < SIZE_COLS / OFFSET; j++) {
			if (res[*(mat + (SIZE_ROWS * i + j)) - 1] == 0) {
				res[*(mat + (SIZE_ROWS * i + j)) - 1]++;
			} else {
				return 0;
			}
		}
	}
	return 1;
}
