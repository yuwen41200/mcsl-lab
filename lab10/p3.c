#include "libtmd.h"
#include "libadc.h"
#include <stdio.h>
#include <string.h>

extern float resistor;
char msg[128];
char buf[32];

void GPIO_Init(void) {
	// AHB2
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;
	// USART1_TX
	//TM_GPIO_Init(GPIOB, 6, TM_GPIO_Mode_AF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	// USART1_RX
	//TM_GPIO_Init(GPIOB, 7, TM_GPIO_Mode_AF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	GPIOA->MODER   &= 0b11111111110000111111111111111111;
	GPIOA->MODER   |= 0b00000000001010000000000000000000;
	GPIOA->PUPDR   &= 0b11111111110000111111111111111111;
	GPIOA->OSPEEDR &= 0b11111111110000111111111111111111;
	GPIOA->OTYPER  &= 0b11111111111111111111100111111111;
	// AF7
	GPIOA->AFR[1] = (GPIOA->AFR[1] & 0xFFFFF00F) | 0x00000770;
	// LD2
	//TM_GPIO_Init(GPIOC, 13, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Medium);
	GPIOA->MODER   &= 0b11111111111111111111001111111111;
	GPIOA->MODER   |= 0b00000000000000000000010000000000;
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

int UART_Receive(char *c) {
	while (!READ_BIT(USART1->ISR, USART_ISR_RXNE));
	*c = USART1->RDR;
	return *c;
}

int readline() {
	char c;
	int pos = 0;
	do {
		UART_Receive(&c);
		if (c == '\r') {
			UART_Transmit((uint8_t *) "\r\n", 2);
			c = '\0';
		}
		else {
			UART_Transmit((uint8_t *) &c, 1);
		}
		if (c == '\x7F') {
			pos = pos > 0 ? pos - 1 : 0;
		}
		else {
			msg[pos] = c;
			pos++;
		}
	} while (c != '\0');
	return pos;
}

int main(void) {
	fpu_enable();
	GPIO_Init();
	USART1_Init();
	ADC1_Init();
	SysTick_UserConfig(0.01f);
	while (1) {
		strcpy(msg, "> ");
		UART_Transmit((uint8_t *) msg, strlen(msg));
		readline();
		if (!strcmp(msg, ""));
		else if (!strcmp(msg, "showid")) {
			strcpy(msg, "0316323/0316213\r\n");
			UART_Transmit((uint8_t *) msg, strlen(msg));
		}
		else if (!strcmp(msg, "light")) {
			int pressed_q = 0;
			while (1) {
				while (READ_BIT(USART1->ISR, USART_ISR_RXNE)) {
					if (USART1->RDR == 'q') {
						strcpy(msg, "\r\n");
						UART_Transmit((uint8_t *) msg, strlen(msg));
						pressed_q = 1;
					}
				}
				if (pressed_q == 1)
					break;
				strcpy(msg, "\r                ");
				UART_Transmit((uint8_t *) msg, strlen(msg));
				sprintf(msg, "\r%f", resistor);
				UART_Transmit((uint8_t *) msg, strlen(msg));
				delay_ms(500);
			}
		}
		else if (!strcmp(msg, "led on")) {
			GPIOA->BSRR = 1 << 5;
			strcpy(msg, "Turn on LED\r\n");
			UART_Transmit((uint8_t *) msg, strlen(msg));
		}
		else if (!strcmp(msg, "led off")) {
			GPIOA->BRR = 1 << 5;
			strcpy(msg, "Turn off LED\r\n");
			UART_Transmit((uint8_t *) msg, strlen(msg));
		}
		else {
			strcpy(buf, "Unknown Command: [");
			UART_Transmit((uint8_t *) buf, strlen(buf));
			UART_Transmit((uint8_t *) msg, strlen(msg));
			strcpy(buf, "]\r\n");
			UART_Transmit((uint8_t *) buf, strlen(buf));
		}
	}
	return 0;
}
