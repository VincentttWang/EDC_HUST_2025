#include "LaserUsart.h"

static uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};
static uint8_t new_package[USART_LASER_RX_BUF_LEN];
static uint8_t g_new_package_flag = 0;

uint16_t Laser_Loc[10];
uint16_t Rect_Loc[10];

CanMV_Error Laser_error = CANMV_ERR_INIT;
CanMV_Error Rect_error  = CANMV_ERR_INIT;

static int Laser_Find_None_Init = 1;
static int Rect_Find_None_Init  = 1;

void Laser_USART_Init(void)
{
    NVIC_ClearPendingIRQ(LASER_INI_IRQn);
    NVIC_EnableIRQ(LASER_INI_IRQn);
}

void Laser_SendChar(char ch)
{
    while (DL_UART_isBusy(LASER_UART) == true)
        ;
    DL_UART_Main_transmitData(LASER_UART, ch);
}

void Laser_SendString(char *str)
{
    while (*str != 0 && str != 0)
        Laser_SendChar(*str++);
}

static uint8_t Get_High_Val(uint16_t value)
{
    return (value >> 8) & 0xFF;
}

static uint8_t Get_Low_Val(uint16_t value)
{
    return value & 0xFF;
}

static uint16_t Combine_Bytes(uint8_t high, uint8_t low)
{
    return ((uint16_t)high << 8) | low;
}

static void Deal_Rx(uint8_t rxtemp)
{
    static uint8_t g_start = 0;
    static uint8_t rx_count = 0;

    if (rxtemp == 0x12) {
        g_start = 1;
        USART_LASER_RX_BUF[rx_count++] = rxtemp;
    } else {
        if (g_start == 0) {
            if (Laser_error == CANMV_ERR_NOT_FOUND)
                return;
            Laser_error = CANMV_ERR_FRAME_DROP;
            Rect_error  = CANMV_ERR_FRAME_DROP;
            return;
        }

        USART_LASER_RX_BUF[rx_count++] = rxtemp;

        if (rxtemp == 0x5B) {
            g_start = 0;
            rx_count = 0;
            memcpy(new_package, USART_LASER_RX_BUF, USART_LASER_RX_BUF_LEN);
            g_new_package_flag = 1;
            memset(USART_LASER_RX_BUF, 0, USART_LASER_RX_BUF_LEN);
            Laser_error = CANMV_ERR_NONE;
            Rect_error  = CANMV_ERR_NONE;
        }

        if (rx_count >= USART_LASER_RX_BUF_LEN) {
            if (Laser_error != CANMV_ERR_NOT_FOUND) {
                Laser_error = CANMV_ERR_FRAME_DROP;
                Rect_error  = CANMV_ERR_FRAME_DROP;
            }
            g_start = 0;
            rx_count = 0;
            memset(USART_LASER_RX_BUF, 0, USART_LASER_RX_BUF_LEN);
        }
    }
}

static void Get_CanMV_Loc(int begin_index, int data_num, uint16_t Loc[])
{
    for (int i = 0; i < data_num; i += 2)
        Loc[i / 2] = Combine_Bytes(new_package[i + begin_index], new_package[i + begin_index + 1]);
}

void Laser_GetLoc(uint16_t Loc[])
{
    Get_CanMV_Loc(Laser_Begin, Laser_RX_Num, Loc);
}

void Rect_GetLoc(uint16_t Loc[])
{
    Get_CanMV_Loc(Rect_Begin, Rect_RX_Num, Loc);
}

static bool Array_Empty(uint16_t arr[], int len)
{
    for (int i = 0; i < len; i++) {
        if (arr[i] != 0)
            return false;
    }
    return true;
}

static void Update_Error(uint16_t arr[], int len, CanMV_Error *error, int *find_none_init)
{
    if (*error == CANMV_ERR_FRAME_DROP)
        return;

    if (Array_Empty(arr, len)) {
        *error = (*find_none_init == 0) ? CANMV_ERR_LOST : CANMV_ERR_NOT_FOUND;
    } else {
        *error = CANMV_ERR_NONE;
        *find_none_init = 0;
    }
}

void CanMV_Process(void)
{
    char uart_data = DL_UART_Main_receiveData(LASER_UART);
    Deal_Rx(uart_data);

    if (g_new_package_flag == 1) {
        g_new_package_flag = 0;
        Laser_GetLoc(Laser_Loc);
        Rect_GetLoc(Rect_Loc);
    }

    Update_Error(Laser_Loc, Laser_RX_Num / 2 / 2, &Laser_error, &Laser_Find_None_Init);
    Update_Error(Rect_Loc, Rect_RX_Num / 2, &Rect_error, &Rect_Find_None_Init);
}
