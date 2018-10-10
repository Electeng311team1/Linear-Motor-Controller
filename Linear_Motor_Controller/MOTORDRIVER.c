/*
 * motordriver.c
 *
 * Created: 4/10/2018 
 *  Author: Team 1
 */ 

#include "includes.h"

//This function is used initiate the timers used to drive the motor
 void driver_timer_initiate(void){
	//Timer 1 8bit (no prescalar)
	TCCR2B |= (1 << CS20);
	OCR2A = HIGH_OFF_TIME_COUNT_VALUE;
	OCR2B = LOW_OFF_TIME_COUNT_VALUE;
	
	//Timer 2 16bit (8 prescalar)
	TCCR1B |= (1 << CS11);

	//Set pins to output
	DDRB |= (1 << DDB0);
	DDRD |= (1 << DDD5) | (1 << DDD6) | (1 << DDD7);

	//Set initial switches
	SET_SW1;
	SET_SW4;
	CLR_SW2;
	CLR_SW3;

	change_duty = false;
	first_cycle = true;
 }

//This function is called to set the motor to drive to a given frequency and mass flow rate
 void set_parameters(float frequency, uint8_t mfc){
	if(mfc == 0){
		stop_motor();
	}
	else{
		mfr = mfc;
		float duty_cycle = (float)mfc/255;
		float off_time = ((1000/(2*(frequency)))-(LOW_OFF_TIME+HIGH_OFF_TIME)/1000)*(1-duty_cycle);
		float on_time = ((1000/(2*(frequency)))-(LOW_OFF_TIME+HIGH_OFF_TIME)/1000)*(duty_cycle);

		//Set T1 Compare
		t1_compare_a = (uint16_t)((on_time+off_time+HIGH_OFF_TIME/1000)*1000);
		t1_compare_b = (uint16_t)(on_time*1000);

		isNegative = false;
		change_duty = true;

		if(first_cycle){
			OCR1A = (uint16_t)t1_compare_a;
			OCR1B = (uint16_t)t1_compare_b;
			first_cycle = false;
		}
	}

	//Initialise timer interrupt
	TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);
	TCNT1 = 0;
}

//This function slowly ramps up the motor to the desired mass flow rate
void soft_start(float req_freq, int req_mfc){
	unsigned int i = 0;
	while(i != req_mfc){
		set_parameters(req_freq, i);
		i++;
	}
}

//This function will force a hard stop to the driving of the motor
void stop_motor(){
	TIMSK1 &= ~(1 << OCIE1B);
	TIMSK1 &= ~(1 << OCIE1A);
	TIMSK2 &= ~(1 << OCIE2A);
	TIMSK2 &= ~(1 << OCIE2B);
	CLR_SW1;
	_delay_ms(10);
	CLR_SW2;
	_delay_ms(10);
	SET_SW3;
	_delay_ms(10);
	SET_SW4;

	first_cycle = true;
	resonance_counter = 0;
}

//This function calculates resonant frequency by half driving the motor for a brief amount of time
//then reading the ADC values representing the back emf. Following this peak detection is used to
//determine the frequency of the system. The frequency will then be automatically adjust accordingly
void calculate_resonance(float* freq, uint8_t mfc){
	set_parameters(*freq, 150);
	_delay_ms(200);
 	stop_driving = false;
 	while(!stop_driving){};

 	stop_motor();

// 	//adc_start(freq);
	//_delay_ms(500);
 
 	//calculate resonance
 	
	set_parameters(*freq, mfc);
	
}

//Timer used to time high side discharge times of P type MOSFETS
ISR(TIMER2_COMPA_vect){	
	TIMSK2 &= ~(1 << OCIE2A);
	if(isNegative == false){
		SET_SW3;
	}
	else{
		SET_SW4;
	}
}

//Timer used to time on time of driver signal
ISR(TIMER1_COMPA_vect){
	if(isNegative == false){
		CLR_SW4;
	}
	else{
		CLR_SW3;
	}
	TIMSK1 &= ~(1 << OCIE1A);
	TIMSK1 &= ~(1 << OCIE1B);
	TIMSK2 |= (1 << OCIE2B);
	TCNT2 = 0;
}

//Timer used to time low side discharge times of N type MOSFETS
ISR(TIMER2_COMPB_vect){
	TIMSK2 &= ~(1 << OCIE2B);
	if(mfr == 0){
		stop_motor();		
	}
	else{
		if(isNegative == false){
			SET_SW2;
			isNegative = true;
		}
		else{
			SET_SW1;
			isNegative = false;
			if(resonance_counter < CYCLES){
				resonance_counter++;
				if(change_duty == true){
					OCR1A = t1_compare_a;
					OCR1B = t1_compare_b;
					change_duty = false;
				}	
			}
			else{
				check_resonance = true;
				resonance_counter = 0;
			}
			if(force){
				resonance_counter = 0;
			}
		}

		//Continue to drive next stage of H-bridge
		TIMSK1 |= (1 << OCIE1B);
		TIMSK1 |= (1 << OCIE1A);
		TCNT1 = 0;
	}
		
}

//Timer used to time on and off time of driver signal
ISR(TIMER1_COMPB_vect){
	TIMSK1 &= ~(1 << OCIE1B);
	if(isNegative == false){
		CLR_SW1;
	}
	else{
		CLR_SW2;
	}
	
	TIMSK2 |= (1 << OCIE2A);
	TCNT2 = 0;
	stop_driving = true;
}

//Function to play the Imperial March
void project_skywalker(){
	set_parameters(CLOW, x);
	_delay_ms(560);
	set_parameters(CLOW, 1);
	_delay_ms(100);
	set_parameters(CLOW, x);
	_delay_ms(560);
	set_parameters(CLOW, 1);
	_delay_ms(100);
	set_parameters(CLOW, x);
	_delay_ms(560);
	set_parameters(CLOW, 1);
	_delay_ms(100);
	set_parameters(AFLATLOW, x);
	_delay_ms(420);
	set_parameters(AFLATLOW, 1);
	_delay_ms(100);
	set_parameters(E, x);
	_delay_ms(140);
	set_parameters(E, 1);
	_delay_ms(100);
	set_parameters(CLOW, x);
	_delay_ms(560);
	set_parameters(CLOW, 1);
	_delay_ms(100);
	set_parameters(AFLATLOW, x);
	_delay_ms(420);
	set_parameters(AFLATLOW, 1);
	_delay_ms(100);
	set_parameters(E, x);
	_delay_ms(140);
	set_parameters(E, 1);
	_delay_ms(100);
	set_parameters(CLOW, x);
	_delay_ms(1120);
	set_parameters(CLOW, 1);
	_delay_ms(100);

	set_parameters(G, x);
	_delay_ms(560);
	set_parameters(G, 1);
	_delay_ms(100);
	set_parameters(G, x);
	_delay_ms(560);
	set_parameters(G, 1);
	_delay_ms(100);
	set_parameters(G, x);
	_delay_ms(560);
	set_parameters(G, 1);
	_delay_ms(100);
	set_parameters(AFLAT, x);
	_delay_ms(420);
	set_parameters(AFLAT, 1);
	_delay_ms(100);
	set_parameters(E, x);
	_delay_ms(140);
	set_parameters(E, 1);
	_delay_ms(100);
	set_parameters(B, x);
	_delay_ms(560);
	set_parameters(B, 1);
	_delay_ms(100);
	set_parameters(AFLATLOW, x);
	_delay_ms(420);
	set_parameters(AFLATLOW, 1);
	_delay_ms(100);
	set_parameters(E, x);
	_delay_ms(140);
	set_parameters(E, 1);
	_delay_ms(100);
	set_parameters(CLOW, x);
	_delay_ms(1120);
	set_parameters(CLOW, 1);
	_delay_ms(100);
}