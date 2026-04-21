## 工程配置：

- **配置I²C1**：在 `I2C1` 配置页，将I2C模式选择为 **`I2C`**，并在下方 `Parameter Settings` 将 `I2C Speed Mode` 选择为 **`Fast Mode`**
- 将`OLED`库文件夹移至Drivers/BSP/文件夹

## 接线：

PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA

供电：5V

## 代码：

- **在main.c或AllHeader.h中添加include**：

  ```c
  #include "string.h"
  #include "oled.h"
  #include "stdio.h"
  ```

- **初始化OLED**：

  ```c
  OLED_Init(); // 初始化OLED
  ```
  
- **显示字符串:** (自带简单换行)

  - `uint8_t x` 纵坐标 [0, 127]

  - `uint8_t y` **纵向行数（页数）** [0, 7]

    > y在这里是页地址(Page Address)，范围是 `0` 到 `7`。整个128x64的屏幕被分成了8页，每页高8个像素。`y=0` 指的是第0-7行像素，以此类推。当使用16像素高的字体时（`Char_Size = 16`），函数会一次性占用 `y` 和 `y+1` 两页。

  - `const char *chr` 要显示的字符串

  - `uint8_t Char_Size` 字体大小，16或8

    ```c
    void OLED_ShowString(uint8_t x,uint8_t y,const char *chr,uint8_t Char_Size)
    ```

  - 示例：

    ```c
    OLED_ShowString(11, 0, "EDC-HUST-2025",8);
    OLED_ShowString(15, 2, "Vincent Wang",16);
    OLED_ShowString(15, 4, "QingHan Yang",16);
    OLED_ShowString(40, 6, "Xin Liu",16);
    ```

- **显示纯数字：**

  - `uint8_t x` 纵坐标 [0, 127]

  - `uint8_t y` 纵坐标  [0, 63]

  - `unsigned int num` 要显示的无符号整数

  - `uint8_t len`希望显示的位数（前面会用空格填充）

  - `uint8_t size2` 字体大小，16或8

    ```c
    void OLED_ShowNum(uint8_t x, uint8_t y, unsigned int num, uint8_t len, uint8_t size2);
    ```

- **清屏：**`OLED_Clear()`
- **清除屏幕某一行：**`OLED_Clearrow(uint8_t i)`
- **休眠模式：**`OLED_Display_Off()`
  - **用法**: 在不需要显示内容时调用，可以显著降低功耗。
- **休眠重亮：**`OLED_Display_On()`

**示例：**

```c
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//需要include的部分（可封装到AllHeader.h头文件中）
#include "string.h"
#include "oled.h"
#include "stdio.h"
/* USER CODE END Includes */

    /* USER CODE BEGIN 2 */
    //初始化OLED屏幕
    OLED_Init();
    OLED_Clear(); // 清屏
    /* USER CODE END 2 */

    while (1)
    {
        OLED_ShowString(11, 0, "EDC-HUST-2025",8);
        //OLED_ShowString(10, 0, "EDC-HUST-2025",16);
        OLED_ShowString(15, 2, "Vincent Wang",16);
        OLED_ShowString(15, 4, "QingHan Yang",16);
        OLED_ShowString(40, 6, "Xin Liu",16);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
```

