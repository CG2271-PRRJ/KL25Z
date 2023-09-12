#include "led_control.h"

void blinkRedLED(void)
{
    PTA->PTOR |= MASK(RED_LED_01);
}

void setGreenLED(bool turnOn)
{
    if (turnOn)
    {
        PTA->PSOR |= MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08);
        PTD->PSOR |= MASK(GREEN_LED_05);
        PTC->PSOR |= MASK(GREEN_LED_09) | MASK(GREEN_LED_10) | MASK(GREEN_LED_02) | MASK(GREEN_LED_01);
    }
    else
    {
        PTA->PCOR |= MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08);
        PTD->PCOR |= MASK(GREEN_LED_05);
        PTC->PCOR |= MASK(GREEN_LED_09) | MASK(GREEN_LED_10) | MASK(GREEN_LED_02) | MASK(GREEN_LED_01);
    }
}

void cycleGreenLED(int led_pos)
{
    setGreenLED(false);
    switch (led_pos)
    {
    case 0:
        PTC->PSOR |= MASK(GREEN_LED_01);
        break;
    case 1:
        PTC->PSOR |= MASK(GREEN_LED_02);
        break;
    case 2:
        PTA->PSOR |= MASK(GREEN_LED_03);
        break;
    case 3:
        PTA->PSOR |= MASK(GREEN_LED_04);
        break;
    case 4:
        PTD->PSOR |= MASK(GREEN_LED_05);
        break;
    case 5:
        PTA->PSOR |= MASK(GREEN_LED_06);
        break;
    case 6:
        PTA->PSOR |= MASK(GREEN_LED_07);
        break;
    case 7:
        PTA->PSOR |= MASK(GREEN_LED_08);
        break;
    case 8:
        PTC->PSOR |= MASK(GREEN_LED_09);
        break;
    case 9:
        PTC->PSOR |= MASK(GREEN_LED_10);
        break;
    default:
        break;
    }
}
