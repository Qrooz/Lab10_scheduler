/*	Author: Cruz Ramirez
 *  	Partner(s) Name: 
 *	Lab Section:22
 *	Assignment: Lab 10  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	Light on B7 will light up if any keypad button is pressed.
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL> N/A
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "bit.h"
#include "timer.h"
#endif

//Get keypad function
unsigned char GetKeypadKey(){
	
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


//enumeration of states of first SM
enum KEY_States{KEY_wait, KEY_press };

int TickFct_KEY(int state) {

	unsigned char x = GetKeypadKey();

	switch (state) {//state machine tranistion
		case KEY_wait:
			if(x != '\0'){
				state = KEY_press;
			}
			else{
				state = KEY_wait;
			}
		break;

		case KEY_press:
			if(x == '\0'){
				state = KEY_wait;
			}
			else{
				state = KEY_press;
			}
		break;

		default: state = KEY_wait; break;
	}
	switch(state) {//state machine actions
		case KEY_wait:
			PORTB = 0x00;
		break;

		case KEY_press:
			PORTB = 0x80;
		break;
		
		default: break;
	}
	return state;
}




int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */
    
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	const char start = -1;
	//task1
	task1.state = start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &TickFct_KEY;

	TimerSet(100);
	TimerOn();

	unsigned short i;
	while (1) {
		for(i = 0; i < numTasks; i++){
			
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 100;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
return 0;
}
