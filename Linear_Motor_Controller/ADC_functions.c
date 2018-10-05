/*
 * ADC_functions.c
 *
 * Created: 2/10/2018 11:03:44 AM
 *  Author: Helena
 */ 


#include "includes.h"
volatile uint8_t i = 0; //adc values index
#define v_scaler 2.6
float Vref = 5.0;
float adc_convert = 5.0/1024.0;
uint32_t Pinst[SAMPLING_SIZE] = {0};

void adc_Init(){
	//Read Vref
	adc_convert = Vref/1024;
	ADMUX |=  (1<<REFS0);											// Reference selection to external AVcc
	ADCSRA |= (1<<ADPS2)|(1<<ADPS0);								// ADC prescaler selection divide by 32 for 250khz sampling rate
	ADCSRA |= (1<<ADEN);											// ADC enable
	ADMUX |= (1<<ADLAR);											//left adjust ADC register to read 10 bit value
	
	ADCSRA |= (1<<ADIE); //enable ADC interrupts
	ADCSRA |= (1<<ADATE); //enable ADC Auto trigger
	ADCSRB |= (1<<ADTS1) | (1<<ADTS0); //set Autotrigger to fire when Timer 0 compare interrupt executes
	
	//set up autotrigger source Timer 0
	TCCR0B |= (1<<CS00) | (1<<CS01);										//Start timer with 64 prescaler
	
	TIMSK0 |= (1<<OCIE0A);	//enable Timer 0 compare A interrupt
	OCR0A |= (uint8_t) (round(F_CPU/(64.0*SAMPLING_SIZE*OPERATING_FREQUENCY))); //set compare value to ensure ADC converts at  time intervals to evenly distribute sampling across a given period  
	TCNT0 = 0;//
}


  ISR(TIMER0_COMPA_vect){
	TCNT0=0;
	ovf_count++;
  }
  
  ISR(ADC_vect){
	  //disable adc conversion complete interrupts
	  
	 // uint16_t tempADC = ADC;	//Store the value in a temporary variable
	if (isCalculating==0){
		ADCSRA &= ~(1<<ADATE); //disable adc auto trigger	 
	 if ((i%10) ==0){
	 	  	voltage[i/10] = ADC;								//Voltage reads in values from ADC1 (once every 9 current readings)
	  		voltageTime[i/10] = TCNT0 + i*ovf_count*OCR0A;									//Load in values from TCNT0
			ADMUX &= ~(1<<MUX0);												// Mux select for current channel 0 next sample
	 } else {
		 	current[i] = ADC;							//Current reads in values from ADC0
		 	currentTime[i] = TCNT0 + i*ovf_count;	
			if ((i%10) ==9){
				 ADMUX |= (1<<MUX0);												// preparing to sample voltage next from channel 1	
			}							 
	 i++;
	 
	 if (i==SAMPLING_SIZE){
		 TIMSK0 &= ~(1<<OCIE0A); //disable timer interrupt	 
		 isCalculating = 1;	//switch to parameter calculation mode
		 i=0;
		 ovf_count=0;
	 } else {
		  ADCSRA |= (1<<ADATE); //enable auto trigger for adc to convert at next interval
	 }
	 
	 }
	}


}


float calculateAverageSupply(){
	float finalSupply=0;
	
	for (uint8_t j=0; j< 4; i++){
		finalSupply += (adc_convert * v_scaler * voltage[j]);
	}
	
	finalSupply = finalSupply/4.0;
	
	return finalSupply;
}
float calculateCurrentRMS(){ //Perform a calculation for rms voltage by square rooting sum of the squares
		float rmsCurrent = 0;
		for (uint8_t i = 0; i < SAMPLING_SIZE; i++){
			
			if ((i%10) ==0)		{
				rmsCurrent += (current[i+1] + current[i-1])*(current[i+1] + current[i-1])/4.0; //uses average of adjacent values for missing ADC result
			} else {
				rmsCurrent += (current[i] * current[i]);
			}						
			
		}

		rmsCurrent = rmsCurrent/(SAMPLING_SIZE);									
		rmsCurrent = (sqrt(rmsCurrent))*Vref* .078/(3.8*0.025*1024);															
		
		return rmsCurrent;													//Return 10x the value so it work with usart_transmit function
}

float calculatePower(){
	float timeInterval;
	float P_area =0;
	
	double Vsupply_average = calculateAverageSupply();
	double Vsupply_average_squared = Vsupply_average*Vsupply_average;//*adc_convert;
	for (uint8_t i =0; i< SAMPLING_SIZE; i++){
		if (((currentTime[i]>= switchingPointTimes[0]) && (currentTime[i]<= switchingPointTimes[1])) || ((currentTime[i]>= switchingPointTimes[2]) && (currentTime[i]<= switchingPointTimes[3]))){//still need to multiply by 64/8MHZ
			Pinst[i] = current[i]* current[i]* Vsupply_average_squared;//squaring power curve
		}
	}
		
	for (uint8_t i =0; i< (SAMPLING_SIZE-1); i++){
		//still need to multiply by 64/8MHZ

			timeInterval = (currentTime[i+1]-currentTime[i])/F_CPU;
			P_area += (Pinst[i] + Pinst[i+1])*timeInterval*OPERATING_FREQUENCY/2.0;//finding the area under the curve and divide by period	
	
	}
	
	float PRMS = (sqrt(P_area))*(Vref*0.78/(3.8*0.025*1024));
		
	
	return PRMS;
}

float calculateResonantFrequency(){
	float dutycycle = 0.6;//set constant duty cycle
	float resonantFrequency = 12.4;
	//set for range of frequency
	for (float i=9; i<18; i=i+0.5 ){
		setFrequency(i, dutycycle);
	}
	
	return resonantFrequency;
}
