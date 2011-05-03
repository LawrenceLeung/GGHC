#include "jigbox.h"


// TODO non-Timer1 have slower clocks!

// TIM1 is running from SystemCoreClock.
//
// TIM2, TIM3, TIM4, and TIM12 are running from the APB1 clock.
// With SystemCoreClock at 72MHz, the APB1 prescaler output (the input to the TIMx prescalers)
// can be a maximum of 36MHz (24MHz on the Value Line devices).
// AHB prescaler = 1
// APB1 prescaler = 3
//  => APB1 PCLK1 = 24MHz
// x2
//  => TIMXCLK = 48MHz
//
// to have 10-bit PWM at the highest possible rate,
#define TIM1_FREQUENCY

#define TIM2_FREQUENCY
#define TIM3_FREQUENCY
#define TIM4_FREQUENCY
#define TIM12_FREQUENCY

static void GPIOSetup(void)
{
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Mode  = GPIO_Mode_AF_PP,
        .GPIO_Pin   = GPIO_Pin_2       // PA2/TIM2_CH3
                      |GPIO_Pin_3      // PA3/TIM2_CH4
                      |GPIO_Pin_8      // PA8/TIM1_CH1
                      |GPIO_Pin_9
    };                                 // PA9/TIM1_CH2
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 // PB0/TIM3_CH3
                                  |GPIO_Pin_1 // PB1/TIM3_CH4
                                  |GPIO_Pin_8 // PB8/TIM4_CH3
                                  |GPIO_Pin_9 // PB9/TIM4_CH4
                                  |GPIO_Pin_14 // PB14/TIM12_CH1
                                  |GPIO_Pin_15; // PB15/TIM12_CH2
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void EnableTimers(void)
{
    // Enable Timer1 clock and release reset
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);

    // TIM2,3,4,12 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2
                           |RCC_APB1Periph_TIM3
                           |RCC_APB1Periph_TIM4
                           |RCC_APB1Periph_TIM12, ENABLE);

    // and release reset
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2
                           |RCC_APB1Periph_TIM3
                           |RCC_APB1Periph_TIM4
                           |RCC_APB1Periph_TIM12, DISABLE);

    TIM_DeInit(TIM1); // Reset TIM1
    TIM_DeInit(TIM2); // Reset TIM2
    TIM_DeInit(TIM3); // Reset TIM3
    TIM_DeInit(TIM4); // Reset TIM4
    TIM_DeInit(TIM12); // Reset TIM12
}

// Time base configuration
static void InitTimerClocks(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    // TIM1: audio PWM timer
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure =
    {
        .TIM_Prescaler         = 0, // Max frequency
        .TIM_CounterMode       = TIM_CounterMode_Up,
        .TIM_Period            = 0x3FF, // 10 bit resolution
        .TIM_ClockDivision     = 0,
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // TIM2: sample timer
    TIM_TimeBaseStructure.TIM_Period = (RCC_Clocks.PCLK2_Frequency+sampFreq/2)/sampFreq;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // TIM3: LED PWM
    TIM_TimeBaseStructure.TIM_Period        = 1000; // 10ms
    TIM_TimeBaseStructure.TIM_Prescaler     = 720; // 10uS
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // TIM4: LED PWM
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // TIM12: LED PWM
    TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
}

static void InitTimerOutputCompare(void)
{
    // TIM1
    // Channel 1 is Audio PWM output
    TIM_OCInitTypeDef TIM_OCInitStructure =
    {
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable, // not using complementary outputs!
        .TIM_Pulse        = 0x200,
        .TIM_OCPolarity   = TIM_OCPolarity_Low,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };

    TIM_OC1Init(TIM1, &TIM_OCInitStructure); // TIM1_CH1 for PWM, TIM1_CH1N not used

    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; // LEDs are active-low
    TIM_OCInitStructure.TIM_Pulse      = 0;

    // TIM1 Channel2 is LED output
    TIM_OC2Init(TIM1, &TIM_OCInitStructure); // TIM1_CH2 for Blue1, TIM1_CH2N not used

    // TIM2
    // Channel 3 and 4 Configuration in PWM mode
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // TIM2_CH3 for Red2
    TIM_OC4Init(TIM2, &TIM_OCInitStructure); // TIM2_CH4 for Green1

    // TIM3
    // Channel 3 and 4 Configuration in PWM mode
    TIM_OC3Init(TIM3, &TIM_OCInitStructure); // TIM3_CH3 for Blue3
    TIM_OC4Init(TIM3, &TIM_OCInitStructure); // TIM3_CH4 for Blue2

    // TIM4
    TIM_OC3Init(TIM4, &TIM_OCInitStructure); // TIM4_CH3 for Red3
    TIM_OC4Init(TIM4, &TIM_OCInitStructure); // TIM4_CH4 for Green3

    // TIM12
    TIM_OC1Init(TIM12, &TIM_OCInitStructure); // TIM12_CH1 for Green2
    TIM_OC2Init(TIM12, &TIM_OCInitStructure); // TIM12_CH2 for Red1
}

// Init Sample Timer - Timer2
static void Init_AudioSampleTimer(void)
{
    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    // Clear update interrupt bit
    TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
    // Enable update interrupt
    TIM_ITConfig(TIM2, TIM_FLAG_Update, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // TIM2 enable counter
    TIM_Cmd(TIM2, ENABLE);
}

// Timer1 initialize
static void Init_AudioPWMTimer(void)
{
    // Double buffered
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    // TIM1 counter enable
    TIM_Cmd(TIM1, ENABLE);

    // TIM1 Main Output Enable
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

static void Init_Timer3(void)
{
    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // TIM3 enable counter
    TIM_Cmd(TIM3, ENABLE);
}

static void Init_Timer4(void)
{
    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM4, ENABLE);

    // TIM4 enable counter
    TIM_Cmd(TIM4, ENABLE);
}

static void Init_Timer12(void)
{
    // Disable double buffer of the APR register
    TIM_ARRPreloadConfig(TIM12, ENABLE);

    // TIM12 enable counter
    TIM_Cmd(TIM12, ENABLE);
}

void InitTimers(void)
{
    GPIOSetup();
    EnableTimers();
    InitTimerClocks();
    InitTimerOutputCompare();
    Init_AudioPWMTimer();              // TIM1
    Init_AudioSampleTimer();           // TIM2
    Init_Timer3();
    Init_Timer4();
    Init_Timer12();
}
