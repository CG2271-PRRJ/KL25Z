#include "MKL25Z4.h"
#include "cmsis_os2.h"
#include "init.h"
#include "stdbool.h"

void blinkRedLED(void);
void setGreenLED(bool turnOn);
void cycleGreenLED(int led_pos);
