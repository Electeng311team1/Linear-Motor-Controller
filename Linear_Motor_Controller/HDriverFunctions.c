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
	TCNT1 = 0;
}

void driverTimersInterrupts_Init(){
	sei();																		//Enable global interrupts
	//TIMSK0 |= (1<<OCIEA) | (1<<OCIEB);										// Enable dead time timer compare match interrupts
	TIMSK1 |= (1<<OCIEA) | (1<<OCIEB);										// Enable on/off time timer compare match interrupts
	
}

ISR(TIMER0_COMPB){// Set up timer0 compare match ISRs
	//Switch signals for dead time
	TIMSK0 &=~ (1<<OCIEB);//disable Compare Value B interrupt until next period
}	
																		
ISR(TIMER0_COMPA){
	//Turn on signal
}

ISR(TIMER1_COMPB){// Set up timer0 compare match ISRs
	//Turn signal for off time
	
}	
																		
ISR(TIMER1_COMPA){
	//Turn signal for on time
}

void findOnOffTimes(){//Takes in a frequency to drive at and returns timer counter values to implement it  -could store in struct
		
}

void driveHBridge(){
	
	
	findOnOffTimes();//acquire on/off times or alternatively could manually set on and off times
	
	OCR1B = ON_TIME+ DEAD_TIME_COUNT_1/8;//set compare values for on and off times
	OCR1A = OFF_TIME_1+ON_TIME+ DEAD_TIME_COUNT_1/8;
	//Turn outputs to on
	//Turn outputs off
	clearFallTimeTimer();//start fall time timer
	TIMSK0 |= (1<<OCIEB); //enable fall time interrupt
	
	
	OCR1A = OFF_TIME_2+ON_TIME+ DEAD_TIME_COUNT_2/8;//changing off time accounts for motor assymetry
	
	clearOnOffTimeTimer();
}