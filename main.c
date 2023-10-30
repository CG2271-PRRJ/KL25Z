#include "RTE_Components.h"
#include CMSIS_device_header
// #include "cmsis_os2.h"

// #include "MKL25Z4.h" // Device header
#include "init.h"
#include "motor_control.h"
#include "audio.h"
#include "led_control.h"
#include "stdbool.h"

osMessageQueueId_t msgBrain, msgMotorControl, msgBuzzer, msgGreenLED, msgRedLED;
osSemaphoreId_t brainSem;
osSemaphoreId_t motorSem;

// osMutexId_t redMutex;

const osThreadAttr_t priorityMax = {
	.priority = osPriorityRealtime};

const osThreadAttr_t priorityHigh = {
	.priority = osPriorityHigh};

uint8_t rx_data = 112;

void UART1_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(UART1_IRQn);

	if (UART1->S1 & UART_S1_RDRF_MASK)
	{
		rx_data = UART1->D;
	}

	osMessageQueuePut(msgBrain, &rx_data, NULL, 0);
	osSemaphoreRelease(brainSem);

	PORTE->ISFR = 0xffffffff;
}

void tRedLED(void *argument)
{
	uint8_t isStopped = 1;
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
		blinkRedLED(); // 250ms
		osDelay(delay);
	}
}

void tGreenLED(void *argument)
{
	uint8_t isStopped = 1;
	int i = 0;
	for (;;)
	{
		osMessageQueueGet(msgGreenLED, &isStopped, NULL, 0);

		if (isStopped)
		{
			setGreenLED(true);
		}
		else
		{
			i = i > 9 ? 0 : i + 1;
			cycleGreenLED(i);
		}
		osDelay(100);
	}
}

void tMotorControl(void *argument)
{
	uint8_t rx = 112;
	for (;;)
	{
		osSemaphoreAcquire(motorSem, osWaitForever);
		osMessageQueueGet(msgMotorControl, &rx, NULL, 0); // maybe need os wait forever here?
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
	bool isplaying = false;

	// 375000Hz/(50Hz) = MOD
	for (;;)
	{
		osMessageQueueGet(msgBuzzer, &isAlt, NULL, 0);
		if (!isplaying)
		{
			if (isAlt == 0)
			{
				stopNote();
			}
			else if (isAlt == 1)
			{
				changeNoteMain();
			}
			else if (isAlt == 2)
			{
				changeNoteAlt();
			}
			isplaying = true;
		}
		else
		{
			isplaying = false;
			osDelay(10);
			stopNote();
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
		osSemaphoreAcquire(brainSem, osWaitForever);
		osMessageQueueGet(msgBrain, &rx, NULL, 0);
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
		else if (rx == 227)
		{
			isAlt = 2;
		}

		osMessageQueuePut(msgMotorControl, &rx, NULL, 0);
		osSemaphoreRelease(motorSem);
		osMessageQueuePut(msgBuzzer, &isAlt, NULL, 0);
		osMessageQueuePut(msgGreenLED, &isStopped, NULL, 0);
		osMessageQueuePut(msgRedLED, &isStopped, NULL, 0);
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
	brainSem = osSemaphoreNew(1, 0, NULL);
	motorSem = osSemaphoreNew(1, 0, NULL);

	// osMutexNew(redMutex);

	osThreadNew(tBrain, NULL, &priorityMax);
	msgBrain = osMessageQueueNew(2, sizeof(uint8_t), NULL);

	osThreadNew(tMotorControl, NULL, &priorityHigh);
	msgMotorControl = osMessageQueueNew(2, sizeof(uint8_t), NULL);

	osThreadNew(tRedLED, NULL, NULL);
	msgRedLED = osMessageQueueNew(2, sizeof(uint8_t), NULL);

	osThreadNew(tGreenLED, NULL, NULL);
	msgGreenLED = osMessageQueueNew(2, sizeof(uint8_t), NULL);

	osThreadNew(tBuzzer, NULL, NULL);
	msgBuzzer = osMessageQueueNew(2, sizeof(uint8_t), NULL);

	//__enable_irq();

	osKernelStart();

	for (;;)
	{
	}
}
