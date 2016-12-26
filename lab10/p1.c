#include "libtmd.h"

void GPIO_Init(void) {
	// AHB2
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;
	// USART1_TX
	TM_GPIO_Init(GPIOB, 6, TM_GPIO_Mode_AF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	// USART1_RX
	TM_GPIO_Init(GPIOB, 7, TM_GPIO_Mode_AF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	// AF7
	GPIOB->AFR[0] = (GPIOB->AFR[0] & 0x00FFFFFF) | 0x77000000;
	// B1_USER
	TM_GPIO_Init(GPIOC, 13, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Medium);
}

void USART1_Init(void) {
	// APB2
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	// Word length, Parity selection, Parity control enable, Transmitter enable, Receiver enable, Oversampling mode
	MODIFY_REG(USART1->CR1, USART_CR1_M | USART_CR1_PS | USART_CR1_PCE | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8, USART_CR1_TE | USART_CR1_RE);
	// STOP bits, LIN mode enable, Clock enable
	MODIFY_REG(USART1->CR2, USART_CR2_STOP | USART_CR2_LINEN | USART_CR2_CLKEN, 0x0);
	// RTS enable, CTS enable, One sample bit method enable, Smartcard mode enable, Half-duplex selection, IrDA mode enable
	MODIFY_REG(USART1->CR3, USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT | USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN, 0x0);
	// USARTDIV
	MODIFY_REG(USART1->BRR, 0xFFFF, 4000000L/9600L);
	// USART enable
	USART1->CR1 |= USART_CR1_UE;
}

int UART_Transmit(uint8_t *arr, uint32_t size) {
	int sent = 0;
	for (int i = 0; i < size; ++i) {
		while (!READ_BIT(USART1->ISR, USART_ISR_TC));
		USART1->TDR = *arr;
		sent++;
		arr++;
	}
	while (!READ_BIT(USART1->ISR, USART_ISR_TC));
	return sent;
}

int main(void) {
	int prev_btn = 1, curr_btn = 1;
	GPIO_Init();
	USART1_Init();
	while (1) {
		if (!prev_btn && curr_btn)
			UART_Transmit((uint8_t *) "Hello World!", 12);
		prev_btn = curr_btn;
		curr_btn = GPIO_ReadInputDataBit(GPIOC, GPIO_PIN_13);
	}
	return 0;
}
