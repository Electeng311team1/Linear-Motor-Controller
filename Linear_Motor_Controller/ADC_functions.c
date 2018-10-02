/*
 * ADC_functions.c
 *
 * Created: 2/10/2018 11:03:44 AM
 *  Author: Helena
 */ 


void adc_Init(){
	ADMUX |=  (1<<REFS0);											// Reference selection to external AVcc
	ADCSRA |= (1<<ADPS2)|(1<<ADPS0);								// ADC prescaler selection divide by 32 for 250khz sampling rate
	ADCSRA |= (1<<ADEN);											// ADC enable
	ADMUX &= 0b11110000;
}

void adc_SetChannel(uint8_t channel){
	if(channel == 1){
		ADMUX |= ADC1;												// Mux select for voltage channel 1		
		} else {
		ADMUX &= ~ADC1;												// Mux select for current channel 0
	}
}

double adc_Read(uint8_t channel_sel){
	adc_SetChannel(channel_sel);									// Set channel to read from
	ADCSRA |= (1<<ADSC);											// Start conversion
	while (!(ADCSRA & (1<<ADIF)));									// Wait for conversion to finish (polling)
	uint16_t tempADC = ADC;											//Store the value in a temporary variable

	return (double)tempADC;
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