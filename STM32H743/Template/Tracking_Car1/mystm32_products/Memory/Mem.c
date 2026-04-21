#include "Mem.h"

Mem *mem[MAX_MEM_NUM];
int memCount = 0;

// Function to initialize a memory block with a given size and division factor
// and associate it with a UART for output
void MemInit(Mem *m, int size, int div, UART_HandleTypeDef *huart) // Initialize a memory block
{
	if (m == NULL)
	{
		return; // Invalid memory block or maximum number of blocks reached
	}
	if (size > MAX_MEM_SIZE)
	{
		size = MAX_MEM_SIZE; // Limit size to maximum defined size
		HAL_UART_Transmit(huart, (uint8_t *)"Size exceeds maximum limit, adjusting to MAX_MEM_SIZE.\n", 55, 1000);
	}
	if (size <= 0 || div <= 0)
	{
		return; // Invalid size or division factor
	}
	m->Data = (double *)malloc(size * sizeof(double));
	if (m->Data == NULL)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"Memory allocation failed.\n", 26, 1000);
		return; // Memory allocation failed
	}
	m->size = size;
	m->div = div;
	m->total = 0;
	m->huart = huart;
	m->isSet = 0; // Initialize isSet to 0 (not set)

	for (int i = 0; i < size; i++)
	{
		m->Data[i] = 0.0; // Initialize memory block with zeros
	}
}

void MemInitDefault(int size, int div, UART_HandleTypeDef *huart)
{
	if (memCount >= MAX_MEM_NUM)
	{
		return; // Maximum number of memory blocks reached
	}
	if (size > MAX_MEM_SIZE)
	{
		size = MAX_MEM_SIZE; // Limit size to maximum defined size
		HAL_UART_Transmit(huart, (uint8_t *)"Size exceeds maximum limit, adjusting to MAX_MEM_SIZE.\n", 55, 1000);
	}
	if (size <= 0 || div <= 0)
	{
		return; // Invalid size or division factor
	}
	mem[memCount] = (Mem *)malloc(sizeof(Mem));//Pay attention to you minimum heap setting. If it's too small, you'll get NULL
	if (mem[memCount] == NULL)
	{
		HAL_UART_Transmit(huart, (uint8_t *)"Memory allocation failed.\n", 26, 1000);
		return; // Memory allocation failed
	}
	Mem *m = mem[memCount];
	MemInit(m, size, div, huart); // Initialize the memory block
	memCount++;
	return;
}

//Functions below are used to set and get data in the memory blocks
void MemSet(Mem *m, double (*func)(void)) // Set the i-th memory block with data from a function
{
	if (m == NULL || m->Data == NULL)
	{
		return; // Invalid memory block
	}
	if (m->total >= m->size * m->div) 
	{
		return;
	}
	if (func == NULL)
	{
		return; // Invalid function pointer
	}
	if(m->total % m->div == 0)
	{
		m->Data[m->total / m->div] = func(); // Call the function to get data
	}
	m->isSet = 1; // Mark the memory block as set
	m->total++;
}

double MemGet(Mem *m, int i) // Get the i-th memory block data
{
	if (m == NULL || m->Data == NULL || i < 0 || i >= m->total / m->div)
	{
		return 0.0; // Invalid memory block or index
	}
	return m->Data[i];
}

/*
MemFunc WrapFunc(MemFuncSeq func, int i) 
{
	if (func == NULL)
	{
		return NULL; // Invalid function pointer
	}
	static double wrappedFunc(void) 
	{
		return func(index); // Call the function with the current index
	}
	return wrappedFunc; // Return the wrapped function
	// Note: The wrapped function captures the index variable, so it should be used carefully.
}

void MemSetSequence(int begin, int end, double (*func)(int), int bi) // Set the i-th memory block with data from a function with a sequence
{
	if (begin < 0 || end < begin || func == NULL)
	{
		return; // Invalid range or function pointer
	}
	for (int i = bi; i < end - begin + bi; i++)
	{
		if (i > memCount || i < 0)
		{
			return; // Invalid memory block index
		}

		MemSet(mem[i], WrapFunc(func, begin + i - bi)); // Set the memory block with data from the function
	}
}*/

void MemOut(Mem *m)// Output the i-th memory block to the UART, beginning from 0
{
	if (m == NULL || m->Data == NULL)
	{
		return; // Invalid memory block
	}
	char message[50];
	sprintf(message, "Memory Block :\n");
	HAL_UART_Transmit(m->huart, (uint8_t *)message, strlen(message), 1000);
	for (int j = 0; j < m->total / m->div; j++)
	{
		sprintf(message, "%lf\t", m->Data[j]);
		HAL_UART_Transmit(m->huart, (uint8_t *)message, strlen(message), 1000);
	}
}

void MemOutDefault()
{
	for (int i = 0; i < memCount; i++)
	{
		MemOut(mem[i]);
		HAL_UART_Transmit(mem[i]->huart, (uint8_t *)"\n", 1, 1000); // New line after each memory block
	}
}

//Function to destroy a memory block and free its resources
void destoryMem(Mem *m)
{
	if (m != NULL) {
		if (m->Data != NULL) {
			free(m->Data);
			m->Data = NULL;
		}
		free(m);
		m = NULL;
	}
}

void destoryMemDefault()
{
    for (int i = 0; i < memCount; i++)
	{
		destoryMem(mem[i]);
	}
	memCount = 0; // Reset the memory count
}
