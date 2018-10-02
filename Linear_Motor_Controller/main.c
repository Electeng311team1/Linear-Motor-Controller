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

//volatile bool isNegativeCycle;


int main(void)
{
	/*
	uart_initiate(UBRR_VALUE);
	char message[MAX_SIZE] = "Hello we are Team 1!\n\r";
	*/
	
	float frequency = 9;
	float dutyCycle= 0.5; //must be greater than 0, less that 1
	
	setFrequency(frequency, dutyCycle);//acquire on/off times or alternatively could manually set on and off times
	driverTimers_Init();
	driverTimersInterrupts_Init();
	
	void adc_Init();
	
	
    while (1){
		
		//if messaged received requesting dutycycle
		//setFrequency(frequency, dutyCycle);
		
		getRawData();	
		
		modTime();															// Modulus time array with AC_freq_count to facilitate 'folding' the data into one period
		sortArray();														// Manipulate arrays into time-order
		adc_Calculate();													// Adjust voltage and current data to real-world values

		uint16_t power_value = calculatePower();						// Calculate power/voltage/current values, need to be x100 for display
		uint16_t voltage_value = calculateVoltage();
		uint16_t current_value = calculateCurrent();
		
		//uart_transmit(message);
    }
}

