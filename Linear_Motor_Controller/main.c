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

//Test variables 
volatile uint8_t received_byte = 0;
volatile uint8_t new_byte = false;

//This is the ISR for UART receive
ISR(USART_RX_vect){
//  	while(!RECEIVE_COMPLETE);
// 	char tmp = UDR0; 
// 	if(tmp == '{'){
// 		net_brackets++;
// 	}
// 	else if(tmp == '}'){
// 		net_brackets--;
// 		if(net_brackets == 0){
// 			message_complete = true;
// 		}
// 	}
// 
// 	if(net_brackets > 0){
// 		received_message[message_index] = tmp;
// 		message_index++;
// 	}
	while(!RECEIVE_COMPLETE);
	received_byte = UDR0;
	new_byte = true;

	//uart_flush();
}

int main(void)
{
	uint8_t old_byte = 0;
	//UART functionalities 
	uart_initiate(UBRR_VALUE);

	//Enable Global interrupts
	sei();

    while (1){

		if(new_byte == true){
			new_byte = false;
			//uart_flush();
			//UCSR0B &= ~(1 << RXEN0);

			if(old_byte != received_byte){
				uart_transmit_byte(received_byte);
			}
			old_byte = received_byte;
			uart_flush();
		}


	}
}


