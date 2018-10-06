/*
 * motordriver.c
 *
 * Created: 4/10/2018 
 *  Author: Team 1
 */ 

 #include "includes.h"

 //Dead times in us 
 #define LOW_OFF_TIME 3 
 #define HIGH_OFF_TIME 26

 #define LOW_OFF_TIME_COUNT_VALUE (LOW_OFF_TIME*8)
 #define HIGH_OFF_TIME_COUNT_VALUE (HIGH_OFF_TIME*8)

 //Switch defines
 #define SET_SW1 PORTB |= (1 << PB0)
 #define CLR_SW1 PORTB &= ~(1 << PB0) 
 #define SET_SW2 PORTD |= (1 << PD5)
 #define CLR_SW2 PORTD &= ~(1 << PD5)
 #define SET_SW3 PORTD |= (1 << PD7)
 #define CLR_SW3 PORTD &= ~(1 << PD7)
 #define SET_SW4 PORTD |= (1 << PD6)
 #define CLR_SW4 PORTD &= ~(1 << PD6) 

 void driver_timer_initiate(void){
	//Timer 1 8bit (no prescalar)
	TCCR0B |= (1 << CS00);
	OCR0A = HIGH_OFF_TIME_COUNT_VALUE;
	OCR0B = LOW_OFF_TIME_COUNT_VALUE;
	
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

	isNegative = false;
 }

 void set_parameters(float* frequency, int* mfc){
	float duty_cycle = (float)*mfc/255;
	float off_time = ((1000/(2*(*frequency)))-(LOW_OFF_TIME+HIGH_OFF_TIME)/1000)*(1-duty_cycle);
	float on_time = ((1000/(2*(*frequency)))-(LOW_OFF_TIME+HIGH_OFF_TIME)/1000)*(duty_cycle);

	//Set T1 Compare
	OCR1A = (uint16_t)((on_time+off_time+HIGH_OFF_TIME/1000)*1000);
	OCR1B = (uint16_t)(on_time*1000);

	//Initialise timer interrupt
	TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);
}

ISR(TIMER0_COMPA_vect){	
	TIMSK0 &= ~(1 << OCIE0A);
	if(isNegative == false){
		SET_SW3;
	}
	else{
		SET_SW4;
	}

}

ISR(TIMER1_COMPA_vect){
	if(isNegative == false){
		CLR_SW4;
	}
	else{
		CLR_SW3;
	}
	TCNT0 = 0;
	TIMSK0 |= (1 << OCIE1B);
}

ISR(TIMER0_COMPB_vect){
	TIMSK0 &= ~(1 << OCIE0B);
	if(isNegative == false){
		SET_SW2;
		isNegative = true;
	}
	else{
		SET_SW1;
		isNegative = false;
	}
	TCNT1 = 0;
}

ISR(TIMER1_COMPB_vect){
	if(isNegative == false){
		CLR_SW1;
	}
	else{
		CLR_SW2;
	}
	TCNT0 = 0;
	TIMSK0 |= (1 << OCIE0A);
}