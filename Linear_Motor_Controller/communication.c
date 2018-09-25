/*
 * communication.c
 *
 * Created: 1/09/2018
 *  Author: Team 1
 */ 

#include "includes.h"

//Determining if the transmission buffer is empty
#define UDR_FULL !(UCSR0A & (1<<UDRE0))

//Determining if the transmission has been received 
#define receive_complete (UCSR0A & (1<<RXC0))

void initialise_uart(uint16_t UBRR_VALUE){
	//Setting the UBRR value
	UBRR0L = UBRR_VALUE;
	UBRR0H = (UBRR_VALUE>>8);
	
	//Setting the Transmitter and Receiver Enable Bits
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);

	//Enable UART receive interrupt
	UCSR0B = (1<RXCIE0);

	//Setting the Character size to 8 bits
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_transmit(char character){
	//Check if the transfer buffer is empty 
	while(UDR_FULL);
	//Transfer the character to the transfer buffer
	UDR0 = character;
}

void transmit_message(char* message){
	//Determine the length of the message
	uint16_t message_length = strlen(message);
	
	//Transmit each separate character of the message
	for(uint16_t i = 0; i < message_length; i++){
		uart_transmit(message[i]);
	}
}

//Clean out the contents of the receive buffer
void uart_flush(){
	UCSR0B &= ~(1<<RXEN0);
	UCSR0B |= (1<<RXEN0);
}

unsigned char uart_receive(){
	uart_flush();
	while(!receive_complete);
	return UDR0;
}

// void receive_message(char* received_message){
// 	unsigned char received_character;
// 	char* store = malloc(sizeof(char));
// 	while(received_character != '}'){
// 		received_character = uart_receive(); //Get the new character we've received
// 		*store = received_character; //Store the character in the string
// 		strcat(received_message,store);
// 	}
// }

#define MAX_LENGTH 20
char* receive_message(){
	static char received_message[MAX_LENGTH];
	unsigned char end_character = '}'; //Assign desired end character
	unsigned char received_character;
	uint16_t i = 0; //Used to loop through string
	while(i<MAX_LENGTH){
		received_character = uart_receive(); //Get the new character we've received
		if (received_character == end_character) { //If its the end of the string, exit the loop
			i = MAX_LENGTH;
		}
		received_message[i] = received_character; //Store the character in the string
		i++;
	}
	return received_message; //Return the pointer
}

void append(char *s, unsigned char character){
	int length = strlen(s);
	s[length] = character;
	s[length+1] = '\0';
}