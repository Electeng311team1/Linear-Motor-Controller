/*
 * Linear_Motor_Controller.c
 *
 * Created: 1/09/2018
 * Author : Team 1
 */ 

#include "includes.h"

#define BAUD_RATE 9600
#define UBRR_VALUE F_CPU/16/BAUD_RATE-1
#define MAX_MESSAGE_SIZE 20	

volatile unsigned char msg[MAX_MESSAGE_SIZE];
volatile int i = 0;
volatile int net_brackets = 0;

ISR(USART_RX_vect){
	unsigned char temp;
	temp = UDR0;
	if(i < MAX_MESSAGE_SIZE){
		if(temp == '{'){
			net_brackets++;
		}
		else if(temp == '}') {
			net_brackets--;
		}
		else{
			if (net_brackets == 0){
				i = 0;
				net_brackets = 0;
				uart_flush();
				//Stop interrupt
			}
			else{
				msg[i] = temp;
				i++;
			}
		}
	}
	else{
		i = 0;
		net_brackets = 0;
		uart_flush();
	}
}

int main(void)
{	
	initialise_uart(UBRR_VALUE);
	sei(); //Enabling global interrupts 
	
	transmit_message("Hello we are Team 1!\n\r");
	char* received_message = "";
    while (1) 
    {
		flash_led();
		transmit_message("new\n\r");
		receive_message(received_message);
		transmit_message(received_message);
		received_message = "";
		_delay_ms(500);
	}
}

