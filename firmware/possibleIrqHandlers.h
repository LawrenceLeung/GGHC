// this is just to document the possible names of the interrupts
#error "Do not include this file!"

typedef void (*irqHandler)(void);

extern irqHandler
 ADC1_2_IRQHandler,		/* ADC1 and ADC2 */
 ADC1_IRQHandler,		/* ADC1 */
 ADC3_IRQHandler,		/* ADC3 */
 BusFault_Handler,		/* Bus Fault Handler */
 CAN1_RX0_IRQHandler,		/* CAN1 RX0 */
 CAN1_RX1_IRQHandler,		/* CAN1 RX1 */
 CAN1_SCE_IRQHandler,		/* CAN1 SCE */
 CAN1_TX_IRQHandler,		/* CAN1 TX */
 CAN2_RX0_IRQHandler,		/* CAN2 RX0 */
 CAN2_RX1_IRQHandler,		/* CAN2 RX1 */
 CAN2_SCE_IRQHandler,		/* CAN2 SCE */
 CAN2_TX_IRQHandler,		/* CAN2 TX */
 CEC_IRQHandler,		/* HDMI-CEC */
 DMA1_Channel1_IRQHandler,		/* DMA1 Channel 1 */
 DMA1_Channel2_IRQHandler,		/* DMA1 Channel 2 */
 DMA1_Channel3_IRQHandler,		/* DMA1 Channel 3 */
 DMA1_Channel4_IRQHandler,		/* DMA1 Channel 4 */
 DMA1_Channel5_IRQHandler,		/* DMA1 Channel 5 */
 DMA1_Channel6_IRQHandler,		/* DMA1 Channel 6 */
 DMA1_Channel7_IRQHandler,		/* DMA1 Channel 7 */
 DMA2_Channel1_IRQHandler,		/* DMA2 Channel1 */
 DMA2_Channel2_IRQHandler,		/* DMA2 Channel2 */
 DMA2_Channel3_IRQHandler,		/* DMA2 Channel3 */
 DMA2_Channel4_5_IRQHandler,		/* DMA2 Channel4 & Channel5 */
 DMA2_Channel4_IRQHandler,		/* DMA2 Channel4 */
 DMA2_Channel5_IRQHandler,		/* DMA2 Channel5 */
 DebugMon_Handler,		/* Debug Monitor Handler */
 ETH_IRQHandler,		/* Ethernet */
 ETH_WKUP_IRQHandler,		/* Ethernet Wakeup through EXTI line */
 EXTI0_IRQHandler,		/* EXTI Line 0 */
 EXTI15_10_IRQHandler,		/* EXTI Line 15..10 */
 EXTI1_IRQHandler,		/* EXTI Line 1 */
 EXTI2_IRQHandler,		/* EXTI Line 2 */
 EXTI3_IRQHandler,		/* EXTI Line 3 */
 EXTI4_IRQHandler,		/* EXTI Line 4 */
 EXTI9_5_IRQHandler,		/* EXTI Line 9..5 */
 FLASH_IRQHandler,		/* Flash */
 FSMC_IRQHandler,		/* FSMC */
 HardFault_Handler,		/* Hard Fault Handler */
 I2C1_ER_IRQHandler,		/* I2C1 Error */
 I2C1_EV_IRQHandler,		/* I2C1 Event */
 I2C2_ER_IRQHandler,		/* I2C1 Error */
 I2C2_ER_IRQHandler,		/* I2C2 Error */
 I2C2_EV_IRQHandler,		/* I2C2 Event */
 MemManage_Handler,		/* MPU Fault Handler */
 NMI_Handler,		/* NMI Handler */
 OTG_FS_IRQHandler,		/* USB OTG FS */
 OTG_FS_WKUP_IRQHandler,		/* USB OTG FS Wakeup through EXTI line */
 PVD_IRQHandler,		/* PVD through EXTI Line detect */
 PendSV_Handler,		/* PendSV Handler */
 RCC_IRQHandler,		/* RCC */
 RTCAlarm_IRQHandler,		/* RTC Alarm through EXTI Line */
 RTCAlarm_IRQHandler,		/* RTC alarm through EXTI line */
 RTC_IRQHandler,		/* RTC */
 Reset_Handler,		/* Reset Handler */
 SDIO_IRQHandler,		/* SDIO */
 SPI1_IRQHandler,		/* SPI1 */
 SPI2_IRQHandler,		/* SPI2 */
 SPI3_IRQHandler,		/* SPI3 */
 SVC_Handler,		/* SVCall Handler */
 SysTick_Handler,		/* SysTick Handler */
 TAMPER_IRQHandler,		/* Tamper */
 TIM12_IRQHandler,		/* TIM12 */
 TIM13_IRQHandler,		/* TIM13 */
 TIM14_IRQHandler,		/* TIM14 */
 TIM1_BRK_IRQHandler,		/* TIM1 Break */
 TIM1_BRK_TIM15_IRQHandler,		/* TIM1 Break and TIM15 */
 TIM1_BRK_TIM9_IRQHandler,		/* TIM1 Break and TIM9 */
 TIM1_CC_IRQHandler,		/* TIM1 Capture Compare */
 TIM1_TRG_COM_IRQHandler,		/* TIM1 Trigger and Commutation */
 TIM1_TRG_COM_TIM11_IRQHandler,		/* TIM1 Trigger and Commutation and TIM11 */
 TIM1_TRG_COM_TIM17_IRQHandler,		/* TIM1 Trigger and Commutation and TIM17 */
 TIM1_UP_IRQHandler,		/* TIM1 Update */
 TIM1_UP_TIM10_IRQHandler,		/* TIM1 Update and TIM10 */
 TIM1_UP_TIM16_IRQHandler,		/* TIM1 Update and TIM16 */
 TIM2_IRQHandler,		/* TIM2 */
 TIM3_IRQHandler,		/* TIM3 */
 TIM4_IRQHandler,		/* TIM4 */
 TIM5_IRQHandler,		/* TIM5 */
 TIM6_DAC_IRQHandler,		/* TIM6 and DAC underrun */
 TIM6_IRQHandler,		/* TIM6 */
 TIM7_IRQHandler,		/* TIM7 */
 TIM8_BRK_IRQHandler,		/* TIM8 Break */
 TIM8_BRK_TIM12_IRQHandler,		/* TIM8 Break and TIM12 */
 TIM8_CC_IRQHandler,		/* TIM8 Capture Compare */
 TIM8_TRG_COM_IRQHandler,		/* TIM8 Trigger and Commutation */
 TIM8_TRG_COM_TIM14_IRQHandler,		/* TIM8 Trigger and Commutation and TIM14 */
 TIM8_UP_IRQHandler,		/* TIM8 Update */
 TIM8_UP_TIM13_IRQHandler,		/* TIM8 Update and TIM13 */
 UART4_IRQHandler,		/* UART4 */
 UART5_IRQHandler,		/* UART5 */
 USART1_IRQHandler,		/* USART1 */
 USART2_IRQHandler,		/* USART2 */
 USART3_IRQHandler,		/* USART3 */
 USBWakeUp_IRQHandler,		/* USB Wakeup from suspend */
 USB_HP_CAN1_TX_IRQHandler,		/* USB High Priority or CAN1 TX */
 USB_LP_CAN1_RX0_IRQHandler,		/* USB Low Priority or CAN1 RX0 */
 UsageFault_Handler,		/* Usage Fault Handler */
 WWDG_IRQHandler /* Window Watchdog */
;
#endif
