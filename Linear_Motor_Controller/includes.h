/*
 * includes.h
 *
 * Created: 26/09/2018 
 *  Author: Team 1 
 */ 

#define F_CPU 8000000


/*Include files*/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h> //Remove in final version
#include <stdbool.h>

/*UART Defines*/
//Check if transmission has been received
#define RECEIVE_COMPLETE (UCSR0A & (1 << RXC0))
#define UDR_FULL !(UCSR0A & (1 << UDRE0))
#define MAX_SIZE 150

/*ADC Defines*/
#define SAMPLING_SIZE 150

/*Motor Driver Defines*/
#define SET_SW1 PORTB |= (1 << PB0)
#define CLR_SW1 PORTB &= ~(1 << PB0)
#define SET_SW2 PORTD |= (1 << PD5)
#define CLR_SW2 PORTD &= ~(1 << PD5)
#define SET_SW3 PORTD |= (1 << PD7)
#define CLR_SW3 PORTD &= ~(1 << PD7)
#define SET_SW4 PORTD |= (1 << PD6)
#define CLR_SW4 PORTD &= ~(1 << PD6)

//How often resonant frequency is checked
#define CYCLES 20

//Dead times in us
#define LOW_OFF_TIME 2
#define HIGH_OFF_TIME 8
//Timer 2 dead time compare values
#define LOW_OFF_TIME_COUNT_VALUE (LOW_OFF_TIME*8)
#define HIGH_OFF_TIME_COUNT_VALUE (HIGH_OFF_TIME*8)

/*UART Functions*/
void uart_initiate(uint16_t UBRR_VALUE);
void uart_transmit(char* message);
void process_message(char* message, uint8_t* mfc);

/*Motor Driver Function*/
void driver_timer_initiate(void);
void set_parameters(float frequency,  uint8_t mfc);
void soft_start(float req_freq, int req_mfc);
void stop_motor();

/*Motor Driver Variables*/
volatile bool isNegative;
volatile bool change_duty;
volatile bool first_cycle;
volatile uint16_t t1_compare_a;
volatile uint16_t t1_compare_b;
volatile int mfr;

/*Resonance Functions*/
void calculate_resonance(float* freq, uint8_t mfc);

/*Resonance checking variables*/
volatile bool check_resonance;
volatile uint8_t resonance_counter;
volatile bool stop_driving; 

/*ADC Functions*/
void adc_initiate();
void adc_start(float* operating_frequency);

/*ADC variables*/
volatile uint8_t compA_count;
volatile uint16_t current_values[SAMPLING_SIZE];
volatile uint8_t current_index;
volatile float voltage_values[SAMPLING_SIZE/10];
volatile uint8_t voltage_index;

/*Other Functions*/
//#define SITH
void project_skywalker();
volatile bool force;

/*Project Skywalker defines*/
#define AFLATLOW 208
#define B 246
#define CLOW 260
#define D 292
#define EFLAT 312
#define E 328
#define F 348
#define G 393
#define AFLAT 416
#define A 440
#define BFLAT 466
#define CHIGH 522
#define x 150


