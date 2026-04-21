# Memory（Mem）数据存储模块

本模块为 STM32 平台下的通用数据存储与输出库，适用于编码器测速等场景下的数据采集、存储和串口输出。支持动态内存分配、分块采样、数据输出和资源释放等功能。

---

## 功能简介

- 支持动态分配内存块用于存储采样数据
- 可自定义采样间隔（div），灵活控制数据存储频率
- 支持通过 UART 串口输出采集到的数据
- 提供内存块的初始化、数据写入、读取和释放等接口
- 支持多块内存管理

---

## 主要函数说明

### 1. `void MemInit(Mem *m, int size, int div, UART_HandleTypeDef *huart);`

**功能**：  
初始化一个内存块，分配指定大小的空间，设置采样间隔，并指定用于输出的 UART 句柄。

**参数**：
- `m`：指向 Mem 结构体的指针
- `size`：存储数据的容量（最大 MAX_MEM_SIZE）
- `div`：采样间隔（每 div 次采样存一次数据）
- `huart`：用于数据输出的 UART 句柄

**示例代码**：
```c
Mem l;
MemInit(&l, 100, 1, &huart1);
```

---

### 2. `void MemSet(Mem *m, double (*func)(void));`

**功能**：  
调用用户提供的函数指针 `func` 采集数据，并按设定的采样间隔存入内存块。

**参数**：
- `m`：指向 Mem 结构体的指针
- `func`：无参、返回 double 的函数指针，作为采样源

**包装函数示例**（采集第0号编码器速度）：
```c
double gr(void)
{
    return getSpeed(0);
}
```

**典型用法**（结合定时器中断）：
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    UpdateAllSpeed(htim);    // 更新编码器速度
    MemSet(&l, gr);          // 采集并存储数据，gr为你自定义的采样函数
}
```

---

### 3. `void MemOut(Mem *m);`

**功能**：  
通过 UART 输出指定内存块中已采集的数据。

**参数**：
- `m`：指向 Mem 结构体的指针

**示例代码**：
```c
MemOut(&l);
```

---

### 4. `void destoryMem(Mem *m);`

**功能**：  
释放指定内存块及其数据，防止内存泄漏。

**参数**：
- `m`：指向 Mem 结构体的指针

**示例代码**：
```c
destoryMem(&l);
```

---

### 5. `double MemGet(Mem *m, int i);`

**功能**：  
从指定的内存块 `m` 中读取第 `i` 个已存储的数据，返回该数据的值。

**参数**：  
- `m`：指向 Mem 结构体的指针  
- `i`：要读取的数据索引（从 0 开始）

**示例代码**：  
```c
double value = MemGet(&l, 10); // 读取第11个采样点的数据
```

**注意事项**：  
- 索引 `i` 需小于初始化时设定的容量，否则可能返回无效值或引发错误。

---

## 典型使用流程示例

```c
Mem l;
MemInit(&l, 100, 1, &huart1); // 初始化内存块，容量100，采样间隔1，串口为huart1

double gr(void)
{
    return getSpeed(0); // 采集第0号编码器速度
}

// 在定时器中断回调中采集数据
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    UpdateAllSpeed(htim);    // 更新编码器速度等
    MemSet(&l, gr);          // 采集并存储数据
}

// 需要输出数据时
MemOut(&l);

// 用完后释放内存
destoryMem(&l);
```

---

## 注意事项

- 使用前需包含 `Mem.h` 并确保相关 UART、定时器等外设已初始化。
- `size` 和 `div` 参数需根据实际采样频率和内存大小合理设置，避免分配失败。
- `func` 需为无参、返回 double 的函数指针，通常用于采集传感器或编码器数据。
- 若分配内存失败（如堆空间不足），会有串口提示，请检查堆大小设置。
- 释放内存后指针不可再用，避免重复释放。

---

详细接口说明请参见源码注释或联系作者。