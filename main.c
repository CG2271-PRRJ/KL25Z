#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#include "MKL25Z4.h" // Device header
#include "init.h"
#include "motor_control.h"
#include "audio.h"
#include "stdbool.h"

osMessageQueueId_t msgBrain, msgMotorControl, msgBuzzer;

uint8_t rx_data = 112;
volatile uint8_t rx_data_old = 112;

volatile uint32_t prev_time = 0;

// static void delay(volatile uint32_t nof)
// {
// 	while (nof != 0)
// 	{
// 		__asm("NOP");
// 		nof--;
// 	}
// }

void delay(long time)
{
	long current_time = 0;
	while (current_time - time < 0)
	{
		current_time++;
	}
}

void UART1_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(UART1_IRQn);
	//__disable_irq();
	if (UART1->S1 & UART_S1_TDRE_MASK)
	{
		rx_data = UART1->D;
	}
	if (rx_data_old != rx_data)
	{
		osMessageQueuePut(msgBrain, &rx_data, NULL, 0);
		rx_data_old = rx_data;
	}

	PORTE->ISFR = 0xffffffff;
	//__enable_irq();
}

void tMotorControl(void *argument)
{
	uint8_t rx = 112;
	for (;;)
	{
		osMessageQueueGet(msgMotorControl, &rx, NULL, 0);
		if (rx == 112)
		{
			stop();
		}
		else if (rx < 225)
		{
			move(rx);
		}
	}
}

void tBuzzer(void *argument)
{
	uint8_t isAlt = 0;

	// 375000Hz/(50Hz) = MOD
	for (;;)
	{
		osMessageQueueGet(msgBuzzer, &isAlt, NULL, 0);
		if (!isAlt)
		{
			stopNote();
		}
		else if (isAlt)
		{
			changeNote(NOTE_F5);
		}
	}
}

void tBrain(void *argument)
{
	uint8_t rx = 112;
	uint8_t isStopped = 1;
	uint8_t isAlt = 0;
	for (;;)
	{
		osMessageQueueGet(msgBrain, &rx, NULL, osWaitForever);
		if (rx == 112)
		{
			isStopped = 1;
		}
		else if (rx < 225)
		{
			isStopped = 0;
		}
		else if (rx == 225)
		{
			isAlt = 0;
		}
		else if (rx == 226)
		{
			isAlt = 1;
		}

		osMessageQueuePut(msgMotorControl, &rx, NULL, osWaitForever);
		osMessageQueuePut(msgBuzzer, &isAlt, NULL, osWaitForever);
	}
}

int main(void)
{
	SystemCoreClockUpdate();
	initGPIO();
	initPWM();
	initUART1(BAUD_RATE);
	initBUZZER();
	initLED();

	osKernelInitialize();

	osThreadNew(tBrain, NULL, NULL);
	msgBrain = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(tMotorControl, NULL, NULL);
	msgMotorControl = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(tBuzzer, NULL, NULL);
	msgBuzzer = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osKernelStart();

	for (;;)
	{
	}
}
