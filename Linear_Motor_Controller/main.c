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
volatile uint8_t message_index = 0;
volatile uint8_t net_brackets = 0;
volatile uint8_t message_complete = false;
volatile uint8_t receive_error = false;
volatile uint8_t message_start = false;

//ISR for UART receive
ISR(USART_RX_vect){
	char tmp = UDR0; 
	if(message_complete == false){
		if(tmp == '{'){
			net_brackets++;
			message_start = true;
		}
		else if((tmp == '}') && (message_start == true)){
			net_brackets--;
			if(net_brackets == 0){
				message_complete = true;
			}
		}

		if(message_start == true){
			received_message[message_index] = tmp;
			message_index++;
			if(message_complete == true){
				received_message[message_index] = '\0';
			}
		}

		if((message_index == (MAX_SIZE-1)) || (net_brackets > 3)){
			receive_error = true;
			message_start = false;
			message_complete = false;
			message_index = 0;
		}
	}
}

int main(void)
{
	//UART functionalities 
	uart_initiate(UBRR_VALUE);

	//Enable Global interrupts
	sei();

    while (1){
		if(message_complete == true){
			UCSR0B &= ~(1 << RXEN0);
			UCSR0B &= ~(1<<RXCIE0);
			uart_transmit("\n\r");
			uart_transmit("From Microcontroller: ");
			uart_transmit((char*)received_message);
			uart_transmit("\n\r");
			message_complete = false;
			receive_error = false;
			message_start = false;
			message_index = 0;
			UCSR0B |= (1 << RXEN0);
			UCSR0B |= (1 << RXCIE0);
		}
		else if(receive_error == true){
			uart_transmit("From Microcontroller: ");
			uart_transmit("Error! The command is invalid");
			uart_transmit("\n\r");
			message_complete = false;
			receive_error = false;
			message_start = false;
			net_brackets = 0;
		} 
	}
}


