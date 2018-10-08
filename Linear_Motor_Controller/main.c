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
	adc_Init();
	
	driverTimers_Init();
	//driverTimersInterrupts_Init();
	
	float voltage_value;
	float current_value;
	float power_value;
	float resonantFrequency;
	
	uint8_t calculateResonant=0;
	
	float frequency = 9;
	float dutyCycle= 0.5; //must be greater than 0, less that 1
	setFrequency(frequency, dutyCycle);//acquire on/off times or alternatively could manually set on and off times
	adc_Begin();
	
    while (1){
		
		//if messaged received requesting dutycycle
		//setFrequency(frequency, dutyCycle);

		if (isCalculating==1){
			ADCSRA &= ~(1<<ADATE); //disable adc auto trigger	 
			
				voltage_value = calculateAverageSupply();
				current_value = calculateCurrentRMS();
				power_value = calculatePower(voltage_value);						// Calculate power/voltage/current values, need to be x100 for display
				isCalculating=0;	//return to converting ADC values
		}
		//calculateResonant=1 if want to find resonant frequency
		if (calculateResonant==1){
			setFrequency(0, dutyCycle);
			resonantsamplingcomplete = 0;
			resonantFrequency = calculateResonantFrequency();
			frequency = resonantFrequency;
			setFrequency(frequency, dutyCycle);
			
		}
		
		//if dutyCycle is low 
		if (dutyCycle<= LOWDUTYTHRESHOLD){
			setHalfDrive = 1;
			float halfDriveDutyCycle = dutyCycle * 1.5; //arbitrary scale
			setFrequency(frequency, halfDriveDutyCycle);
		} else {
			setHalfDrive=0;
		}
			

		
		//uart_transmit(message);
    }
}

