#include "motor_control.h"

void stop()
{
	TPM1_C0V = 0;
	TPM1_C1V = 0;
	TPM2_C0V = 0;
	TPM2_C1V = 0;
}

uint16_t getSpeed(uint8_t speed)
{
	switch (speed)
	{
	case 0:
	case 14:
		return SPEED7;
	case 1:
	case 13:
		return SPEED6;
	case 2:
	case 12:
		return SPEED5;
	case 3:
	case 11:
		return SPEED4;
	case 4:
	case 10:
		return SPEED3;
	case 5:
	case 9:
		return SPEED2;
	case 6:
	case 8:
		return SPEED1;
	case 7:
		return 0;
	default:
		return 0;
	}
}

void changeLeft(uint8_t speed)
{
	if (speed > 7)
	{
		TPM2_C0V = 0;
		TPM2_C1V = getSpeed(speed);
	}
	else
	{
		TPM2_C0V = getSpeed(speed);
		TPM2_C1V = 0;
	}
}

void changeRight(uint8_t speed)
{
	if (speed > 7)
	{
		TPM1_C0V = 0;
		TPM1_C1V = getSpeed(speed);
	}
	else
	{
		TPM1_C0V = getSpeed(speed);
		TPM1_C1V = 0;
	}
}

void move(uint8_t speed)
{
	uint8_t leftspeed = speed / 15;
	uint8_t rightspeed = speed % 15;
	changeLeft(leftspeed);
	changeRight(rightspeed);
}
