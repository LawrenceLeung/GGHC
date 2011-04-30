#include "LED.h"

typedef struct
{
    TIM_TypeDef *timer;
    uint8_t outputChannel;
} LED_Definition;

static LED_Definition Single_LED_map[N_LEDS] =
{
    { TIM12, 1 },                      // Red1 = TIM12_CH1
    { TIM2, 4 },                       // Green1 = TIM2_CH4
    { TIM1, 2 },                       // Blue1 = TIM1_CH2
    { TIM2, 3 },                        // Red2 = TIM2_CH3
    { TIM12, 1 },                      // Green2 = TIM12_CH1
    { TIM3, 4 },                       // Blue2 = TIM3_CH4
    { TIM4, 3 },                       // Red3 = TIM4_CH3
    { TIM4, 4 },                       // Green3 = TIM4_CH4
    { TIM3, 3 },                       // Blue3 = TIM3_CH3
};

#define SCALE_BRIGHTNESS(howmuch) (howmuch)

/* Initialize all 9 external LEDs */
void LEDInit()
{
	int i;
    for (i = 0; i < N_LEDS; i++)
    {
        Single_LED_On((Single_LED_t) i, 0);
    }
}

bool Single_LED_On(Single_LED_t led, LED_Brightness howmuch)
{
    if (led < N_LEDS)
    {
        LED_Definition def = Single_LED_map[(int)led];
        switch (def.outputChannel)
        {
            case 1:
                TIM_SetCompare1(def.timer, SCALE_BRIGHTNESS(howmuch));
                break;

            case 2:
                TIM_SetCompare2(def.timer, SCALE_BRIGHTNESS(howmuch));
                break;

            case 3:
                TIM_SetCompare3(def.timer, SCALE_BRIGHTNESS(howmuch));
                break;

            case 4:
                TIM_SetCompare4(def.timer, SCALE_BRIGHTNESS(howmuch));
                break;

            default:
                return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool RGB_LED_On(RGB_LED_t led, LED_Brightness red, LED_Brightness green, LED_Brightness blue)
{
    switch (led)
    {
        case RGB_LED_1:
            Single_LED_On(LED_1_RED, red);
            Single_LED_On(LED_1_GREEN, green);
            Single_LED_On(LED_1_BLUE, blue);
            break;

        case RGB_LED_2:
            Single_LED_On(LED_2_RED, red);
            Single_LED_On(LED_2_GREEN, green);
            Single_LED_On(LED_2_BLUE, blue);
            break;

        case RGB_LED_3:
            Single_LED_On(LED_3_RED, red);
            Single_LED_On(LED_3_GREEN, green);
            Single_LED_On(LED_3_BLUE, blue);
            break;

        default:
            return false;
    }

    return true;
}
