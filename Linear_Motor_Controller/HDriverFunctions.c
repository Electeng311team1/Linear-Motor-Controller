#include "includes.h"

#define DEAD_TIME_COUNT_HIGH 208 //First deadtime of 3uS
#define DEAD_TIME_COUNT_LOW  24//Second deadtime of 26uS
#define LOW_OFF_TIME 3
#define	HIGH_OFF_TIME 26
#define MAGNITUDE_DELAY 0.0 //Additional proportional delay for second dead time to account for motor asymmetry

#define TOGGLE_SW1 PORTB ^= (1<<PB0)
#define TOGGLE_SW2 PORTD ^= (1<<PD5)
#define TOGGLE_SW3 PORTD ^= (1<<PD7)
#define TOGGLE_SW4 PORTD ^= (1<<PD6)

#define SET_SW1 PORTB |= (1<<PB0)
#define SET_SW2 PORTD |= (1<<PD5)
#define SET_SW3 PORTD |= (1<<PD7)
#define SET_SW4 PORTD |= (1<<PD6)

#define CLEAR_SW1 PORTB &= ~(1<<PB0)
#define CLEAR_SW2 PORTD &= ~(1<<PD5)
#define CLEAR_SW3 PORTD &= ~(1<<PD7)
#define CLEAR_SW4 PORTD &= ~(1<<PD6)

void driverTimers_Init(){
 	TCCR2B |= (1<<CS20); //Set up 8bit timer to use 8MHZ clock
 	OCR2A = DEAD_TIME_COUNT_HIGH; //DEAD_TIME_COUNT_HIGH;	
 	OCR2B = DEAD_TIME_COUNT_LOW; //DEAD_TIME_COUNT_LOW;			//Initializing dead times which remain constant		

	TCCR1B |= (1<<CS11);//Set up 16 bit timer with pre-scaler 8
	DDRB |= (1 << DDB0); //Configuring driver pins to output
	DDRD |= (1 << DDD5) | (1 << DDD6) | (1 << DDD7);
	
	//SET PINS AND VARIABLES TO START STATE to start with positive cycle 
	CLEAR_SW1 ;
	CLEAR_SW2 ;
	CLEAR_SW3 ;
	CLEAR_SW4 ;
	
	isNegativeCycle = false; 
	
}

void setFrequency(float frequency, float duty_Cycle){

		//driverDisable = 0;
		/*PORTB |= (1<<PB0);*/
		//PORTD |= (1<<PD6);
		
		
// 		float OFFTime = 1000.0/ (frequency*(2+ MAGNITUDE_DELAY + (2*dutyCycle)/(1-dutyCycle))); //Find T_ON, T_OFF1, T_OFF2 from given frequency
//  		T_OFF1 = (uint16_t) (round(1000*OFFTime) + ((DEAD_TIME_COUNT_HIGH + DEAD_TIME_COUNT_LOW) *8)); 
//  		T_OFF2 = (uint16_t) (round(OFFTime*1000.0 * (1.0+MAGNITUDE_DELAY)) + ((DEAD_TIME_COUNT_HIGH + DEAD_TIME_COUNT_LOW) *8));
// 	
// 		T_ON = dutyCycle * T_OFF1/ (1-dutyCycle); //calculating on time from calculated off time
	 
// 		OCR1B = T_ON; //on time constant until setFrequency called
// 		OCR1A = T_ON + T_OFF1; 
	//float dutyCycle = *mfc/255;

	float off_time = ((1000/(2*frequency)) - (LOW_OFF_TIME+HIGH_OFF_TIME)/1000)*(1-duty_Cycle);
	float on_time = ((1000/(2*frequency)) - (LOW_OFF_TIME+HIGH_OFF_TIME)/1000)*(duty_Cycle);
	OCR1A = (uint16_t) ((on_time + off_time + HIGH_OFF_TIME/1000)*1000);
	OCR1B = (uint16_t) (on_time*1000);
		
		if (duty_Cycle == 0){
			stopDriver=1;
		} else {
			stopDriver=0;
			TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B);	// Enable on/off time timer compare match interrupts
		}
// 		
// 		if (stopDriver==0){
// 			
// 		} 
		

	
}

void driverTimersInterrupts_Init(){

										//Enable global interrupts	
		


}

  ISR(TIMER2_COMPB_vect){

		TIMSK2 &= ~(1<<OCIE2B);
		TCNT1 = 0; //clear timer 1 count  to start next half cycle
 		if (stopDriver==0){
			 if (isNegativeCycle){		//Set pins for next half cycle
				SET_SW1; //sets pins for positive cycle
			} else {
				SET_SW2;//set pins on for positive cycle
			}
		 
		 }
		
		isNegativeCycle = !isNegativeCycle; //set flag to indicate next half cycle
	  
	
  }

 																		
ISR(TIMER2_COMPA_vect){

		 TIMSK2 &= ~(1<<OCIE2A);	// Disable high dead time timer interrupts
		 
		 if (stopDriver==0){
		 	if (isNegativeCycle){
				SET_SW4; //turn on SW4
			} else {
				SET_SW3; //turn on SW3
			}
		 
		 }

	 
	
}
									



ISR(TIMER1_COMPB_vect){
	
		  if (isNegativeCycle){		//set to correct timer upper value for each half cycle
 		CLEAR_SW2; //turn off SW2
		 //OCR1A = T_ON + T_OFF2; 
 	} else {
 		CLEAR_SW1; //TURN OFF SW1
		 //OCR1A = T_ON + T_OFF1;
 	}

	TCNT2=0; 													//clear counter to start dead time timer
	TIMSK2 |= (1<<OCIE2A);			
	  
 								// Enable dead time timer compare match A interrupts
}	

	
ISR(TIMER1_COMPA_vect){
	 
		  if (isNegativeCycle){
 			CLEAR_SW3; //turns off SW3
 		} else {
 			CLEAR_SW4; //turns off SW4
 		}
 			TCNT2 = 0 ; //reset timer 0
 			TIMSK2 = (1<<OCIE2B); //enable low deadtime timer interrupts		


}
