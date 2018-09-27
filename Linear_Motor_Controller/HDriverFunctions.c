#define F_CPU 16000000
#include <util/delay.h>

#define DEAD_TIME_COUNT_1 208 //First deadtime of 3uS
#define DEAD_TIME_COUNT_2  24//Second deadtime of 26uS

//Volatile Global variables
//T_OFF1
//T_OFF2
//T_ON
bool isNegativeCycle = false;

void driverTimers_Init(){
	TCCR0B |= (1<<CS00); //Set up 8bit timer to use 8MHZ clock
	OCR0A = DEAD_TIME_COUNT_1;				//Initialise dead times which remain constant		
	TCCR1B |= (1<<CS11);//Set up 16 bit timer with Prescaler 8
	OCR1B = T_ON;
}

void setFrequency(){
	//Find T_ON, T_OFF1, T_OFF2 from given frequency
	OCR1B = T_ON;
	OCR1A = T_ON + DEAD_TIME_COUNT_1*8 + T_OFF1;
}

void driverTimersInterrupts_Init(){
	sei();																		//Enable global interrupts	
	TIMSK1 |= (1<<OCIEA) | (1<<OCIEB);										// Enable on/off time timer compare match interrupts
	
}
																		
ISR(TIMER0_COMPA){
	TIMSK0 &=~ (1<<OCIEA);										// Disable dead time timer compare match A interrupts
//Turn signals off for off time
}

ISR(TIMER1_COMPB){// Set up timer0 compare match ISRs
	if (isNegativeCycle){		//set timer 1 on/off timer compare value to correct value 
		OCR1A = T_ON + DEAD_TIME_COUNT_2*8 + T_OFF1;
	} else {
		OCR1A = T_ON + DEAD_TIME_COUNT_1*8 + T_OFF2;
	}
	
	//Turn pins off for dead time
	
	TCNT0=0; 													//clear counter to start dead time timer
	TIMSK0 |= (1<<OCIEA);										// Enable dead time timer compare match A interrupts
}	

	
ISR(TIMER1_COMPA){
	isNegativeCycle = ~isNegativeCycle; //switching to next half cycle
	if (isNegativeCycle){		//Set pins on for next half cycle
		//set pins on for -ve cycle
		OCR0A = DEAD_TIME_COUNT_2;	
	} else {
		//set pins on for +ve cycle
		OCR0A = DEAD_TIME_COUNT_1; 
	} 
	
	TCNT1 = 0; //Reset on/off timer for second half of cycle
}

void driveHBridge(){
	
	setFrequency();//acquire on/off times or alternatively could manually set on and off times
	driverTimers_Init();
	driverTimersInterrupts_Init();

	while(1){
		//other operations
	}

}