#include "includes.h"

#define DEAD_TIME_COUNT_HIGH 208 //First deadtime of 3uS
#define DEAD_TIME_COUNT_LOW  24//Second deadtime of 26uS
//#define MAGNITUDE_DELAY 0.2 //Additional proportional delay for second dead time to account for motor asymmetry

#define TOGGLE_SW1 PORTB ^= (1<<PB0)
#define TOGGLE_SW2 PORTD ^= (1<<PD5)
#define TOGGLE_SW3 PORTD ^= (1<<PD7)
#define TOGGLE_SW4 PORTD ^= (1<<PD6)


//Volatile Global variables
volatile uint16_t T_OFF1;
volatile uint16_t T_OFF2;
volatile uint16_t T_ON;


volatile float MAGNITUDE_DELAY;

void driverTimers_Init(){
 	TCCR0B |= (1<<CS00); //Set up 8bit timer to use 8MHZ clock
 	OCR0A = DEAD_TIME_COUNT_HIGH; //DEAD_TIME_COUNT_HIGH;	
 	OCR0B = DEAD_TIME_COUNT_LOW; //DEAD_TIME_COUNT_LOW;			//Initializing dead times which remain constant		

	TCCR1B |= (1<<CS11);//Set up 16 bit timer with pre-scaler 8
	DDRB = (1 << DDB0); //Configuring driver pins to output
	DDRD = (1 << DDD5) | (1 << DDD6) | (1 << DDD7);
			//CLEAR ALL OUTPUT PINS except for switch 4
	PORTB &= ~(1<<PB0);
	PORTD &= ~(1<<PD5);
	PORTD |= (1<<PD7);
	PORTD &= ~(1<<PD6);
	
	isNegativeCycle = false; 
	
}

void setFrequency(float frequency, float dutyCycle){
// 	MAGNITUDE_DELAY = 0.2; 
// 	double OFFTime = ((1/frequency) - 4*dutyCycle + 200)/ (4 + MAGNITUDE_DELAY); //Find T_ON, T_OFF1, T_OFF2 from given frequency
// 	T_OFF1 = (uint16_t) (round(OFFTime));
// 	T_OFF2 = (uint16_t) (round(OFFTime*(1.0+MAGNITUDE_DELAY)));
// 	T_ON = 2*(dutyCycle-50) + T_OFF1;
	T_ON = 20000;
	OCR1A = 20000+T_ON;
	OCR1B = T_ON;
	//OCR1A = T_ON + DEAD_TIME_COUNT_HIGH*8 + T_OFF1;
}

void driverTimersInterrupts_Init(){
	sei();																		//Enable global interrupts	
	TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B);	// Enable on/off time timer compare match interrupts
							
	TOGGLE_SW2;
}

  ISR(TIMER0_COMPB_vect){
	TIMSK0 &= ~(1<<OCIE0B);
	TOGGLE_SW2;
	_delay_ms(2);
	TOGGLE_SW2;
	
// 	 //TOGGLE_SW2;
// 	// OCR1A = T_ON;
//  	TIMSK0 &=~ (1<<OCIE0B);	//disable low dead time interrupt
//  		//isNegativeCycle = ~isNegativeCycle; //switching to next half cycle
// // 		
// // 		if (isNegativeCycle){		//Set pins for next half cycle
// // 			TOGGLE_SW2; //sets pins for positive cycle
// // 			} else {
// // 			TOGGLE_SW1;//set pins on for positive cycle
// // 		}
// // 		
// // 		TCNT1 = 0; //Reset on/off timer for second half of cycle
// // 
  }

 																		
// ISR(TIMER0_COMPA_vect){
// 	TIMSK0 &=~ (1<<OCIE0A);	
// 	if (isNegativeCycle){
// 		TOGGLE_SW4;
// 	} else {
// 		TOGGLE_SW3; 
// 	}									// Disable high dead time timer interrupts
// 
// }

ISR(TIMER1_COMPB_vect){// Set up timer0 compare match ISRs
	TOGGLE_SW2;
	TIMSK0 |= (1<<OCIE0B);
// 	if (isNegativeCycle){		//set timer 1 on/off timer compare value to correct value 
// 		TOGGLE_SW2; //turn off SW2
// 		_delay_us(500);
// 		//OCR1A = T_ON + (DEAD_TIME_COUNT_HIGH + DEAD_TIME_COUNT_LOW) *8 + T_OFF1;
// 	} else {
// 		//OCR1A = T_ON + (DEAD_TIME_COUNT_HIGH + DEAD_TIME_COUNT_LOW) *8 + T_OFF2;
// 		TOGGLE_SW1; //TURN OFF SWITCH 1
// 		_delay_us(500);
// 	}
// 	
	
	
	//TCNT0=0; 													//clear counter to start dead time timer
	//TIMSK0 |= (1<<OCIE0A);										// Enable dead time timer compare match A interrupts
}	

	
ISR(TIMER1_COMPA_vect){
// 		if (isNegativeCycle){
// 			TOGGLE_SW3;
// 			_delay_us(500);
// 		} else {
// 			TOGGLE_SW4; //turns off switch 4
// 			_delay_us(500);
// 			TOGGLE_SW2;
// 		}
// 		//	TCNT0 = 0 ; //reset timer 0
// 		//	TIMSK0 = (1<<OCIE0B); //enable low deadtime timer interrupts
 		
			TOGGLE_SW2;
			TCNT1 = 0; 
			isNegativeCycle = !isNegativeCycle;
			
}