#ifndef MEM_H
#define MEM_H

#include "main.h"
#include "usart.h"
#include "tim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEM_NUM 10
// Maximum number of memory blocks, adjust as needed.
#define MAX_MEM_SIZE 1000 
// Maximum memory size, adjust as needed. 
//But when you reset it, please reset the minimum heap size in the STM32CubeMX to a larger value, 
//otherwise you will get NULL pointer.

typedef struct{
	double *Data;
	int size;
	int div;
	int total;
	short isSet;
	//isSet is used to indicate whether the memory block has been initialized with data.
	//If isSet is 0, it means the memory block has not been initialized with data.
	UART_HandleTypeDef *huart;
}Mem;

typedef float (*MemFunc)(void);
typedef float (*MemFuncSeq)(int);

#ifdef __cplusplus
extern "C" {
#endif

void MemInit(Mem *m, int size, int div, UART_HandleTypeDef *huart);
void MemInitDefault(int size, int div, UART_HandleTypeDef *huart);
void MemSet(Mem *m, double (*func)(void));
double MemGet(Mem *m, int i);
MemFunc WrapFunc(MemFuncSeq func, int i);
void MemSetSequence(int begin, int end, double (*func)(int), int bi);
void MemOut(Mem *m);
void MemOutDefault(void);
void destoryMem(Mem *m);
void destoryMemDefault(void);

// ...existing code...
#endif // MEM_H
