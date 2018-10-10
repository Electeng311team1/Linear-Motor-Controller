/*
 * ADC.c
 *
 * Created: 26/09/2018
 *  Author: Team 1
 */ 

 #include "includes.h"

 //This function initiates ADC functionalities
 void adc_initiate(){
 	//Set ADC prescalar
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
 
	//Enable ADC
	ADCSRA |= (1 << ADEN);
	
	//Enable ADC interrupt
	ADCSRA |= (1 << ADIE);
	
	//Enable ADC auto trigger
	ADCSRA |= (1 << ADATE);
	
	//Set auto trigger source to timer 0
	ADCSRB |= (1 << ADTS1) | (1 << ADTS0); 

	//Set timer 0 prescalar to 64
	TCCR0B |= (1 << CS00) | (1 << CS01);

	//Enable conversion 
	//ADMUX |= (1 << MUX0);

	//ADCSRA |= (1 << ADSC);
 }

 //This function begins ADC operations
 void adc_start(float* operating_frequency){
 	OCR0A = (uint8_t)(F_CPU/(64.0 * (float)SAMPLING_SIZE * (*operating_frequency)));
	TIMSK0 |= (1 << OCIE0A);
	TCNT0 = 0;
	compA_count = 0;
	current_index = 0;
 }

 //This ISR controls the timer used for ADC auto trigger
 ISR(TIMER0_COMPA_vect){
	compA_count++;
	TCNT0 = 0;
 }

 //This ISR controls the storing of ADC values in their appropriate arrays

 ISR(ADC_vect){
	if(current_index != SAMPLING_SIZE){
		current_values[current_index] = ADC;//*5*0.78/3.8/0.25/1024;
		current_index++;
	}
	else{
		current_index = 0;
		uart_transmit("START OF CYCLE\n\r");
		for(unsigned int i = 0; i < SAMPLING_SIZE; i++){
				uint8_t thousands = (current_values[i]/1000) + 48;
				uint8_t hundreds = (((uint8_t)current_values[i]%1000)/100) + 48;
				uint8_t tens = ((((uint8_t)current_values[i]%1000)%100)/10) + 48;
				uint8_t ones = (((((uint8_t)current_values[i]%1000)%100)%10)) + 48;
				char array[5];
				array[0] = thousands;
				array[1] = hundreds;
				array[2] = tens;
				array[3] = ones;
				array[4] = '\0';
				uart_transmit(array);
				uart_transmit("\n\r");
		}
	}
}

