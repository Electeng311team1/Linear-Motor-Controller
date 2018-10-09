/*
 * ADC.c
 *
 * Created: 26/09/2018
 *  Author: Team 1
 */ 

 #include "includes.h"

 void adc_initiate(){
 	//Set ADC prescalar
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
 
 	//Select AVcc reference
	//ADMUX |= (1 << REFS0);

	//Enable ADC
	ADCSRA |= (1 << ADEN);
	
	//ADMUX |= (1 << ADLAR);
	
	//Enable ADC interrupt
	ADCSRA |= (1 << ADIE);
	
	//Enable ADC auto trigger
	ADCSRA |= (1 << ADATE);
	
	//Set auto trigger source to timer 0
	//ADCSRB |= (1 << ADTS1) | (1 << ADTS0); 

	//Set timer 0 prescalar to 64
	//TCCR0B |= (1 << CS00) | (1 << CS01);

	//Enable conversion 

	ADMUX |= (1 << MUX0);

	ADCSRA |= (1 << ADSC);
 }

 void adc_start(float* operating_frequency){
// 
// 	OCR0A = 200;//(uint8_t)(F_CPU/(64.0 * (float)SAMPLING_SIZE * (*operating_frequency)));
// 	TIMSK0 |= (1 << OCIE0A);
// 	TCNT0 = 0;
	compA_count = 0;
 }

//  ISR(TIMER0_COMPA_vect){
// 	ADCSRA |= (1 << ADSC);
// 	compA_count++;
// 	TCNT0 = 0;
//  }

 ISR(ADC_vect){
	if(!testvalue){
		testadcvalue = ADC;
		testvalue = true;
	}
// 	if (beginCalculation==0){
// 		ADCSRA &= ~(1<<ADATE);
// 		if ((i%10) == 0){
// 			voltage[i/10] = ADC;
// 			voltageTime[i/10] = TCNTO + compA_count*OCROA;
// 			ADMUX &= ~(1<<MUX0);
// 		} else {
// 			current[i] = ADC;
// 			currentTime = TCNTO + compA_count*OCROA;
// 			if (i%10 == 9){
// 				ADMUX |= (1<<MUX0); 
// 			}
// 		}
// 		i++;
// 		if (i == SAMPLING_SIZE-1){
// 			TIMSK0 &= ~(1<<OCIE0A);
// 			beginCalculation = 1; //stops adc sampling and saves values to perform calculations
// 			i=0;
// 			compA_count = 0;
// 		} else {
// 			ADCSRA |= (1<<ADATE);
// 		}

}


// float calculateAverageSupply(){
// 	float finalSupply = 0;
// 
// }

