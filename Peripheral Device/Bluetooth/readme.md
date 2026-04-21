# STM32H7 蓝牙模块 UART DMA 空闲线接收使用指南

本文档旨在为在 STM32H7 微控制器上，使用 HAL 库实现蓝牙模块通过 USART3 进行 DMA 空闲线接收提供详细的配置和代码指南，并特别强调高速 MCU（如 H7）中缓存一致性（Cache Coherency）的关键注意事项。

## 1. 硬件连接

* **USART3 连接蓝牙模块**：
    * STM32H7 的 USART3 TX 引脚连接蓝牙模块的 RX 引脚。
    * STM32H7 的 USART3 RX 引脚连接蓝牙模块的 TX 引脚。
    * 确保电源和地线连接正确。

## 2. CubeMX 工程配置

在 STM32CubeMX 中，对您的项目进行如下配置：

### 2.1 USART3 外设配置

* **Connectivity (连接性) -> USART3**：
    * **Mode (模式)**：Asynchronous (异步模式)。
    * **Configuration (配置) -> Parameter Settings (参数设置)**：
        * **Baud Rate (波特率)**：设置 `9600`。
        * **Word Length (字长)**：通常为 `8 Bits (+ Parity if any)`。
        * **Stop Bits (停止位)**：通常为 `1`。
        * **Data Direction (数据方向)**：**Receive and Transmit (接收和发送)**。
        * **Fifo Mode (FIFO 模式)**：推荐 **Disable (禁用)**，`ReceiveToIdle` 即使禁用 FIFO 也能正常工作。如果启用，需要额外配置 RxFifo Threshold。

### 2.2 DMA 配置

* **System Core (系统核心) -> DMA -> Add (添加)**：
    * 为 USART3 RX 添加一个 DMA 请求：选择 `USART3_RX`。
    * **DMA Request Settings (DMA 请求设置)**：
        * **Mode (模式)**：**Normal (普通模式)**。
        * **Increment Address (地址递增)**：Peripheral Disabled (外设禁用)，Memory Enabled (内存启用)。
        * **Data Width (数据宽度)**：Peripheral Byte (外设字节)，Memory Byte (内存字节)。
        * 其他选项保持默认或根据需要配置（如 Burst Size，通常不需要）。
* **确保没有其他外设共用此 DMA Stream**，以避免冲突。

### 2.3 NVIC 中断配置

* **System Core (系统核心) -> NVIC (嵌套向量中断控制器)**：
    * 勾选并**启用 (Enabled)**：
        * `USART3 global interrupt`。
        * `USART3_RX` 所关联的 DMA Stream 的全局中断（例如：`DMA1 Stream X global interrupt` 或 `BDMA Channel Y global interrupt`）。

### 2.4 MPU (Memory Protection Unit) 配置（可选但推荐）

如果您启用了数据缓存（D-Cache），强烈建议配置 MPU，将 **DMA 缓冲区**以及 **HAL 库的外设句柄**（如 `huart3` 和 `hdma_usart3_rx`）所在的内存区域设置为**不可缓存 (Non-cacheable)**。这能有效避免缓存一致性问题。

* **具体 MPU 配置取决于您的内存映射**，通常需要手动将相关内存区域（例如 `SRAM1` 或 `SRAM2` 中用于 DMA 的部分）配置为 `IsCacheable = MPU_ACCESS_NOT_CACHEABLE`。

## 3. 代码实现

### 3.1 变量声明与内存属性

在 `main.c` 或相关头文件中声明接收缓冲区，并利用 GNU C 扩展的 `__attribute__((section(".sram_no_cache")))` 将其放置在不可缓存的 SRAM 区域。同样，修改 `usart.c` 和 `dma.c` 中 HAL 库句柄的声明。

* **`main.c`**：
    ```c
    /* Private variables ---------------------------------------------------------*/
    /* USER CODE BEGIN PV */
    char receive[50] __attribute__((section(".sram_no_cache"))); // 接收缓冲区，放置在不可缓存区域
    int isin = 0; // 用于调试，指示是否进入回调
    /* USER CODE END PV */
    ```
* **`dma.c`**：
    找到 `DMA_HandleTypeDef hdma_usart3_rx;`，修改为：
    ```c
    DMA_HandleTypeDef hdma_usart3_rx __attribute__((section(".sram_no_cache"))); // DMA 句柄，放置在不可缓存区域
    ```
    **注意**：`.sram_no_cache` 是一个示例段名，实际应查阅您的链接器脚本（`.ld` 文件）以确定适用于不可缓存 RAM 的正确段名（例如 `.RamD1NoCacheable` 或 `.dtcm_ram`）。如果该段不存在，您可能需要在链接器脚本中手动定义。

### 3.2 `main` 函数中的初始化

在 `main()` 函数中，MCU 初始化完成后，启动 UART 的 DMA 空闲线接收。

```c
/* USER CODE BEGIN 2 */
char message[] = "Hello, Bluetooth!\r\n"; // 用于发送的测试消息

// IMPORTANT: 由于 receive 缓冲区已声明为不可缓存，DMA 将直接读写主内存，
// CPU 访问时也将直接读写主内存，不再需要 SCB_InvalidateDCache_by_Addr 或 SCB_CleanDCache_by_Addr
// 来处理 receive 缓冲区的数据一致性。
// 因此，以下注释掉的行无需保留：
// SCB_InvalidateDCache_by_Addr((uint32_t *)receive, sizeof(receive));

HAL_StatusTypeDef rx