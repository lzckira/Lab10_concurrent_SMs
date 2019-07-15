/*	Author: zlian030
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
//#ifdef _SIMULATE_
//#include "simAVRHeader.h"
//#endif
volatile unsigned char TimerFlag = 0;
unsigned short TL_Period = 38;
unsigned short BL_Period = 125;
unsigned short currentP = 2;
unsigned short Speaker_Period = 2;
unsigned short Period = 1;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 12;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum TL_States {LED1, LED2, LED3} TL_state;
enum BL_States {ON, OFF} BL_state;
enum Speaker_States {sound, quiet} Speaker_state;
unsigned char tmpTL = 0x00;
unsigned char tmpBL = 0x00;
unsigned char tmpSpeaker = 0x00;
unsigned char count = 0x00;
unsigned char counter = 0x01;
unsigned char flag = 0x00;
void TL_Tick();
void BL_Tick();
void Speaker_Tick();

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port B's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    TL_state = LED1;
    BL_state = OFF;
    Speaker_state = OFF;
    TimerSet(1);
    TimerOn();
    while (1) {
		if (TL_Period >= 380) {
			TL_Tick();
			TL_Period = 0;
		}
		if (BL_Period >= 1250) {
			BL_Tick();
			BL_Period = 0;
		}
		if (Speaker_Period >= currentP) {
			Speaker_Tick();
			Speaker_Period = 0;
		}
		if (PINA == 0xFD && flag == 0x00) { currentP++; flag = 0x01;}
		if (PINA == 0xFF) {flag = 0x00;}
		if (PINA == 0xFB && flag == 0x00 && currentP > 1) { currentP--; flag = 0x01;}
		PORTB = ((tmpSpeaker | tmpBL) | tmpTL);
		TL_Period = TL_Period + Period;
		BL_Period = BL_Period + Period;
		Speaker_Period = Speaker_Period + Period;
		while (!TimerFlag) { }
		TimerFlag = 0;
	}
    return 1;
}


void TL_Tick() {
    switch(TL_state) {
	case LED1:
		TL_state = LED2;
	    tmpTL = 0x01;
	    break;
	case LED2:
		TL_state = LED3;
	    tmpTL = 0x02;
	    break;
	case LED3:
		TL_state = LED1;
	    tmpTL = 0x04;
	    break;
	default:
		TL_state = LED1;
		tmpTL = 0x01;
		break;
		    
	}
}

void BL_Tick() {
    switch(BL_state) {
	case ON:
		BL_state = OFF;
	    tmpBL = 0x08;
	    break;
	case OFF:
		BL_state = ON;
	    tmpBL = 0x00;
	    break;
	default:
		BL_state = OFF;
	    tmpBL = 0x00;
		break;
		    
	}
}

void Speaker_Tick() {
    switch(Speaker_state) {
	case sound:
	    
			Speaker_state = quiet;
		
		tmpSpeaker = 0x10;
	    break;
	case quiet:
		if (PINA == 0xFE) {
			
				Speaker_state = sound;
			
		}
		
	    tmpSpeaker = 0x00;
	    break;
	default:
		Speaker_state = quiet;
	    tmpSpeaker = 0x00;
		break;
		    
	}
}
