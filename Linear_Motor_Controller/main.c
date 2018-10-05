/*
 * Linear_Motor_Controller.c
 *
 * Created: 26/09/2018 
 *	Author: Team 1
 */ 

#include "includes.h"


#define BAUD_RATE 9600
#define UBRR_VALUE F_CPU/16/BAUD_RATE-1

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

		if (isCalculating==1){
			ADCSRA &= ~(1<<ADATE); //disable adc auto trigger	 
			
				float voltage_value = calculateAverageSupply();
				float current_value = calculateCurrentRMS();
				float power_value = calculatePower();						// Calculate power/voltage/current values, need to be x100 for display
				isCalculating=0;	//return to converting ADC values
		}

		
		//uart_transmit(message);
    }
}

