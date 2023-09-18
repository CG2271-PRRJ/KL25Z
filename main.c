#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#include "MKL25Z4.h" // Device header
#include "init.h"
#include "motor_control.h"
#include "audio.h"
#include "led_control.h"
#include "stdbool.h"

osMessageQueueId_t msgBrain, msgMotorControl, msgBuzzer, msgGreenLED, msgRedLED;

// osMutexId_t redMutex;

uint8_t rx_data = 112;
volatile uint8_t rx_data_old = 112;

void UART1_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(UART1_IRQn);

	if (UART1->S1 & UART_S1_RDRF_MASK)
	{
		rx_data = UART1->D;
	}

	if (rx_data_old != rx_data)
	{
		osMessageQueuePut(msgBrain, &rx_data, NULL, 0);
		rx_data_old = rx_data;
	}

	PORTE->ISFR = 0xffffffff;
}

void tRedLED(void *argument)
{
	uint8_t isStopped = 1;
	uint32_t prevTime = 0;
	uint16_t delay = 250;
	for (;;)
	{
		osMessageQueueGet(msgRedLED, &isStopped, NULL, 0);

		if (isStopped)
		{
			delay = 250;
		}
		else
		{
			delay = 500;
		}
		if (osKernelGetTickCount() - prevTime > delay)
		{
			blinkRedLED(); // 250ms
			prevTime = osKernelGetTickCount();
		}
	}
}

void tGreenLED(void *argument)
{
	uint8_t isStopped = 1;
	uint32_t prevTime = 0;
	uint16_t delay = 0;
	int i = 0;
	for (;;)
	{
		osMessageQueueGet(msgGreenLED, &isStopped, NULL, 0);
		if (isStopped)
		{
			delay = 0;
		}
		else
		{
			delay = 100;
		}

		if (osKernelGetTickCount() - prevTime > delay)
		{
			if (isStopped)
			{
				setGreenLED(true);
			}
			else
			{
				i = i > 9 ? 0 : i + 1;
				cycleGreenLED(i);
			}
			prevTime = osKernelGetTickCount();
		}
	}
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
	uint32_t prevTime = 0;
	uint16_t delay = 10;
	bool isplaying = false;

	// 375000Hz/(50Hz) = MOD
	for (;;)
	{
		osMessageQueueGet(msgBuzzer, &isAlt, NULL, 0);
		if (osKernelGetTickCount() - prevTime > delay)
		{
			if (!isplaying)
			{
				if (!isAlt)
				{
					stopNote();
				}
				else if (isAlt)
				{
					delay = changeNoteMain();
				}
				isplaying = true;
			}
			else
			{
				isplaying = false;
				delay = 10;
				stopNote();
			}

			prevTime = osKernelGetTickCount();
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
		osMessageQueuePut(msgGreenLED, &isStopped, NULL, osWaitForever);
		osMessageQueuePut(msgRedLED, &isStopped, NULL, osWaitForever);
	}
}

int main(void)
{
	SystemCoreClockUpdate();
	// disable interrupts here
	//__disable_irq();

	initGPIO();
	initLED();
	initPWM();
	initBUZZER();
	initUART1(BAUD_RATE);

	osKernelInitialize();

	// osMutexNew(redMutex);

	osThreadNew(tBrain, NULL, NULL);
	msgBrain = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(tRedLED, NULL, NULL);
	msgRedLED = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(tGreenLED, NULL, NULL);
	msgGreenLED = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(tMotorControl, NULL, NULL);
	msgMotorControl = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(tBuzzer, NULL, NULL);
	msgBuzzer = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	//__enable_irq();

	osKernelStart();

	for (;;)
	{
	}
}
