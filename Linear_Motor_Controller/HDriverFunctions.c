#include "includes.h"

#define DEAD_TIME_COUNT_HIGH 208 //First dead time of 26uS
#define DEAD_TIME_COUNT_LOW  24//Second dead time of 3uS
#define MAGNITUDE_DELAY 0.4 //Additional proportional delay for second dead time to account for motor asymmetry

#define TOGGLE_SW1 PORTB ^= (1<<PB0)
#define TOGGLE_SW2 PORTD ^= (1<<PD5)
#define TOGGLE_SW3 PORTD ^= (1<<PD7)
#define TOGGLE_SW4 PORTD ^= (1<<PD6)



void driverTimers_Init(){
 	TCCR0B |= (1<<CS00); //Set up 8bit timer to use 8MHZ clock
 	OCR0A = DEAD_TIME_COUNT_HIGH; //DEAD_TIME_COUNT_HIGH;	
 	OCR0B = DEAD_TIME_COUNT_LOW; //DEAD_TIME_COUNT_LOW;			//Initializing dead times which remain constant		

	TCCR1B |= (1<<CS11);//Set up 16 bit timer with pre-scaler 8
	DDRB = (1 << DDB0); //Configuring driver pins to output
	DDRD = (1 << DDD5) | (1 << DDD6) | (1 << DDD7);
	
	//SET PINS AND VARIABLES TO START STATE to start with positive cycle 
	PORTB |= (1<<PB0);
	PORTD &= ~(1<<PD5);
	PORTD &= ~(1<<PD7);
	PORTD |= (1<<PD6);
	
	isNegativeCycle = false; 
	
}

void setFrequency(float frequency, float dutyCycle){
	float OFFTime = 1000.0/ (frequency*(2+ MAGNITUDE_DELAY + (2*dutyCycle)/(1-dutyCycle))); //Find T_ON, T_OFF1, T_OFF2 from given frequency
 	T_OFF1 = (uint16_t) (round(1000*OFFTime) + ((DEAD_TIME_COUNT_HIGH + DEAD_TIME_COUNT_LOW) *8)); 
 	T_OFF2 = (uint16_t) (round(OFFTime*1000.0 * (1.0+MAGNITUDE_DELAY)) + ((DEAD_TIME_COUNT_HIGH + DEAD_TIME_COUNT_LOW) *8));
	
	T_ON = dutyCycle * T_OFF1/ (1-dutyCycle); //calculating on time from calculated off time
	
	OCR1B = T_ON; //on time constant until setFrequency called
	OCR1A = T_ON + T_OFF1;
}

void driverTimersInterrupts_Init(){
	sei();									//Enable global interrupts	
	TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B);	// Enable on/off time timer compare match interrupts

}

ISR(TIMER0_COMPB_vect){
	TIMSK0 &= ~(1<<OCIE0B);
	TCNT1 = 0; //clear timer 1 count  to start next half cycle
 		
		if (isNegativeCycle){		//Set pins for next half cycle
			TOGGLE_SW1; //sets pins for positive cycle
			} else {
			TOGGLE_SW2;//set pins on for positive cycle
		}
		isNegativeCycle = !isNegativeCycle; //set flag to indicate next half cycle
}

 																		
ISR(TIMER0_COMPA_vect){
	TIMSK0 &= ~(1<<OCIE0A);	// Disable high dead time timer interrupts
	if (isNegativeCycle){
		TOGGLE_SW4; //turn on SW4
	} else {
		TOGGLE_SW3; //turn on SW3
	}									

}

ISR(TIMER1_COMPB_vect){
 	if (isNegativeCycle){		//set to correct timer upper value for each half cycle
 		TOGGLE_SW2; //turn off SW2
		 OCR1A = T_ON + T_OFF2; 
 	} else {
 		TOGGLE_SW1; //TURN OFF SW1
		 OCR1A = T_ON + T_OFF1;
 	}

	TCNT0=0; 													//clear counter to start dead time timer
	TIMSK0 |= (1<<OCIE0A);										// Enable dead time timer compare match A interrupts
}	

	
ISR(TIMER1_COMPA_vect){
	
		if (isNegativeCycle){
 			TOGGLE_SW3; //turns off SW3
 		} else {
 			TOGGLE_SW4; //turns off SW4
 		}
 			TCNT0 = 0 ; //reset timer 0
 			TIMSK0 |= (1<<OCIE0B); //enable low deadtime timer interrupts		
}
