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

void adc_SetChannel(uint8_t channel){
	if(channel == 1){
			
		} else {
		
	}
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
	  		voltageTime[i/10] = TCNT0 + i*ovf_count;									//Load in values from TCNT0
			 ADMUX &= ~(1<<MUX0);												// Mux select for current channel 0 next sample
	 } else {
		 	current[i] = ADC;							//Current reads in values from ADC0
		 	currentTime[i] = TCNT0 + i*ovf_count;	
			if ((i%10) ==9){
				 ADMUX |= (1<<MUX0);												// preparing to sample voltage next from channel 1	
			}							
	 }
	 i++;
	 
	 if (i==SAMPLING_SIZE){
		 ADCSRA |= (1<<ADATE); //enable auto trigger for adc to convert at next interval
		 isCalculating = 1;	//switch to parameter calculation mode
		 i=0;
		 ovf_count=0;
	 }
	 
	}


}


void adc_Calculate(){

	double adc_convert = 5.0/1024.0;
	double v_scaler = 9.64809;//123.0/13.0;//old value:123/13 = 9.46, calibrated: 9.64809;
	double c_scaler = 54.0/115.0;
	
	for (uint8_t i = 0; i < READ_SAMPLE_LENGTH; i++){											//Iterate for twice the sample length
		sortedVoltage[i] = ((sortedVoltage[i]*adc_convert)-Vref_25)*v_scaler;					//Account for the reference voltage and scaling that occurred in the analogue section
		sortedCurrent[i] = ((sortedCurrent[i]*adc_convert)-Iref_25)*c_scaler;					//Repeated for current
	}
}

void getRawData(){

	//TCCR0B &= ~(1<<CS00); //turns off counter
	TCNT2 = 0;
	uint8_t cycle_count = 0;
	uint8_t i = 0;

	uint8_t j = TCNT0 + AC_freq_count;

	while(TCNT0 <= AC_freq_count) {								//Repeated three times with a delay between each, creating a staggered set of data

		i++;
	}

	uint8_t k = TCNT0 + DELAY_BETWEEN_CYCLES;
	while(TCNT0 <= k);

	while(TCNT0 <= (AC_freq_count*2+DELAY_BETWEEN_CYCLES)) {
		current[i] = adc_Read(0);
		currentTime[i] = TCNT0;
		voltage[i] = adc_Read(1);
		voltageTime[i] = TCNT0;

		i++;
	}

	k = TCNT0 + DELAY_BETWEEN_CYCLES;
	while(TCNT0 <= k);

	while(TCNT0 <= (AC_freq_count*3+DELAY_BETWEEN_CYCLES*2)) {
		current[i] = adc_Read(0);
		currentTime[i] = TCNT0;
		voltage[i] = adc_Read(1);
		voltageTime[i] = TCNT0;

		i++;
	}

	READ_SAMPLE_LENGTH = i;			//Total number of samples obtained
}