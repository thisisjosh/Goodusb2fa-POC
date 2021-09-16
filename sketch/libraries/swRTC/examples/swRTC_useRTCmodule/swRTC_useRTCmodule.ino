/* This file is part of swRTC library.
   Please check the README file and the notes
   inside the swRTC.h file to get more info

   This example will blink a led every second using the internal
   Real-Time Counter (RTC) of the microcontroller. Be careful: not every
   MCU integrates this module.

   To test this example, set up a standalone Atemga328P microcontroller with:
   * 10K pull-up resistor on pin 1
   * 5V VCC on pins 7 and 20
   * GND on pins 8 and 22
   * external 32,768 Hz crystal on pins 8 and 9 with 2 22pF ceramic capacitors
   * an LED on pin 19 w/ a 330 ohm resistor.
   ("pin" means phisical pin, not digital pin)

   Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.
*/

//the following #define will instruct the library to use the interanl
//real-time counter of the microcontroller
#define USE_INTERNAL_RTC
#include <swRTC.h>

swRTC rtc;

const byte LED_PIN = 13; //change this to wathever you want
byte state = 1; //initial state of the LED

void setup() {
    pinMode(LED_PIN, OUTPUT); //LED pin set to output
    rtc.stopRTC(); //stop the RTC
    rtc.setTime(12,0,0); //set the time here
    rtc.setDate(11,05,2013); //set the date here
    rtc.startRTC(); //start the RTC
}

void loop() {
    //every second change the LED state
    digitalWrite(LED_PIN, state);
    byte second = rtc.getSeconds();
    while (second == rtc.getSeconds());
    state^=1;
}
