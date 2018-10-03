/*
 * UART.c
 *
 * Created: 26/09/2018 
 *  Author: Team 1
 */ 

#include "includes.h"
#define MAX_SIZE 50

//static char comparison_command[MAX_SIZE] = "{\"x\":{\"mfc\":{\"req\":\"xxx\"},\"clr\":\"xx\"}}"; 

//This function initializes UART receive and transmit 
void uart_initiate(uint16_t UBRR_VALUE){
	UCSR0A = 0x00;
	
	//Setting up UBRR value
	UBRR0L = UBRR_VALUE;

	//Enable transmitter
	UCSR0B |= (1 << TXEN0);

	//Enable receiver
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << RXCIE0);

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
	}
}

//This function processes a message received by UART
void process_message(char* message, int* mfc){
	//Format message like {xxx}
	char tmp[3];
	for(int i = 1; i < 4; i++){

		 tmp[i-1] = message[i];
	}
	mfc = (int*)tmp;
}