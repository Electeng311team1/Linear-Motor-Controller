/*
 * Linear_Motor_Controller.c
 *
 * Created: 26/09/2018 
 *	Author: Team 1
 */ 

#include "includes.h"
#define BAUD_RATE 9600
#define UBRR_VALUE F_CPU/16/BAUD_RATE-1
#define MAX_SIZE 255
#define VERSION "1.1.2"

//IFDEFS

volatile char received_message[MAX_SIZE];
volatile uint8_t message_index = 0;
volatile uint8_t net_brackets = 0;
volatile uint8_t message_complete = false;
volatile uint8_t receive_error = false;
volatile uint8_t message_start = false;

volatile float* frequency;
volatile int* mfc;

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

	//Enable SONG mode
	*frequency = 12;
	*mfc = 100;

	//enable timers
	driver_timer_initiate();
	set_parameters((float*)frequency, (int*)mfc);
	
    while (1){
		if(message_complete == true){
			UCSR0B &= ~(1 << RXEN0);
			uart_transmit("\n\rFrom Microcontroller: ");
			uart_transmit((char*)received_message);
			process_message((char*)received_message, (int*)mfc);
			set_parameters((float*)frequency, (int*)mfc);
			uart_transmit("\n\r");
			message_complete = false;
			message_start = false;
			message_index = 0;
			UCSR0B |= (1 << RXEN0);
		}
		else if(receive_error == true){
			UCSR0B &= ~(1 << RXEN0);
			uart_transmit("\n\rFrom Microcontroller: ");
			uart_transmit("Error! The command is invalid\n\r");
			receive_error = false;
			message_start = false;
			net_brackets = 0;
			UCSR0B |= (1 << RXEN0);
		}
	}
}


