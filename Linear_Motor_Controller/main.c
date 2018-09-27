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
volatile unsigned int message_pos = 0;

volatile uint8_t new_byte = false; 
volatile uint8_t byte_received = 0;

//This is the ISR for UART receive
ISR(USART_RX_vect){
 	while(!RECEIVE_COMPLETE);
	received_message[message_pos] = UDR0;
	message_pos++;
}

int main(void)
{
	uart_initiate(UBRR_VALUE);

	//Enable Global interrupts
	sei();
    while (1){
// 		if (new_byte == true){
// 			new_byte = false;
// 			uart_transmit_byte(byte_received);
// 		}
		
		//Trying to distinguish between the message sent from usb and message sent from LLC
		//Better way of clearing array or using message_pos required
		if(received_message[message_pos] == '}'){
			cli();
			uart_transmit("Message sent from LLC {");
			uart_transmit_byte(13);
			uart_transmit_byte(10);
			uart_transmit((char*) received_message);
			message_pos = 0;
			received_message[0] = '\0';
			uart_transmit_byte(13);
			uart_transmit_byte(10);
			uart_transmit("} Message sent from LLC");
			uart_transmit_byte(13);
			uart_transmit_byte(10);
			sei();
		}
	}
}


