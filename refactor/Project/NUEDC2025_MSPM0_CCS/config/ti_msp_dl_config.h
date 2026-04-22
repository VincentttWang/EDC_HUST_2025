/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for Motor */
#define Motor_INST                                                         TIMG7
#define Motor_INST_IRQHandler                                   TIMG7_IRQHandler
#define Motor_INST_INT_IRQN                                     (TIMG7_INT_IRQn)
#define Motor_INST_CLK_FREQ                                              4000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_C0_PORT                                                 GPIOA
#define GPIO_Motor_C0_PIN                                         DL_GPIO_PIN_17
#define GPIO_Motor_C0_IOMUX                                      (IOMUX_PINCM39)
#define GPIO_Motor_C0_IOMUX_FUNC                     IOMUX_PINCM39_PF_TIMG7_CCP0
#define GPIO_Motor_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_C1_PORT                                                 GPIOA
#define GPIO_Motor_C1_PIN                                         DL_GPIO_PIN_18
#define GPIO_Motor_C1_IOMUX                                      (IOMUX_PINCM40)
#define GPIO_Motor_C1_IOMUX_FUNC                     IOMUX_PINCM40_PF_TIMG7_CCP1
#define GPIO_Motor_C1_IDX                                    DL_TIMER_CC_1_INDEX

/* Defines for SMotor_1 */
#define SMotor_1_INST                                                      TIMG6
#define SMotor_1_INST_IRQHandler                                TIMG6_IRQHandler
#define SMotor_1_INST_INT_IRQN                                  (TIMG6_INT_IRQn)
#define SMotor_1_INST_CLK_FREQ                                             62500
/* GPIO defines for channel 0 */
#define GPIO_SMotor_1_C0_PORT                                              GPIOA
#define GPIO_SMotor_1_C0_PIN                                      DL_GPIO_PIN_29
#define GPIO_SMotor_1_C0_IOMUX                                    (IOMUX_PINCM4)
#define GPIO_SMotor_1_C0_IOMUX_FUNC                   IOMUX_PINCM4_PF_TIMG6_CCP0
#define GPIO_SMotor_1_C0_IDX                                 DL_TIMER_CC_0_INDEX

/* Defines for SMotor_2 */
#define SMotor_2_INST                                                      TIMG8
#define SMotor_2_INST_IRQHandler                                TIMG8_IRQHandler
#define SMotor_2_INST_INT_IRQN                                  (TIMG8_INT_IRQn)
#define SMotor_2_INST_CLK_FREQ                                             62500
/* GPIO defines for channel 1 */
#define GPIO_SMotor_2_C1_PORT                                              GPIOA
#define GPIO_SMotor_2_C1_PIN                                      DL_GPIO_PIN_30
#define GPIO_SMotor_2_C1_IOMUX                                    (IOMUX_PINCM5)
#define GPIO_SMotor_2_C1_IOMUX_FUNC                   IOMUX_PINCM5_PF_TIMG8_CCP1
#define GPIO_SMotor_2_C1_IDX                                 DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMA0)
#define TIMER_0_INST_IRQHandler                                 TIMA0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                          (3999U)



/* Defines for Debug */
#define Debug_INST                                                         UART0
#define Debug_INST_FREQUENCY                                            32000000
#define Debug_INST_IRQHandler                                   UART0_IRQHandler
#define Debug_INST_INT_IRQN                                       UART0_INT_IRQn
#define GPIO_Debug_RX_PORT                                                 GPIOB
#define GPIO_Debug_TX_PORT                                                 GPIOB
#define GPIO_Debug_RX_PIN                                          DL_GPIO_PIN_1
#define GPIO_Debug_TX_PIN                                          DL_GPIO_PIN_0
#define GPIO_Debug_IOMUX_RX                                      (IOMUX_PINCM13)
#define GPIO_Debug_IOMUX_TX                                      (IOMUX_PINCM12)
#define GPIO_Debug_IOMUX_RX_FUNC                       IOMUX_PINCM13_PF_UART0_RX
#define GPIO_Debug_IOMUX_TX_FUNC                       IOMUX_PINCM12_PF_UART0_TX
#define Debug_BAUD_RATE                                                   (9600)
#define Debug_IBRD_32_MHZ_9600_BAUD                                        (208)
#define Debug_FBRD_32_MHZ_9600_BAUD                                         (21)
/* Defines for K230 */
#define K230_INST                                                          UART2
#define K230_INST_FREQUENCY                                             32000000
#define K230_INST_IRQHandler                                    UART2_IRQHandler
#define K230_INST_INT_IRQN                                        UART2_INT_IRQn
#define GPIO_K230_RX_PORT                                                  GPIOA
#define GPIO_K230_TX_PORT                                                  GPIOA
#define GPIO_K230_RX_PIN                                          DL_GPIO_PIN_24
#define GPIO_K230_TX_PIN                                          DL_GPIO_PIN_23
#define GPIO_K230_IOMUX_RX                                       (IOMUX_PINCM54)
#define GPIO_K230_IOMUX_TX                                       (IOMUX_PINCM53)
#define GPIO_K230_IOMUX_RX_FUNC                        IOMUX_PINCM54_PF_UART2_RX
#define GPIO_K230_IOMUX_TX_FUNC                        IOMUX_PINCM53_PF_UART2_TX
#define K230_BAUD_RATE                                                  (115200)
#define K230_IBRD_32_MHZ_115200_BAUD                                        (17)
#define K230_FBRD_32_MHZ_115200_BAUD                                        (23)





/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for LED0: GPIOB.22 with pinCMx 50 on package pin 21 */
#define LED_LED0_PIN                                            (DL_GPIO_PIN_22)
#define LED_LED0_IOMUX                                           (IOMUX_PINCM50)
/* Port definition for Pin Group Key */
#define Key_PORT                                                         (GPIOB)

/* Defines for PIN_1: GPIOB.21 with pinCMx 49 on package pin 20 */
#define Key_PIN_1_PIN                                           (DL_GPIO_PIN_21)
#define Key_PIN_1_IOMUX                                          (IOMUX_PINCM49)
/* Port definition for Pin Group SMotor_IO */
#define SMotor_IO_PORT                                                   (GPIOB)

/* Defines for DIR1: GPIOB.14 with pinCMx 31 on package pin 2 */
#define SMotor_IO_DIR1_PIN                                      (DL_GPIO_PIN_14)
#define SMotor_IO_DIR1_IOMUX                                     (IOMUX_PINCM31)
/* Defines for DIR2: GPIOB.15 with pinCMx 32 on package pin 3 */
#define SMotor_IO_DIR2_PIN                                      (DL_GPIO_PIN_15)
#define SMotor_IO_DIR2_IOMUX                                     (IOMUX_PINCM32)
/* Defines for EN1: GPIOB.16 with pinCMx 33 on package pin 4 */
#define SMotor_IO_EN1_PIN                                       (DL_GPIO_PIN_16)
#define SMotor_IO_EN1_IOMUX                                      (IOMUX_PINCM33)
/* Defines for EN2: GPIOB.2 with pinCMx 15 on package pin 50 */
#define SMotor_IO_EN2_PIN                                        (DL_GPIO_PIN_2)
#define SMotor_IO_EN2_IOMUX                                      (IOMUX_PINCM15)
/* Defines for AIN1: GPIOA.16 with pinCMx 38 on package pin 9 */
#define Motor_IO_AIN1_PORT                                               (GPIOA)
#define Motor_IO_AIN1_PIN                                       (DL_GPIO_PIN_16)
#define Motor_IO_AIN1_IOMUX                                      (IOMUX_PINCM38)
/* Defines for AIN2: GPIOB.17 with pinCMx 43 on package pin 14 */
#define Motor_IO_AIN2_PORT                                               (GPIOB)
#define Motor_IO_AIN2_PIN                                       (DL_GPIO_PIN_17)
#define Motor_IO_AIN2_IOMUX                                      (IOMUX_PINCM43)
/* Defines for BIN1: GPIOB.18 with pinCMx 44 on package pin 15 */
#define Motor_IO_BIN1_PORT                                               (GPIOB)
#define Motor_IO_BIN1_PIN                                       (DL_GPIO_PIN_18)
#define Motor_IO_BIN1_IOMUX                                      (IOMUX_PINCM44)
/* Defines for BIN2: GPIOB.19 with pinCMx 45 on package pin 16 */
#define Motor_IO_BIN2_PORT                                               (GPIOB)
#define Motor_IO_BIN2_PIN                                       (DL_GPIO_PIN_19)
#define Motor_IO_BIN2_IOMUX                                      (IOMUX_PINCM45)
/* Defines for E1A: GPIOA.2 with pinCMx 7 on package pin 42 */
#define Motor_IO_E1A_PORT                                                (GPIOA)
// pins affected by this interrupt request:["E1A","E2A"]
#define Motor_IO_INT_IRQN                                       (GPIOA_INT_IRQn)
#define Motor_IO_INT_IIDX                       (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define Motor_IO_E1A_IIDX                                    (DL_GPIO_IIDX_DIO2)
#define Motor_IO_E1A_PIN                                         (DL_GPIO_PIN_2)
#define Motor_IO_E1A_IOMUX                                        (IOMUX_PINCM7)
/* Defines for E2A: GPIOA.21 with pinCMx 46 on package pin 17 */
#define Motor_IO_E2A_PORT                                                (GPIOA)
#define Motor_IO_E2A_IIDX                                   (DL_GPIO_IIDX_DIO21)
#define Motor_IO_E2A_PIN                                        (DL_GPIO_PIN_21)
#define Motor_IO_E2A_IOMUX                                       (IOMUX_PINCM46)
/* Defines for Tracking_1: GPIOB.6 with pinCMx 23 on package pin 58 */
#define Tracking_Tracking_1_PORT                                         (GPIOB)
#define Tracking_Tracking_1_PIN                                  (DL_GPIO_PIN_6)
#define Tracking_Tracking_1_IOMUX                                (IOMUX_PINCM23)
/* Defines for Tracking_2: GPIOB.7 with pinCMx 24 on package pin 59 */
#define Tracking_Tracking_2_PORT                                         (GPIOB)
#define Tracking_Tracking_2_PIN                                  (DL_GPIO_PIN_7)
#define Tracking_Tracking_2_IOMUX                                (IOMUX_PINCM24)
/* Defines for Tracking_3: GPIOB.8 with pinCMx 25 on package pin 60 */
#define Tracking_Tracking_3_PORT                                         (GPIOB)
#define Tracking_Tracking_3_PIN                                  (DL_GPIO_PIN_8)
#define Tracking_Tracking_3_IOMUX                                (IOMUX_PINCM25)
/* Defines for Tracking_4: GPIOB.9 with pinCMx 26 on package pin 61 */
#define Tracking_Tracking_4_PORT                                         (GPIOB)
#define Tracking_Tracking_4_PIN                                  (DL_GPIO_PIN_9)
#define Tracking_Tracking_4_IOMUX                                (IOMUX_PINCM26)
/* Defines for Tracking_5: GPIOA.12 with pinCMx 34 on package pin 5 */
#define Tracking_Tracking_5_PORT                                         (GPIOA)
#define Tracking_Tracking_5_PIN                                 (DL_GPIO_PIN_12)
#define Tracking_Tracking_5_IOMUX                                (IOMUX_PINCM34)
/* Defines for Tracking_6: GPIOB.26 with pinCMx 57 on package pin 28 */
#define Tracking_Tracking_6_PORT                                         (GPIOB)
#define Tracking_Tracking_6_PIN                                 (DL_GPIO_PIN_26)
#define Tracking_Tracking_6_IOMUX                                (IOMUX_PINCM57)
/* Defines for Tracking_7: GPIOB.23 with pinCMx 51 on package pin 22 */
#define Tracking_Tracking_7_PORT                                         (GPIOB)
#define Tracking_Tracking_7_PIN                                 (DL_GPIO_PIN_23)
#define Tracking_Tracking_7_IOMUX                                (IOMUX_PINCM51)
/* Defines for Tracking_8: GPIOA.13 with pinCMx 35 on package pin 6 */
#define Tracking_Tracking_8_PORT                                         (GPIOA)
#define Tracking_Tracking_8_PIN                                 (DL_GPIO_PIN_13)
#define Tracking_Tracking_8_IOMUX                                (IOMUX_PINCM35)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOA)

/* Defines for SDA: GPIOA.10 with pinCMx 21 on package pin 56 */
#define OLED_SDA_PIN                                            (DL_GPIO_PIN_10)
#define OLED_SDA_IOMUX                                           (IOMUX_PINCM21)
/* Defines for SCL: GPIOA.11 with pinCMx 22 on package pin 57 */
#define OLED_SCL_PIN                                            (DL_GPIO_PIN_11)
#define OLED_SCL_IOMUX                                           (IOMUX_PINCM22)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_Motor_init(void);
void SYSCFG_DL_SMotor_1_init(void);
void SYSCFG_DL_SMotor_2_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_Debug_init(void);
void SYSCFG_DL_K230_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
