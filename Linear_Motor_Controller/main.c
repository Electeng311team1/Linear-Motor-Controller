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


int main(void)
{
	/*
	uart_initiate(UBRR_VALUE);
	char message[MAX_SIZE] = "Hello we are Team 1!\n\r";
	*/
	
	float frequency = 12;
	float dutyCycle= 50;
	
	setFrequency(frequency, dutyCycle);//acquire on/off times or alternatively could manually set on and off times
	driverTimers_Init();
	driverTimersInterrupts_Init();
	
    while (1){
		//uart_transmit(message);
    }
}

