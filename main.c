#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"



#include "MKL25Z4.h"                    // Device header
#include "init.h"
#include "motor_control.h"

osMessageQueueId_t msgBrain, msgMotorControl;

uint8_t rx_data = 112;
volatile uint8_t rx_data_old = 112;
volatile int counter = 0;

volatile uint32_t prev_time = 0;

void UART1_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(UART1_IRQn);
	//__disable_irq();
	if (UART1->S1 & UART_S1_TDRE_MASK)
	{
		rx_data = UART1->D;
	}
	//counter++;
	if (rx_data_old != rx_data) {
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
		if (rx == 112) {
			stop();
		} else if (rx < 225) {
			move(rx);
		}
	}
}


void tBrain(void *argument)
{
	for (;;)
	{
		osMessageQueueGet(msgBrain, &rx_data, NULL, osWaitForever);
		osMessageQueuePut(msgMotorControl, &rx_data, NULL, osWaitForever);
	}
}



int main(void)
{
	SystemCoreClockUpdate();
	initGPIO();
	initPWM();
	initUART1(BAUD_RATE);
	
	osKernelInitialize();
	
	osThreadNew(tBrain, NULL, NULL);
	msgBrain = osMessageQueueNew(1, sizeof(uint8_t), NULL);
	
	osThreadNew(tMotorControl, NULL, NULL);
	msgMotorControl = osMessageQueueNew(1, sizeof(uint8_t), NULL);
	
	
	osKernelStart();
	
	
	for (;;)
	{
	}
}
