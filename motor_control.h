#include "MKL25Z4.h"

#define SPEED7 3750
#define SPEED6 3200
#define SPEED5 2600
#define SPEED4 2000
#define SPEED3 1500
#define SPEED2 1000
#define SPEED1 600

void stop(void);
void move(uint8_t speed);
void changeLeft(uint8_t speed);
void changeRight(uint8_t speed);
uint16_t getSpeed(uint8_t speed);
