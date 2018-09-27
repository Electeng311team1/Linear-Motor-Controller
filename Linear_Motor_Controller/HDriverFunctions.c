#define F_CPU 16000000
#include <util/delay.h>

#define DEAD_TIME_COUNT_1 24 //First deadtime of 3uS
#define DEAD_TIME_COUNT_2 208 //Second deadtime of 26uS

//Volatile Global variables
//T_OFF1
//T_OFF2
//T_ON

void driverTimers_Init(){
	TCCR0B |= (1<<CS00); //Set up 8bit timer to use 8MHZ clock
	OCR0B = DEAD_TIME_COUNT_1;				//Initialise dead times which remain constant	
	OCR0A = DEAD_TIME_COUNT_2;
	
	TCCR1B |= (1<<CS11);//Set up 16 bit timer with Prescaler 8
}

void setFrequency(){
	//Find T_ON, T_OFF1, T_OFF2 from given frequency
	OCR1B = T_ON;
	OCR1A = T_OFF1;
}

void driverTimersInterrupts_Init(){
	sei();																		//Enable global interrupts
	TIMSK0 |= (1<<OCIEA);										// Enable dead time timer compare match A only interrupts
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

void driveHBridge(){
	
	setFrequency();//acquire on/off times or alternatively could manually set on and off times
	driverTimers_Init();
	driverTimersInterrupts_Init();
	
	OCR1B = ON_TIME;//set compare values for on/off time first half
	OCR1A = OFF_TIME_1;
	
	while(1){
		//other operations
	}

}