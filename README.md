# Linear-Motor-Controller-
Software for a linear motor control system using a atmega328p

Current Features
-Uart transmit works but, transmits too quick. This is fixed using delays but we are suspecting it is to do with clock frequency calibration 

-added uart receive interrupt to work for one character

-2/10/2018 added uart receive interupt functionality to work with a message
    -errors when message may not be formatted correctly
        -characters following a completed message are causing corruption of message
        -terminal is receiving what we are transmitting as well as what the microcontroller is sending
        
-6/10/2018 added the ability to control the motor under given conditions