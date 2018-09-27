# Linear-Motor-Controller-
Software for a linear motor control system using a atmega328p

Current Features
-Uart transmit works but, transmits too quick. This is fixed using delays but we are suspecting it is to do with clock frequency calibration 

-added uart receive interrupt to work for one character

-Changed F_CPU to 8Mhz, Removed 10ms delay in uart_transmit, Attempted to make receive interrupt work with char array