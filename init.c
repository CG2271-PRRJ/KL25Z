#include "init.h"
void initGPIO(void)
{
	// Enable Clock to PORTA, PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTE_MASK));
}

void initUART1(uint32_t baud_rate)
{
	uint32_t divisor, bus_clock;
	SIM->SOPT5 &= ~(SIM_SOPT5_UART1TXSRC_MASK | SIM_SOPT5_UART1RXSRC_MASK); // changed this
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;

	// setup PTE0 tx
	PORTE->PCR[UART_TX_PORTE0] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX_PORTE0] |= PORT_PCR_MUX(3);

	// setup PTE1 rx
	PORTE->PCR[UART_RX_PORTE1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE1] |= PORT_PCR_MUX(3);

	// turn off transmit and recieve
	UART1->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));

	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
	divisor = bus_clock / (baud_rate * 16);
	UART1->BDH = UART_BDH_SBR(divisor >> 8);
	UART1->BDL = UART_BDL_SBR(divisor);

	UART1->C1 = 0;
	UART1->S2 = 0;
	UART1->C3 = 0;
	
	// enable receieve for serial
	UART1->C2 |= (UART_C2_RE_MASK);

	// config for interrupt
	// turn off transmit and recieve interrupt
	UART1->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK); // changed this
	NVIC_SetPriority(UART1_IRQn, 1);
	NVIC_ClearPendingIRQ(UART1_IRQn);
	NVIC_EnableIRQ(UART1_IRQn);
	
	// turn on recieve interrupt
	UART1->C2 |= (UART_C2_RIE_MASK);
}

void initPWM()
{
	// Configure Mode 3 for PWM pin operation
	PORTE->PCR[RIGHT_BACKW] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RIGHT_BACKW] |= PORT_PCR_MUX(3);
	PORTE->PCR[RIGHT_FORW] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RIGHT_FORW] |= PORT_PCR_MUX(3);

	PORTE->PCR[LEFT_BACKW] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[LEFT_BACKW] |= PORT_PCR_MUX(3);
	PORTE->PCR[LEFT_FORW] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[LEFT_FORW] |= PORT_PCR_MUX(3);

	// Enable clock gating for Timer1 and Timer2
	SIM->SCGC6 = (SIM_SCGC6_TPM1_MASK) | (SIM_SCGC6_TPM2_MASK);

	// Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // MCGFLLCLK OR MCGPLLCLK/2

	// set modulo value 48000000/128 = 375000, 375000Hz/(50Hz * 2) = 3750 pwm phase correct
	TPM1->MOD = MODVALUE;
	TPM2->MOD = MODVALUE;

	// Set Phase Correct PWM mode
	TPM1->SC |= TPM_SC_CPWMS_MASK;
	TPM2->SC |= TPM_SC_CPWMS_MASK;

	// Set prescaler and counting mode
	TPM1->SC &= ~((TPM_SC_PS_MASK) | (TPM_SC_CMOD_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

	TPM2->SC &= ~((TPM_SC_PS_MASK) | (TPM_SC_CMOD_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

	// enable PWM on the TPM and channel
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM1_C0V = 0;
	TPM1_C1V = 0;
	TPM2_C0V = 0;
	TPM2_C1V = 0;
}

void initLED(void)
{
	// mux switch all led pins
	PORTA->PCR[GREEN_LED_03] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_03] |= PORT_PCR_MUX(1);
	PORTA->PCR[GREEN_LED_04] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_04] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_LED_05] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_LED_05] |= PORT_PCR_MUX(1);
	PORTA->PCR[GREEN_LED_06] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_06] |= PORT_PCR_MUX(1);
	PORTA->PCR[GREEN_LED_07] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_07] |= PORT_PCR_MUX(1);
	PORTA->PCR[GREEN_LED_08] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_08] |= PORT_PCR_MUX(1);

	PORTC->PCR[GREEN_LED_09] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_09] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_10] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_10] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_02] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_02] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_LED_01] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_01] |= PORT_PCR_MUX(1);

	PORTA->PCR[RED_LED_01] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RED_LED_01] |= PORT_PCR_MUX(1);

	// set data direction registers to output
	PTA->PDDR |= (MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08) | MASK(RED_LED_01));

	PTC->PDDR |= (MASK(GREEN_LED_09) | MASK(GREEN_LED_10) | MASK(GREEN_LED_02) | MASK(GREEN_LED_01));

	PTD->PDDR |= (MASK(GREEN_LED_05) | MASK(0));
}

void initBUZZER(void)
{
	// Configure Mode 3 for PWM pin operation
	PORTD->PCR[BUZZER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BUZZER_PIN] |= PORT_PCR_MUX(4);

	// Enable clock gating for Timer0
	SIM->SCGC6 |= (SIM_SCGC6_TPM0_MASK);

	// Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // MCGFLLCLK OR MCGPLLCLK/2

	// set modulo value 48000000/128 = 375000, 375000Hz/(50Hz) = 7500 Edge aligned
	TPM0->MOD = 3500;

	// Set Edge aligned PWM mode
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);

	// Set prescaler and counting mode
	TPM0->SC &= ~((TPM_SC_PS_MASK) | (TPM_SC_CMOD_MASK));
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

	// enable PWM on the TPM and channel
	TPM0_C5SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C5SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	// off buzzer first
	TPM0_C5V = 0;
}
