#include "jigbox.h"
#include "audio_class.h"

// TODO non-Timer1 have slower clocks!


// Init Sample Timer - Timer2
void Init_AudioSampleTimer(void)
{
    // TIM2 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2,DISABLE);
    // Reset TIM2
    TIM_DeInit(TIM2);
    // Time base configuration
    // Get different on chips' clocks.
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    SempPeriod                              = RCC_Clocks.PCLK2_Frequency/SampFreq;
    DeltaPer                                = (SempPeriod/(SampPerFrame*2)) + 1;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = SempPeriod / 1000;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0; // max resolution
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // GPIO setup
	GPIO_InitTypeDef GPIO_InitStructure = { 
	    .GPIO_Speed = GPIO_Speed_50MHz,
	    .GPIO_Mode = GPIO_Mode_AF_PP,
	    .GPIO_Pin = GPIO_Pin_2
    };
	GPIO_Init(GPIOA, &GPIO_InitStructure);      // PA2/TIM2_CH3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);      // PA3/TIM2_CH4

    // Channel 3 and 4 Configuration in PWM mode
    TIM_OCInitTypeDef TIM_OCInitStructure = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,   // not using complementary outputs!
        .TIM_Pulse        = 0,
        .TIM_OCPolarity   = TIM_OCPolarity_High,    // LEDs active low
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };

    TIM_OC3Init(TIM2, &TIM_OCInitStructure);    // TIM2_CH3 for Red2
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);    // TIM2_CH4 for Green1

    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    // Clear update interrupt bit
    TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
    // Enable update interrupt
    TIM_ITConfig(TIM2,TIM_FLAG_Update,ENABLE);

    // TIM2 enable counter
    TIM_Cmd(TIM2, ENABLE);
}

// Timer1 initialize
void Init_AudioPWMTimer(void)
{
    // Enable Timer1 clock and release reset
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1,DISABLE);

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Prescaler         = 0; // Max frequency
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 0x3FF; // 10 bit resolution
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // GPIO setup
	GPIO_InitTypeDef GPIO_InitStructure = { 
	    .GPIO_Speed = GPIO_Speed_50MHz,
	    .GPIO_Mode = GPIO_Mode_AF_PP,
	    .GPIO_Pin = GPIO_Pin_8
    };
	GPIO_Init(GPIOA, &GPIO_InitStructure);      // PA8/TIM1_CH1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);      // PA9/TIM1_CH2

    // Channel 1 Configuration in PWM mode
    TIM_OCInitTypeDef TIM_OCInitStructure = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,   // not using complementary outputs!
        .TIM_Pulse        = 0x200,
        .TIM_OCPolarity   = TIM_OCPolarity_Low,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);    // TIM1_CH1 for PWM, TIM1_CH1N not used

    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;  // LEDs are active-low
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);    // TIM1_CH2 for Blue1, TIM1_CH2N not used

    // Double buffered
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    // TIM1 counter enable
    TIM_Cmd(TIM1, ENABLE);

    // TIM1 Main Output Enable
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Init_Timer3(void)
{
    // TIM3 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // Reset TIM3
    TIM_DeInit(TIM3);

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Period        = 1000; // 10ms
    TIM_TimeBaseStructure.TIM_Prescaler     = 720; // 10uS
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // GPIO setup
	GPIO_InitTypeDef GPIO_InitStructure = { 
	    .GPIO_Speed = GPIO_Speed_50MHz,
	    .GPIO_Mode = GPIO_Mode_AF_PP,
	    .GPIO_Pin = GPIO_Pin_0
    };
	GPIO_Init(GPIOB, &GPIO_InitStructure);      // PB0/TIM3_CH3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);      // PB1/TIM3_CH4

    // Channel 3 and 4 Configuration in PWM mode
    TIM_OCInitTypeDef TIM_OCInitStructure = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,   // not using complementary outputs!
        .TIM_Pulse        = 0,
        .TIM_OCPolarity   = TIM_OCPolarity_High,    // LEDs active low
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);    // TIM3_CH3 for Blue3
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);    // TIM3_CH4 for Blue2

    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // TIM3 enable counter
    TIM_Cmd(TIM3, ENABLE);
}


void Init_Timer4(void)
{
    // TIM4 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // Reset TIM4
    TIM_DeInit(TIM4);

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Period        = 1000; // 10ms
    TIM_TimeBaseStructure.TIM_Prescaler     = 720; // 10uS
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // GPIO setup
	GPIO_InitTypeDef GPIO_InitStructure = { 
	    .GPIO_Speed = GPIO_Speed_50MHz,
	    .GPIO_Mode = GPIO_Mode_AF_PP,
	    .GPIO_Pin = GPIO_Pin_8
    };
	GPIO_Init(GPIOB, &GPIO_InitStructure);      // PB8/TIM4_CH3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);      // PB9/TIM4_CH4

    // Channel 3 and 4 Configuration in PWM mode
    TIM_OCInitTypeDef TIM_OCInitStructure = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,   // not using complementary outputs!
        .TIM_Pulse        = 0,
        .TIM_OCPolarity   = TIM_OCPolarity_High,    // LEDs active low
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };

    TIM_OC3Init(TIM4, &TIM_OCInitStructure);    // TIM4_CH3 for Red3
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);    // TIM4_CH4 for Green3

    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM4, ENABLE);

    // TIM4 enable counter
    TIM_Cmd(TIM4, ENABLE);
}

void Init_Timer12(void)
{
    // TIM12 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
    // Reset TIM12
    TIM_DeInit(TIM12);

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Period        = 1000; // 10ms
    TIM_TimeBaseStructure.TIM_Prescaler     = 720; // 10uS
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

    // GPIO setup
	GPIO_InitTypeDef GPIO_InitStructure = { 
	    .GPIO_Speed = GPIO_Speed_50MHz,
	    .GPIO_Mode = GPIO_Mode_AF_PP,
	    .GPIO_Pin = GPIO_Pin_14
    };
	GPIO_Init(GPIOB, &GPIO_InitStructure);      // PB14/TIM12_CH1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);      // PB15/TIM12_CH2

    // Channel 3 and 4 Configuration in PWM mode
    TIM_OCInitTypeDef TIM_OCInitStructure = {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,   // not using complementary outputs!
        .TIM_Pulse        = 0,
        .TIM_OCPolarity   = TIM_OCPolarity_High,    // LEDs active low
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };

    TIM_OC1Init(TIM12, &TIM_OCInitStructure);    // TIM12_CH1 for Green2
    TIM_OC2Init(TIM12, &TIM_OCInitStructure);    // TIM12_CH2 for Red1

    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM12, ENABLE);

    // TIM12 enable counter
    TIM_Cmd(TIM12, ENABLE);
}

void InitTimers(void)
{
    Init_AudioSampleTimer();
    Init_AudioPWMTimer();
    Init_Timer3();
    Init_Timer4();
    Init_Timer12();
}

