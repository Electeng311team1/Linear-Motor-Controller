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
volatile unsigned int message_index = 0;
volatile unsigned int net_brackets = 0;
volatile uint8_t message_complete = false;

//This is the ISR for UART receive
ISR(USART_RX_vect){
 	while(!RECEIVE_COMPLETE);
	char tmp = UDR0; 
	if(tmp == '{'){
		net_brackets++;
	}
	else if(tmp == '}'){
		net_brackets--;
		if(net_brackets == 0){
			message_complete = true;
		}
	}

	if(net_brackets > 0){
		received_message[message_index] = tmp;
		message_index++;
	}
}

int main(void)
{
	//UART functionalities 
	uart_initiate(UBRR_VALUE);

	//Enable Global interrupts
	sei();
    while (1){
		//Trying to distinguish between the message sent from USB and message sent from LLC
		//Better way of clearing array or using message_pos required
		if(received_message[message_index] == '}'){
			//Disable Global interrupts 
			cli();

			uart_transmit("Message sent from LLC {");
			uart_transmit_byte(13);
			uart_transmit_byte(10);

			uart_transmit((char*) received_message);

			//Clearing the array to receive another message
			message_index = 0;
			received_message[0] = '\0';
			net_brackets = 0;
			message_complete = false;

			uart_transmit_byte(13);
			uart_transmit_byte(10);
			uart_transmit("} Message sent from LLC");
			uart_transmit_byte(13);
			uart_transmit_byte(10);

			//Re-enable Global interrupts 
		sei();
		}
	}
}


