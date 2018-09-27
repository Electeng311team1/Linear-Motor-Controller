#define F_CPU 16000000
#include <util/delay.h>

#define DEAD_TIME_COUNT_1 24 //First deadtime of 3uS
#define DEAD_TIME_COUNT_2 208 //Second deadtime of 26uS

void driverTimers_Init(){
	TCCR0B |= (1<<CS00); //Set up 8bit timer to use 8MHZ clock
	OCR0B = DEAD_TIME_COUNT_1;				//Initialise dead times which remain constant	
	OCR0A = DEAD_TIME_COUNT_2;
	
	TCCR1B |= (1<<CS11);//Set up 16 bit timer with Prescaler 8

}

void clearFallTimeTimer(){ //executed after Pulse on output is turned off
	TCNT0 = 0;
}

void clearOnOffTimeTimer(){ //executed after Pulse on output is turned off
	TCNT0 = 0;
}

void driverTimersInterrupts_Init(){
	sei();																		//Enable global interrupts
	TIMSK0 |= (1<<OCIEA) | (1<<OCIEB);										// Enable dead time timer compare match interrupts
	TIMSK1 |= (1<<OCIEA) | (1<<OCIEB);										// Enable on/off time timer compare match interrupts
	
}

ISR(TIMER0_COMPB){// Set up timer0 compare match ISRs
	//Turn on signal
}	
																		
ISR(TIMER0_COMPA){
	//Turn on signal
}

ISR(TIMER1_COMPB){// Set up timer0 compare match ISRs
	//Turn on signal
}	
																		
ISR(TIMER1_COMPA){
	//Turn on signal
}

void findOnOffTimes(){//Takes in a frequency to drive at and returns timer counter values to implement it  -could store in struct
		
}

void driveHBridge(){
	
	findOnOffTimes()//acquire on/off times
	
	OCR1B = ON_TIME;//set compare values for on and off times
	OCR1A = OFF_TIME_1;
	
	OCR1A = OFF_TIME_2;
	clearOnOffTimeTimer(
}