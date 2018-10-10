/*
 * Linear_Motor_Controller.c
 *
 * Created: 26/09/2018 
 *	Author: Team 1
 */ 

 /*Include files*/
#include "includes.h"
#include "mjson.h"

/*Main defines*/
#define BAUD_RATE 9600
#define UBRR_VALUE F_CPU/16/BAUD_RATE-1
#define VERSION "1.5.1"

volatile char received_message[MAX_SIZE];
volatile uint8_t message_index = 0;
volatile uint8_t net_brackets = 0;
volatile uint8_t message_complete = false;
volatile uint8_t receive_error = false;
volatile uint8_t message_start = false;

float frequency_value;
uint8_t mfc_value;

//ISR for UART receive
ISR(USART_RX_vect){
	char tmp = UDR0; 
	if(message_complete == false){
		if(tmp == '{'){
			net_brackets++;
			message_start = true;
		}
		else if((tmp == '}') && (message_start == true)){
			net_brackets--;
			if(net_brackets == 0){
				message_complete = true;
			}
		}

		if(message_start == true){
			received_message[message_index] = tmp;
			message_index++;
			if(message_complete == true){
				received_message[message_index] = '\0';
			}
		}

		if((message_index == (MAX_SIZE-1)) || (net_brackets > 3)){
			receive_error = true;
			message_start = false;
			message_complete = false;
			message_index = 0;
		}
	}
}

int main(void)
{
	uint8_t* mfc = &mfc_value;
	float* frequency = &frequency_value;
	
	//UART functionalities 
	uart_initiate(UBRR_VALUE);
	char req_mfc_str[4] = "XXX";
	char mfc_str[4] = "XXX";
	char pwr_str[4] = "XXX";
	char freq_str[4] = "XXX";
	char curr_str[4] = "XXX";
	char volt_str[4] = "XXX";
	char sith_str[4] = "\0";

	//Enable Global interrupts
	sei();

	//Initiation for the project skywalker
	force = false;
		
	//Setting default values
	*frequency = 12.5;
	*mfc = 150;

	//enable timers
	driver_timer_initiate();
	set_parameters(*frequency, *mfc);
	//soft_start(*frequency, *mfc);

	//ADC initiation 
	adc_initiate();

	//Resonance checking flag
	check_resonance = false;
	resonance_counter = 0;

    while (1){

		if(message_complete == true){
			UCSR0B &= ~(1 << RXEN0);
			uart_transmit((char*)received_message);
			itoa(*mfc,mfc_str,10);
			process_message((char*)received_message, mfc);
			itoa(*mfc,req_mfc_str,10);
			set_parameters(*frequency, *mfc);
			uart_transmit("\n\r");
			message_complete = false;
			message_start = false;
			message_index = 0;
			UCSR0B |= (1 << RXEN0);
		}

		if(message_complete == true){
			char *transmit_message = NULL;
			struct mjson_out out = MJSON_OUT_DYNAMIC_BUF(&transmit_message);
			mjson_printf(&out,
			"{%Q: {%Q: {%Q: %Q, %Q: %Q}, %Q: %Q, %Q: {%Q: \"%s%s\", %Q: \"%s%s\", %Q: \"%s%s\", %Q: \"%s%s\"}, %Q: { %Q: %Q}}}",
			"3", "mfc","req",req_mfc_str, "cur", mfc_str, "ver", VERSION, "param", "pwr", pwr_str, "W", "freq", freq_str, "Hz", "curr", curr_str, "A", "volt", volt_str, "V", "user", "force", sith_str);
		
			uart_transmit("\n\r");
			uart_transmit((char *)transmit_message);
			free(transmit_message);
		}
			
		if(force){
			project_skywalker();
			force = false;
			strcpy(sith_str,"off");
			set_parameters(*frequency, *mfc);
		}

		if(check_resonance && (*mfc != 0)){
			calculate_resonance(frequency, *mfc);
			check_resonance = false;
		}



	}
}


