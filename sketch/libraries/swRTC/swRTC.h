/*
	swRTC.h - A software implementation of a Real-Time Clock (RTC) using
	the timer2 of Arduino or the Real-Time Counter module, integrated in
	several Atmel Microcontrollers.

	Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
	with contribution of lesto <www.arduino.cc>
	Inspired by MsTimer2 by Javier Valencia

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 3.0 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef swRTC_H
#define swRTC_H

//library version
#define swRTC_VERSION 128

//Library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//include required libraries
#include <avr/interrupt.h>

//check if the micro is supported
#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega48P__) || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega168P__) || defined (__AVR_ATmega328P__)
#define ATMEGAx8
#elif defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
#define ATTINYx5
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
#define ATMEGA8
#elif defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny84__)
#define ATTINYx4
#elif defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)
#define ATMEGAx0
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1284P__)
#define ATMEGA644
#elif defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny4313__)
#define ATTINYx313
#elif defined (__AVR_ATmega32U4__)
#define ATMEGAxU
#if (F_CPU!=16000000UL)
#error Sorry, Atmega32U4 is supported only at 16 MHz
#endif
#else
#error Sorry, microcontroller not supported!
#endif

//RTC module is preset in: ATMEGA8, ATMEGAX4, ATMEGAx8, ATMEGAx0
//RTC is NOT in: ATTINYx4, ATTINYx5, ATTINYx313, ATMEGA32u4
#if defined (USE_INTERNAL_RTC)
#if defined (ATTINYx4) || defined (ATTINYx313) || defined (ATTINYx5) || defined(ATMEGAxU)
#error This MCU does NOT have the RTC module
#endif
#endif

//check if the frequency is supported
#if ((F_CPU!=1000000UL) && (F_CPU!=4000000UL) && (F_CPU!=8000000UL) && (F_CPU!=16000000UL))
#error Sorry, clock frequency not supported!
#endif

class swRTC {
	public:
		//public methods
		swRTC();
		boolean setTime(byte hourT, byte minuteT, byte secondT);
		boolean setDate(byte dayT, byte monthT, int yearT);
		void startRTC();
		void stopRTC();
		boolean isLeapYear(int yearT = 0);
		unsigned long getTimestamp(int yearT = 0);
		boolean setDeltaT(double deltaT = 0.0) __attribute__ ((deprecated));
        boolean setDeltaT(int deltaT = 0);
        int getDeltaT();
		byte getSeconds();
		byte getMinutes();
		byte getHours();
		byte getDay();
		byte getMonth();
		int getYear();
		byte setClockWithTimestamp(unsigned long timeT, int yearT = 0);
		byte weekDay(unsigned long timestamp);
		byte getWeekDay();

	private:
		//private methods
		void setTimer();
		//private variables
		boolean isRunning;
		int yearSize(int yearT);
};


// ****** MAIN FUNCTIONS *******

//global variables
swRTC *lib;
volatile int delta = 0;
volatile long deltaS = 0;
volatile int8_t deltaDir = 0;
#if defined(ATMEGAxU)
volatile unsigned int starter = 0;
#else
volatile byte starter = 0;
#endif
volatile int counterT = 0;
volatile byte hours = 0;
volatile byte minutes = 0;
volatile byte secondsX = 0;
volatile byte day = 0;
volatile byte month = 0;
volatile int year = 0;
volatile byte daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//instructions to execute when a new istance of the lib is created
swRTC::swRTC(void) {
	isRunning = false;
	lib = this;
	setTimer();
	counterT = 0;
	delta = 0;
}


//
//private methods
//

/*
    *****  WARNING  *****
    DO NOT MODIFY THE FOLLOWING FUNCTIONS BETWEEN HERE......
*/
//set the timer
void swRTC::setTimer() {
	float prescaler = 0.0;

#if defined (ATMEGAx8) || defined (ATMEGA644) || defined (ATMEGAx0)
	//during setup, disable all the interrupts based on timer
	TIMSK2 &= ~(1<<TOIE2);
	TIMSK2 &= ~((1<<OCIE2A) | (1<<OCIE2B));

//set the timer for asynchronous or synchronous mode
#if defined (USE_INTERNAL_RTC)
	//source clock set to external crystal (asynch mode)
	ASSR |= (1<<AS2);
	//prescaler set to 128 - with a 32,768 Hz crystal we get 1 overflow every 1 second
	//because timer 2 is an 8-bit timer, so: 32768/128/256 = 1 Hz
	TCCR2B |= ((1<<CS22) | (1<<CS20));
	TCCR2B &= ~(1<<CS21);
	//starting value of the counter
	TCNT2 = 0;
#else
	//prescaler source clock set to internal Atmega clock (synch mode)
	ASSR &= ~(1<<AS2);
	//the following code sets the prescaler depending on the system clock
	if (F_CPU == 16000000UL) {   // prescaler set to 64
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if ((F_CPU == 8000000UL) || (F_CPU == 4000000UL)) { // prescaler set to 32
		TCCR2B &= ~(1<<CS22);
		TCCR2B |= ((1<<CS21) | (1<<CS20));
		prescaler = 32.0;
	} else if (F_CPU == 1000000UL) { // prescaler set to 8
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		TCCR2B |= (1<<CS21);
		prescaler = 8.0;
	}
#endif
	//CTC (counter) mode: the timer increments the counter until overflow
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);

#elif defined (ATTINYx5) || defined (ATTINYx313)
	//during setup, disable all the interrupts based on timer 0
	TIMSK &= ~(1<<TOIE0);
	TIMSK &= ~((1<<OCIE0A) | (1<<OCIE0B));
	//normal mode: counter not connected to external pins
	TCCR0A &= ~((1<<COM0A0) | (1<<COM0A1));
	//this sets the timer to increment the counter until overflow
	TCCR0A &= ~((1<<WGM01) | (1<<WGM00));
	TCCR0B &= ~(1<<WGM02);
	//the following code sets the prescaler depending on the system clock
	if ((F_CPU == 16000000UL) || (F_CPU == 8000000UL)) {   // prescaler set to 64
		TCCR0B &= ~(1<<CS02);
		TCCR0B |= ((1<<CS01) | (1<<CS00));
		prescaler = 64.0;
	} else if (F_CPU == 1000000UL) { // prescaler set to 8
		TCCR0B &= ~((1<<CS02) | (1<<CS00));
		TCCR0B |= (1<<CS01);
		prescaler = 8.0;
	}
#elif defined (ATTINYx4)
	//on Attinyx4 we must use the timer 0 because timer1 is a 16 bit counter

	//during setup, disable all the interrupts based on timer 0
	TIMSK0 &= ~(1<<TOIE0);
	TIMSK0 &= ~((1<<OCIE0A) | (1<<OCIE0B));
	//normal mode: increment counter until overflow
	TCCR0B &= ~(1<<WGM02);
	TCCR0A &= ~((1<<WGM01) | (1<<WGM00));
	//normal mode: disconnect timer from pins
	TCCR0A &= ~((1<<COM0A0) | (1<<COM0A1));
	//the following code sets the prescaler depending on the system clock
	if ((F_CPU == 16000000UL) || (F_CPU == 8000000UL)) {   // prescaler set to 64
		TCCR0B &= ~(1<<CS02);
		TCCR0B |= ((1<<CS01) | (1<<CS00));
		prescaler = 64.0;
	} else if (F_CPU == 1000000UL) { // prescaler set to 8
		TCCR0B &= ~((1<<CS02) | (1<<CS00));
		TCCR0B |= (1<<CS01);
		prescaler = 8.0;
	}
#elif defined (ATMEGA8)
	//during setup, disable all the interrupts based on timer1
	TIMSK &= ~(1<<TOIE2);
	TIMSK &= ~(1<<OCIE2);
	//normal mode: counter incremented until overflow
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));

//set the timer for asynchronous or synchronous mode
#if defined (USE_INTERNAL_RTC)
	//source clock set to external crystal (asynch mode)
	ASSR |= (1<<AS2);
	//prescaler set to 128 - with a 32,768 Hz crystal we get 1 overflow every 1 second
	//because timer 2 is an 8-bit timer, so: 32768/128/256 = 1 Hz
    TCCR2 |= ((1<<CS22) | (1<<CS20));
    TCCR2 &= ~(1<<CS21);
    //starting value of the counter
    TCNT2 = 0;
#else
	//prescaler source clock set to internal Atmega clock (synch mode)
	ASSR &= ~(1<<AS2);
	//the following code sets the prescaler depending on the system clock
	if (F_CPU == 1600000UL) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if ((F_CPU == 8000000UL) || (F_CPU == 4000000UL)) {	// prescaler set to 32
		TCCR2 &= ~(1<<CS22);
		TCCR2 |= ((1<<CS21) | (1<<CS20));
		prescaler = 32.0;
	} else if (F_CPU == 1000000L) { // prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	}
#endif
#elif defined (ATMEGAxU)
    //during setup, disable all the interrupts based on timer3
    TIMSK3 &= ~((1<<TOIE3) | (1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C) | (1<<ICIE3));
    //normal mode: counter incremented until overflow, prescaler set to /1
    TCCR3A &= ~((1<<WGM31) | (1<<WGM30));
    TCCR3B &= ~((1<<WGM33) | (1<<WGM32) | (1<<CS32) | (1<<CS31));
    TCCR3B |= (1<<CS30);
    //TCCR3B = 1;
    prescaler = 1.0;
#endif

//set the initial value of the counter depending on the prescaler
#if defined (USE_INTERNAL_RTC)
	starter = 0;
#elif defined (ATMEGAxU)
	starter = 65536 - (uint16_t)((float)F_CPU * 0.001 / prescaler); //for 16 MHz: 49536
#else
	starter = 256 - (int)((float)F_CPU * 0.001 / prescaler);
#endif
}


//interrupt-driven routine to update the software clock
#if defined (ATMEGAx8) || defined (ATMEGA8) || defined (ATMEGA644) || defined (ATMEGAx0)
ISR(TIMER2_OVF_vect) {
	TCNT2 = starter;
#elif defined (ATTINYx313)
ISR(TIMER0_OVF_vect) {
	TCNT0 = starter;
#elif defined (ATTINYx4) || defined (ATTINYx5)
ISR (TIM0_OVF_vect) {
	TCNT0 = starter;
#elif defined (ATMEGAxU)
ISR (TIMER3_OVF_vect) {
    TCNT3 = starter;
#endif
	byte dayT;

#if ! defined (USE_INTERNAL_RTC)
	counterT++;
	if (delta != 0) {
		deltaS--;
		if (deltaS <= 0) {
			if ((deltaDir > 0) && ((counterT + 1) < 1000)) {
				deltaS = 864000UL / delta;
				counterT++;
			}
			if ((deltaDir < 0) && ((counterT - 1) >= 0)) {
				deltaS = 864000UL / delta;
				counterT--;
			}
		}
	}

	if (counterT > 999) { //1000 ms equals to 1 s
		counterT = 0;
#endif
		secondsX++;
		if (secondsX > 59) {
			secondsX = 0;
			minutes++;
			if (minutes > 59) {
				minutes = 0;
				hours++;
				if (hours > 23) {
					hours = 0;
					day++;
					if (month == 2) { //february?
						if ((*lib).isLeapYear()) {
							dayT = 29;
						} else {
							dayT = 28;
						}
					} else {
						dayT = daysPerMonth[month - 1];
					}
					if (day > dayT) {
						day = 1;
						month++;
						if (month > 12) {
							month = 1;
							year++;
						}
					}
				}
			}
		}
#if ! defined (USE_INTERNAL_RTC)
	}
#endif
}

/*
    .....AND HERE, UNLESS YOU EXACTLY KNOW WHAT YOU'RE DOING!
    YOU COULD ALTER THE TIMEKEEPING ALGORITHM
*/

/**********************************************************************/

//
//public methods
//

//sets the time
boolean swRTC::setTime(byte hourT, byte minuteT, byte secondT) {

	//check if the params are correct
    if (hourT > 23) { return false; }
    if (minuteT > 59) { return false; }
    if (secondT > 59) { return false; }

	hours = hourT;
	minutes = minuteT;
	secondsX = secondT;
	return true;
}


//sets the date
boolean swRTC::setDate(byte dayT, byte monthT, int yearT) {

	//check if the params are correct
	if ((dayT < 1) || (dayT > 31)) { return false; }
	if ((monthT < 1) || (monthT > 12)) { return false; }
	if (yearT < 0)  { return false; }

	day = dayT;
	month = monthT;
	year = yearT;
	return true;
}


//start the software RTC
void swRTC::startRTC() {
	setTimer();
	counterT = 0;
#if defined (ATMEGAx8) || defined (ATMEGA644) || defined (ATMEGAx0)
	TCNT2 = starter;
	TIMSK2 |= (1<<TOIE2);
#elif defined (ATMEGA8)
	TCNT2 = starter;
	TIMSK |= (1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
	TCNT0 = starter;
	TIMSK |= (1<<TOIE0);
#elif defined (ATTINYx4)
	TCNT0 = starter;
	TIMSK0 |= (1<<TOIE0);
#elif defined (ATMEGAxU)
    TCNT3 = starter;
	TIMSK3 |= (1<<TOIE3);
#endif
	SREG |= (1<<SREG_I);
	isRunning=true;
}


void swRTC::stopRTC() {
#if defined (ATMEGAx8) || defined (ATMEGA644) || defined (ATMEGAx0)
	TIMSK2 &= ~(1<<TOIE2);
#elif defined (ATMEGA8)
	TIMSK &= ~(1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
	TIMSK &= ~(1<<TOIE0);
#elif defined (ATTINYx4)
	TIMSK0 &= ~(1<<TOIE0);
#elif defined (ATMEGAxU)
	TIMSK3 &= ~(1<<TOIE3);
#endif
	SREG &= ~(1<<SREG_I);
	isRunning = false;
}


//returns the seconds
byte swRTC::getSeconds() {
	return secondsX;
}


//return the minutes
byte swRTC::getMinutes() {
	return minutes;
}


//return the hours
byte swRTC::getHours() {
	return hours;
}


//return the day
byte swRTC::getDay() {
	return day;
}


//return the month
byte swRTC::getMonth() {
	return month;
}


//return the year
int swRTC::getYear() {
	return year;
}


//check if the current year is a leap year
boolean swRTC::isLeapYear(int yearT) {
	if (yearT == 0) { yearT = year; }
	if (((yearT % 4) == 0) && ((yearT % 100) != 0) || ((yearT % 400) == 0)) {
		return true;
	} else {
		return false;
	}
}


//returns a timestamp giving the number of seconds since a part year (default=1970)
unsigned long swRTC::getTimestamp(int yearT){
	unsigned long time=0;

	//check the epoch
	if (yearT == 0) {
		yearT = 1970;
	} else if (yearT < 1900) {
		yearT = 1900;
	} else if (yearT > 1970) {
		yearT = 1970;
	} else if ((yearT != 1900) && (yearT != 1970)) {
		yearT = 1970;
	}

	//One revolution of the Earth is not 365 days but accurately 365.2422 days.
	//It is leap year that adjusts this decimal fraction. But...
	time += (getYear() - yearT) * 365.2422;
	for (byte i = 0; i < getMonth() - 1; i++){
		time += daysPerMonth[i]; //find day from month
	}
	time = (time + getDay()) * 24UL; //find hours from day
	time = (time + getHours()) * 60UL; //find minutes from hours
	time = (time + getMinutes()) * 60UL; //find seconds from minute
	time += getSeconds(); // add seconds
	if (time > 951847199UL) { time += 86400UL; } //year 2000 is a special leap year, so 1 day must be added if date is greater than 29/02/2000
	//the code below checks if, in case of a leap year, the date is before or past the 29th of februray:
	//if no, the leap day hasn't been yet reached so we have to subtract a day
	if (isLeapYear(getYear())) {
	    if (getMonth() <= 2 ) {
	        time -= 86400UL;
	    }
	}
	return (time - 86400UL); //because years start at day 0.0, not day 1.
}


//set deltaT to correct the deviation between computed & real time
//(floating point, given as seconds per day)
//@DEPRECATED - this will be removed in the next versions of the
//library. Use the int method instead.
boolean swRTC::setDeltaT(double deltaT) {
    return (swRTC::setDeltaT((int)(deltaT * 10)));
}

//set deltaT to correct the deviation between computed & real time
//(int given as decimal seconds per day)
boolean swRTC::setDeltaT(int deltaT) {
	if ((deltaT < -8400) || (deltaT > 8400)) {
		return false;
	}

	delta = deltaT;
	if (delta == 0) {
		deltaS = 0;
		deltaDir = 0;
	} else {
		deltaS = abs(864000UL / delta);
		deltaDir = delta / (abs(delta));
		delta = abs(delta);
	}
	return true;
}


//return the interal deviation between computed & real time
int swRTC::getDeltaT() {
    return (delta * deltaDir);
}


// set the internal clock using a timestamp using the epoch passed as argument
byte swRTC::setClockWithTimestamp(unsigned long timeT, int yearRef) {
	unsigned long dayT;

	dayT = timeT / (60UL * 60UL * 24UL);
	float remaining = float(timeT - dayT * (60UL * 60UL * 24UL));
	int yearT = (dayT / 365.2422);
	float dayRemaining = dayT - yearT * 365.2422;

	if (yearRef == 0) {
		yearRef = 1970;
	} else if (yearRef < 1900) {
		yearRef = 1900;
	} else if (yearRef > 1970) {
		yearRef = 1970;
	} else if ((yearRef != 1900) && (yearRef != 1970)) {
		yearRef = 1970;
	}

	yearT += yearRef;
	if (dayRemaining >= 365.2422) {
		return 1;//my math is wrong!
    }
	if (yearT < yearRef) {
		return 2;//year not supported!
    }
	byte monthT = 0;
	while (dayRemaining > daysPerMonth[monthT]){
		dayRemaining -= daysPerMonth[monthT];
		if (monthT == 1 && isLeapYear(yearT)) {
			dayRemaining--;
		}
		monthT++;
	}

	monthT++;//because month 0 doesn't exist
	if (monthT > 12) {
	    monthT = 1;
	    yearT++;
		//return 3;//my math is wrong!
    }
	if (dayRemaining >= (60UL*60UL*24UL)) {
		return 4;//my math is wrong!
    }
	dayT = dayRemaining;
	if (dayRemaining - dayT > 0){ //add partial day!
		dayT++;
	}
	byte hoursT = remaining / (60UL * 60UL);
	remaining = remaining - hoursT * (60UL * 60UL);
	if (remaining >= (60UL * 60UL)) {
		return 5;//my math is wrong!
    }
	byte minutesT = remaining / 60UL;
	remaining = remaining - minutesT * 60UL;
	if (remaining >= 60) {
		return 6;//my math is wrong!
    }
	year = yearT;
	month = monthT;
	day = dayT;
	//with leap years we must consider that there's a day more in the calendar
	//and at the jan., 1st the count rolls back to dec, 31th. so we must check
	//the next year
	if ((day == 31) && (month == 12)) {
	    if (isLeapYear(year + 1)) {
	        yearT++;
	    }
	};
	//in leap years we have a day more but we also be careful if we are on feb., 28th
	//because we must go to feb., 29th
	if (isLeapYear(yearT)) {
	    day++;
	    month == 2 ? dayT = daysPerMonth[month - 1] + 1 : dayT = daysPerMonth[month - 1];
	    if (day > dayT) {
	        day = 1;
	        month++;
	        if (month > 12) {
	            month = 1;
	            year++;
	        }
	    }
	}
	hours = hoursT;
	minutes = minutesT;
	secondsX = remaining;
	return 0;
}


//return the day of the week giving a timestamp
byte swRTC::weekDay(unsigned long timestamp) {
  return ((timestamp / 86400UL) + 4 ) % 7;
}


//return the current week day
byte swRTC::getWeekDay() {
	return (swRTC::weekDay(swRTC::getTimestamp()));
}

#endif
