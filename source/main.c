/*	Author: Cruz Ramirez
 *  Partner(s) Name: 
 *	Lab Section:22
 *	Assignment: Lab 10  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	Who knows
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//Get keypad function
unsigned char GetKeyPadKey(){
	
	//check keys in column 1
	PORTC = 0xEF;
	asm("nop");
	if (GetBit(PINC,0)==0){return('1');}
	if (GetBit(PINC,1)==0){return('4');}
	if (GetBit(PINC,2)==0){return('7');}
	if (GetBit(PINC,3)==0){return('*');}

	//check keys in column 2
	PORTC = 0xDF;
	asm("nop");
	if (GetBit(PINC,0)==0){return('2');}
        if (GetBit(PINC,1)==0){return('5');}
        if (GetBit(PINC,2)==0){return('8');}
        if (GetBit(PINC,3)==0){return('0');}

	//check keys in column 3
	PORTC = 0xBF;
	asm("nop");
	if (GetBit(PINC,0)==0){return('3');}
        if (GetBit(PINC,1)==0){return('6');}
        if (GetBit(PINC,2)==0){return('9');}
        if (GetBit(PINC,3)==0){return('#');}

	//check keys in column 4
	PORTC = 0x7F;
	asm("nop");
	if (GetBit(PINC,0)==0){return('A');}
        if (GetBit(PINC,1)==0){return('B');}
        if (GetBit(PINC,2)==0){return('C');}
        if (GetBit(PINC,3)==0){return('D');}

	return ('\0'); //default value
}

//task scheduler data structure
typedef struct _task {
	
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;
//end of task scheduler structure

//shared variables
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;
//end of shared variables

//enumeration of states of first SM
enum pauseButtonSM_States{pauseButton_wait, pauseButton_press, pauseButton_release};

int pauseButtonSMTick(int state) {

	unsigned char press = ~PINA & 0x01;

	switch (state) {//state machine tranistion
	
		case pauseButton_wait:
			state = press == 0x01? pauseButton_press: pauseButton_wait; break;
		case pauseButton_press:
			state = pauseButton_release; break;
		case pauseButton_release:
			state = press == 0x00? pauseButton_wait: pauseButton_press; break;
		default: state = pauseButton_wait; break;
	}
	switch(state) {//state machine actions
		case pauseButton_wait : break;
		case pauseButton_press :
			pause = (pause == 0) ? 1 : 0; //toggle pause
			break;
		case pauseButton_release: break;
	}
	return state;
}


//enumeration of toggle led0 states
enum toggleLED0_States { toggleLED0_wait, toggleLED0_blink };

//if paused DO NOT toggle LED connected to PB0
//if unpaused toggle LED connected to PB0
int toggleLED0SMTick(int state){

	switch (state) {//state machine transitions 
		case toggleLED0_wait: state = !pause? toggleLED0_blink: toggleLED0_wait; break;
		case toggleLED0_blink: state = pause? toggleLED0_wait: toggleLED0_blink; break;
		default: state = toggleLED0_wait; break;
	}
	switch (state) {//state machine 
		case toggleLED0_wait: break;
		case toggleLED0_blink:
			led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
			break;
	}
	return state;
}

//enumeration of toggle LED1
enum toggleLED1_States {toggleLED1_wait, toggleLED1_blink};

//if paused DO NOT toggle LED connect to PB1
//if unpaused toggle LED connected to PB1
int toggleLED1SMTick(int state) {
	switch (state) { //State machine transitions
		case toggleLED1_wait: state = !pause? toggleLED1_blink: toggleLED1_wait; break;
		case toggleLED1_blink: state = pause? toggleLED1_wait: toggleLED1_blink; break;
		default: state = toggleLED1_wait; break;
	}
	switch (state) { //state machine actions
		case toggleLED1_wait: break;
		case toggleLED1_blink:
			led1_output = (led1_output == 0x00) ? 0x01 : 0x00;
			break;

	}
	return state;
}

//enumeration of display states
enum display_States { display_display };

int displaySMTick(int state) {
	
	unsigned char output;

	switch (state) { //state machine transitions
		case display_display: state = display_display; break;
		default: state = display_display; break;
	}
	switch (state) { //state machine actions
		case display_display:
			output = led0_output | led1_output << 1; //write shared outputs to local variables
		break;
	}
	PORTB = output; //write combined, shared output variables to PORTB
	return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	unsigned char x;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */
    while (1) {
	x = GetKeypadKey();
	switch(x){
		case '\0': PORTB = 0x1F; break; //all 5 leds on
		case '1' : PORTB = 0x01; break; //hex equivalent
		case '2' : PORTB = 0x02; break;
		case '3' : PORTB = 0x03; break; 
		case '4' : PORTB = 0x04; break;
		case '5' : PORTB = 0x05; break;
		case '6' : PORTB = 0x06; break;
		case '7' : PORTB = 0x07; break;
		case '8' : PORTB = 0x08; break;
		case '9' : PORTB = 0x09; break;
		case 'A' : PORTB = 0x0A; break;
		case 'B' : PORTB = 0x0B; break;
		case 'C' : PORTB = 0x0C; break;
		case 'D' : PORTB = 0x0D; break;
		case '*' : PORTB = 0x0E; break;
		case '0' : PORTB = 0x00; break;
		case '#' : PORTB = 0x0F; break;
		default: PORTB = 0x1B; break; //should never occur. Middle LED off.
	}
    }
    return 0;
}
