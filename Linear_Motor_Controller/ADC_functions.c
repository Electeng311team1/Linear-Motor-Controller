/*
 * ADC_functions.c
 *
 * Created: 2/10/2018 11:03:44 AM
 *  Author: Helena
 */ 


#include "includes.h"
volatile uint8_t adc_i = 0; //adc values index
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
	
	TCCR0B |= (1<<CS00) | (1<<CS01);										//Start timer with 64 prescaler
}

void adc_Begin(){
	ADCSRA |= (1<<ADIE); //enable ADC interrupts
	ADCSRA |= (1<<ADATE); //enable ADC Auto trigger
	ADCSRB |= (1<<ADTS1) | (1<<ADTS0); //set Autotrigger to fire when Timer 0 compare interrupt executes
	
	//set up autotrigger source Timer 0

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
	 if (isCalculating==0) { //do not start loading new ADC values until calculations are complete
		ADCSRA &= ~(1<<ADATE); //disable adc auto trigger	 
	 if (((adc_i%10) ==0) && (calculateResonant==0)){
	 	  	voltage[adc_i/10] = ADC;								//Voltage reads in values from ADC1 (once every 9 current readings)
	  		voltageTime[adc_i/10] = TCNT0 + ovf_count*OCR0A;									//Load in values from TCNT0
			ADMUX &= ~(1<<MUX0);												// Mux select for current channel 0 next sample
	 } else {
		 
		 	current[adc_i] = ADC;							//Current reads in values from ADC0
		 	currentTime[adc_i] = TCNT0 + ovf_count*OCR0A;	
			if (((adc_i%10) ==9) && (calculateResonant==0)){
				 ADMUX |= (1<<MUX0);												// preparing to sample voltage next from channel 1	
	 }							 
	 adc_i++;
	 
	 if (adc_i==SAMPLING_SIZE){
		 TIMSK0 &= ~(1<<OCIE0A); //disable timer interrupt	 
		
		 if (calculateResonant==1){
			 resonantsamplingcomplete=1;
		 } else {
			 isCalculating = 1;	//switch to parameter calculation mode
		 }	
		 
		 adc_i=0;
		 ovf_count=0;
	 } else {
		  ADCSRA |= (1<<ADATE); //enable auto trigger for adc to convert at next interval
	 }
	 
	 }
	}



}


float calculateAverageSupply(){
	float finalSupply=0;
	
	for (uint8_t i=0; i< SAMPLING_SIZE/10; i++){
		finalSupply += (adc_convert * v_scaler * voltage[i]);
	}
	
	finalSupply = finalSupply/(SAMPLING_SIZE/10);
	
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
		if (((currentTime[i]>= (switchTime1/8)) && (currentTime[i]<= (switchTime2/8))) || ((currentTime[i]>= (switchTime3/8)) && (currentTime[i]<= (switchTime4/8)))){//still need to multiply by 64/8MHZ
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

	
	float resonantFrequency = 0;
	uint8_t pointsPassedPeak = 0;
	uint8_t measure = 1;
	uint8_t emfTrough=10; //arbitrary number to compare emf troughs
	uint8_t pointsPassedTrough;
	uint16_t emfPeakTime1;
	uint16_t emfPeakTime2;
	uint8_t emfPeak1;
	uint8_t emfPeak2;
	
	
	setFrequency(10,0);
	ADCSRA &= ~(1<<ADATE); //disable ADC Autotrigger
	TIMSK0 &= ~(1<<OCIE0A);
	//turn off all switching and switches
	
	
	
	OCR0A |= (uint8_t) (RESONANTDECAYTIME*F_CPU/ (SAMPLING_SIZE *64.0)); //To distribute 150 samples evenly through the emf decay period
	TIMSK0 |= (1<<OCIE0A);	//enable Timer 0 compare A interrupt
	ADCSRA |= (1<<ADATE);//re-enable autotrigger
	
	
	SET_SW4;
	SET_SW1;
	
	_delay_ms(40); 
	
	CLEAR_SW1;
	SET_SW3;
	TIMSK0 |= (1<<OCIE0A); //enabling ADC autotrigger source
	TCNT0 = 0;
	
	while(calculateResonant==1){ //wait until ADC has made 150 samples
		if(resonantsamplingcomplete ==1){ //process to find resonant frequency
			ADCSRA &= ~(1<<ADATE); //disable ADC Autotrigger
			TIMSK0 &= ~(1<<OCIE0A);	//enable Timer 0 compare A interrupt

			
			emfPeak1 = current[0];
			for (uint8_t i=0; i< SAMPLING_SIZE; i++){
				
				switch(measure){
					case 1:
						if ((current[i]>=emfPeak1) && (current[i+1]<=current[i])){ //have found local peak
							emfPeakTime1 = currentTime[i];
							emfPeak1 = current [i];
							pointsPassedPeak = 0;
						} 
						if (current[i]<=emfPeak1){
							pointsPassedPeak ++;
						}
						if (pointsPassedPeak>=5){
							measure =2;
						}
					break;
					
					case 2:
						if ((current[i] <= emfTrough) && (current[i+1]>=current[i])) {//have found local trough
							emfTrough=current[i];
							pointsPassedTrough = 0;

						}
						if (current[i] >= emfTrough) {
							pointsPassedTrough ++;
							if (pointsPassedTrough>=5){
								emfPeak2 = emfTrough;
								measure =3;
							}
						}
					break;
					
					case 3:
						if ((current[i]>=emfPeak2) && (current[i+1]<=current[i])){ //second local peak found
							emfPeakTime2 = currentTime[i];
							emfPeak2 = current [i];
							pointsPassedPeak = 0;
						}
					break;
				}		
		}
		//Peak values are found so set exiting condition from the while waiting loop
		calculateResonant = 0;	
		float resonantFrequency = 1/((emfPeakTime2-emfPeakTime1)* 64/F_CPU);//calculating resonant frequency
		}
	
	}
	

	return resonantFrequency;
}