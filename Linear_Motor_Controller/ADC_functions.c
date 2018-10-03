/*
 * ADC_functions.c
 *
 * Created: 2/10/2018 11:03:44 AM
 *  Author: Helena
 */ 
volatile uint8_t i = 0;

void adc_Init(){
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
	OCR0A |= (uint8_t) round(F_CPU/(64.0*SAMPLING_SIZE*OPERATING_FREQUENCY)); //set compare value to ensure ADC converts at  time intervals to evenly distribute sampling across a given period  
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


void adc_Calculate(){

	double 
	double v_scaler = 9.64809;//123.0/13.0;//old value:123/13 = 9.46, calibrated: 9.64809;
	double c_scaler = 54.0/115.0;
	
	for (uint8_t i = 0; i < READ_SAMPLE_LENGTH; i++){											//Iterate for twice the sample length
		sortedVoltage[i] = ((sortedVoltage[i]*adc_convert)-Vref_25)*v_scaler;					//Account for the reference voltage and scaling that occurred in the analogue section
		sortedCurrent[i] = ((sortedCurrent[i]*adc_convert)-Iref_25)*c_scaler;					//Repeated for current
	}
}

double calculateAverageSupply(){
	double finalSupply=0;
	
	for (uint8_t j=0; j< 4; i++){
		finalSupply += (adc_convert*v_scaler*voltage[j]);
	}
	
	finalSupply = finalSupply/4.0;
	
	return finalSupply;
}

double calculateCurrentRMS(){ //Perform a calculation for rms voltage by square rooting sum of the squares
		double rmsCurrent = 0;
		for (uint8_t i = 0; i < SAMPLING_SIZE; i++){
			
			if ((i%10) ==0)		{
				
			} else {
				rmsCurrent += (current[i] * current[i]);
			}						
			
		}

		rmsCurrent = rmsCurrent/(SAMPLING_SIZE);									
		rmsCurrent = sqrt(rmsCurrent);															
		
		return rmsCurrent;													//Return 10x the value so it work with usart_transmit function
}

double calculatePower(){
	
}