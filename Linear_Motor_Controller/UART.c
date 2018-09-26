/*
 * UART.c
 *
 * Created: 26/09/2018 
 *  Author: Team 1
 */ 

#include "includes.h"
#define F_CPU 8000000
#include <util/delay.h>
#define MAX_SIZE 30

volatile char received_message[MAX_SIZE];
volatile unsigned int i = 0;

//Checking to see if the transmission is complete
#define UDR_FULL !(UCSR0A & (1 << UDRE0))

//Check if transmission has been received
#define RECEIVE_COMPLETE (UCSR0A & (1 << RXC0))

//This is the ISR for UART receive
ISR(USART_RX_vect){
	while(!RECEIVE_COMPLETE);
	received_message[i] = UDR0;
	i++;
}

//This function initializes UART receive and transmit 
void uart_initiate(uint16_t UBRR_VALUE){
	//Setting up UBRR value
	UBRR0L = UBRR_VALUE;

	//Enable transmitter
	UCSR0B |= (1 << TXEN0);

	//Enable receiver
	UCSR0B |= (1 << RXEN0);

	//Setting the character size to 8 bits
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
}

//This function transmit a message via UART
void uart_transmit(char* message){
	unsigned int i = 0;
	while(message[i] != '\0'){
		while(UDR_FULL);
		UDR0 = message[i];
		i++;
		_delay_ms(10);
	}
}

