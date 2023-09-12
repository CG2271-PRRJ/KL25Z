#include "MKL25Z4.h"

#define MASK(x) (1 << (x))

// LED control
#define GREEN_LED_01 1  // PortA Pin 1
#define GREEN_LED_02 2  // PortA Pin 2
#define GREEN_LED_03 4  // PortD Pin 4
#define GREEN_LED_04 12 // PortA Pin 12
#define GREEN_LED_05 4  // PortA Pin 4
#define GREEN_LED_06 5  // PortA Pin 5
#define GREEN_LED_07 8  // PortC Pin 8
#define GREEN_LED_08 9  // PortC Pin 9
#define GREEN_LED_09 7  // PortC Pin 7
#define GREEN_LED_10 0  // PortC Pin 0


#define RED_LED_01  13  // PortA Pin 13


// UART control
#define BAUD_RATE 9600
#define UART_TX_PORTE0 0 //PTE0 UART1TX
#define UART_RX_PORTE1 1 //PTE1 UART1RX
#define UART1_INT_PRIO 128 //UART PRIORITY


// MOTOR control
#define LEFT_FWD 20   // TPM1_CH0   PTE_20
#define LEFT_BCK 21   // TPM1_CH1   PTE_21
#define RIGHT_FWD 22  // TPM2_CH0   PTE_22
#define RIGHT_BACK 23 // TPM2_CH1   PTE_23
#define MODVALUE 3750


// Buzzer Control
#define BUZZER_PIN 5 // TPM0_CH5   PTD5


// Declare funcs
void initUART1(uint32_t baud);
void initGPIO(void);
void initPWM(void);
//void initTimer();
//void initLED();
