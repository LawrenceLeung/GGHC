/*************************************************************************
 * SysTick interrupt handler. SysTick is the system clock in
 * this product. The base rate is 10ms (100 ticks per second). Various
 * other software subsystems can be triggered from this timer. Do not
 * directly call other software modules from this routine as this is an
 * interrupt so needs to be kept short. If another system should be
 * scheduled from here, set a global flag to trigger the other subsystem
 * then check the flag in the main loop.
 *
 *************************************************************************/
void SysTick_Handler(void)
{
    // Clear update interrupt bit
    TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
#ifdef TODO
    if(LedTimer-- == 0)
    {
        LedState  = !LedState;
        LedTimer  = LED_RATE; /* TODO This should be the metronome rate */
        LedUpdate = true;
    }
#endif
    // TODO ReadButtons();
    // playNextFrame = true;
}

/*************************************************************************
 * Function Name: Timer3IntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 1 interrupt handler. Timer 1 is the system clock in
 * this product. The base rate is 10ms (100 ticks per second). Various
 * other software subsystems can be triggered from this timer. Do not
 * directly call other software modules from this routine as this is an
 * interrupt so needs to be kept short. If another system should be
 * scheduled from here, set a global flag to trigger the other subsystem
 * then check the flag in the main loop.
 *
 *************************************************************************/
void Tim3Handler(void)
{
    // Clear update interrupt bit
    // TODO delete? TIM1_ClearITPendingBit(TIM1_FLAG_Update);
    TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
    // TODO  TIM3->SR &= (uint16_t)~TIM1_FLAG_Update;
    sysTime++;

    if(LedTimer-- == 0)
    {
        LedState  = !LedState;
        LedTimer  = LED_RATE; /* TODO This should be the metronome rate */
        LedUpdate = true;
    }

    ButtonUpdate = true;
}

/*************************************************************************
 * Function Name: Clk_Init
 * Parameters: uint32_t Frequency
 * Return: uint32_t
 *
 * Description: Init clock system
 *
 *************************************************************************/
void Clk_Init(void)
{
    // 1. Clocking the controller from internal HSI RC (8 MHz)
    RCC_HSICmd(ENABLE);
    // wait until the HSI is ready
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    // 2. Enable ext. high frequency OSC
    RCC_HSEConfig(RCC_HSE_ON);
    // wait until the HSE is ready
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
    {
    }
    // 3. Init PLL
#ifndef STM_DISCOVERY
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); // 72MHz
#endif
    RCC_PLLCmd(ENABLE);
    // wait until the PLL is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
#ifdef EMB_FLASH
    // 5. Init Embedded Flash
    // Zero wait state, if 0 < HCLK 24 MHz
    // One wait state, if 24 MHz < HCLK 56 MHz
    // Two wait states, if 56 MHz < HCLK 72 MHz
    // Flash wait state
    FLASH_SetLatency(FLASH_Latency_2);
    // Half cycle access
    FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
    // Prefetch buffer
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
#endif // EMB_FLASH
    // 5. Clock system from PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void InitTimers(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // Timer1 initialize
    // Enable Timer1 clock and release reset
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1,DISABLE);

    // Set timer period 0.01 seconds
    TIM1_TimeBaseInitStruct.TIM1_Prescaler         = 720; // 10us resolution
    TIM1_TimeBaseInitStruct.TIM1_CounterMode       = TIM1_CounterMode_Up;
    TIM1_TimeBaseInitStruct.TIM1_Period            = 1000; // 10ms
    TIM1_TimeBaseInitStruct.TIM1_ClockDivision     = TIM1_CKD_DIV1;
    TIM1_TimeBaseInitStruct.TIM1_RepetitionCounter = 0;
    TIM1_TimeBaseInit(&TIM1_TimeBaseInitStruct);

    // Clear update interrupt bit
    TIM1_ClearITPendingBit(TIM1_FLAG_Update);
    // Enable update interrupt
    TIM1_ITConfig(TIM1_FLAG_Update,ENABLE);

    // Enable timer counting
    TIM1_Cmd(ENABLE);

    // Init Sample Timer - Timer3
    // TIM3 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // Reset TIM3
    TIM_DeInit(TIM3);
    // Time base configuration

    TIM_TimeBaseStructure.TIM_Period        = 1000; // 10ms
    TIM_TimeBaseStructure.TIM_Prescaler     = 720; // 10uS
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    // Clear update interrupt bit
    TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
    // Enable update interrupt
    TIM_ITConfig(TIM3,TIM_FLAG_Update,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // TIM2 enable counter
    TIM_Cmd(TIM3, ENABLE);

    // Time base configuration
    TIM_TimeBaseStructure.TIM_Prescaler         = 0; // Max frequency
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 0x3FF; // 10 bit resolution
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // Channel 1 Configuration in PWM mode
    TIM1_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM1_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
    TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM1_OCInitStructure.TIM_Pulse        = 0x200;
    TIM1_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;
    TIM1_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
    TIM1_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;
    TIM1_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
    // Double buffered
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    // TIM1 counter enable
    TIM_Cmd(TIM1, ENABLE);

    // TIM1 Main Output Enable
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    //#endif

    // Init Sample Timer - Timer2
    // TIM2 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // Reset TIM2
    TIM_DeInit(TIM2);
    // Time base configuration
    SempPeriod                              = RCC_Clocks.PCLK2_Frequency/SampFreq;
    DeltaPer                                = (SempPeriod/(SampPerFrame*2)) + 1;
    TIM_TimeBaseStructure.TIM_Period        = SempPeriod / 100;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0; // max resolution
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    // Clear update interrupt bit
    TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
    // Enable update interrupt
    TIM_ITConfig(TIM2,TIM_FLAG_Update,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USB_INTR_AUDIO_SAMP_TIMER_PRIORITY; // max priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    // TIM2 enable counter
    TIM_Cmd(TIM2, ENABLE);
}

void Init_SysTick(void)
{
}

void Init_AudioSampleTimer(void)
{
}

void Init_AudioPWMTimer(void)
{
}
