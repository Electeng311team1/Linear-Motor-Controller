/*
 * ADC_functions.c
 *
 * Created: 2/10/2018 11:03:44 AM
 *  Author: Helena
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
	ADCSRB |= (1 << ADTS1) | (1 << ADTS0);

	//Set timer 0 prescalar to 64
	TCCR0B |= (1 << CS00) | (1 << CS01);

	//Enable conversion

	ADMUX |= (1 << MUX0);

	ADCSRA |= (1 << ADSC);
}

void adc_start(float* operating_frequency){
	
	 	OCR0A = (uint8_t) 69;//(F_CPU/(64.0 * (float)SAMPLING_SIZE * (*operating_frequency)));
	 	TIMSK0 |= (1 << OCIE0A);
	 	TCNT0 = 0;
	compA_count = 0;
}

 ISR(TIMER0_COMPA_vect){
	//ADCSRA |= (1 << ADSC);
	compA_count++;
	TCNT0 = 0;
 }

ISR(ADC_vect){
// 	if(!testvalue){
// 		testadcvalue = ADC;
// 		testvalue = true;
// 	}
		if (beginCalculation==0){
	 		ADCSRA &= ~(1<<ADATE);
	 		if ((i%10) == 0){
				 j=i/10;
	 			voltage[j] = ADC;
				 
				 uint16_t tmp = voltage[j]; //gets correct adc readings sometimes but garbage other times
				 uint8_t thousands = (tmp/1000) + 48;
				 uint8_t hundreds = ((tmp%1000)/100) + 48;
				 uint8_t tens = (((tmp%1000)%100)/10) + 48;
				 uint8_t ones = ((((tmp%1000)%100)%10)) + 48;
				 char array[5];
				 array[0] = thousands;
				 array[1] = hundreds;
				 array[2] = tens;
				 array[3] = ones;
				 array[4] = '\0';
				 uart_transmit(array);
				 uart_transmit("\n\r");
				 
	 			voltageTime[j/10] = TCNT0 + compA_count*OCR0A;
	 			ADMUX &= ~(1<<MUX0);
	 		} else {
	 			current[i] = ADC;
				currentTime[i]= TCNT0 + compA_count*OCR0A;
				
// 				uint16_t tmp = current[i];
// 				uint8_t thousands = (tmp/1000) + 48;
// 				uint8_t hundreds = ((tmp%1000)/100) + 48;
// 				uint8_t tens = (((tmp%1000)%100)/10) + 48;
// 				uint8_t ones = ((((tmp%1000)%100)%10)) + 48;
// 				char array[5];
// 				array[0] = thousands;
// 				array[1] = hundreds;
// 				array[2] = tens;
// 				array[3] = ones;
// 				array[4] = '\0';
// 				uart_transmit(array);
// 				uart_transmit("\n\r");

				
	 			if (i%10 == 9){
	 				ADMUX |= (1<<MUX0);
	 			}
	 		}
	 		i++;
	 		if (i == (SAMPLING_SIZE-1)){
	 			TIMSK0 &= ~(1<<OCIE0A); //disables further adc reading
	 			beginCalculation = 1; //stops adc sampling and saves values to perform calculations
	 			i=0;
	 			compA_count = 0;
			} else {
	 			ADCSRA |= (1<<ADATE);
	 		}

	}
}

// float calculateAverageSupply(){
// 	float finalSupply = 0;
//
// }



float calculateAverageSupply(){
	float finalSupply=0;
	float adc_convert = 5.0/1024.0;
	for (uint8_t i=0; i< 4; i++){
		finalSupply += (adc_convert * v_scaler * voltage[i]);
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

float calculatePower(float Vsupply_average){
	float timeInterval;
	float P_area =0;
	
	//double Vsupply_average = calculateAverageSupply();
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

// float calculateResonantFrequency(){
// 	float dutycycle = 0.6;//set constant duty cycle
// 	float resonantFrequency = 12.4;
// 	//set for range of frequency
// 	for (float i=9; i<18; i=i+0.5 ){
// 		setFrequency(i, dutycycle);
// 	}
// 	
// 	return 0;
// }
