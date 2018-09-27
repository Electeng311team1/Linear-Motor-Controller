/*
 * Linear_Motor_Controller.c
 *
 * Created: 26/09/2018 
 *	Author: Team 1
 */ 

#include "includes.h"
#define BAUD_RATE 9600
#define UBRR_VALUE F_CPU/16/BAUD_RATE-1
#define MAX_SIZE 30

volatile char received_message[MAX_SIZE];
volatile unsigned int i = 0;

volatile uint8_t new_byte = false; 
volatile uint8_t byte_received = 0;

//This is the ISR for UART receive
ISR(USART_RX_vect){
	while(!RECEIVE_COMPLETE);
	byte_received = UDR0;
	new_byte = true;
}

int main(void)
{
	uart_initiate(UBRR_VALUE);

	//Enable Global interrupts
	sei();

    while (1){
		if (new_byte == true){
			new_byte = false;
			uart_transmit_byte(byte_received);
		}
		
	}
}


