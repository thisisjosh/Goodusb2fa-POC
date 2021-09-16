/* This file is part of swRTC library.
   Please check the README file and the notes
   inside the swRTC.h file to get more info
   
   This example will blink a led every second.
   
   Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.
*/

#include <swRTC.h>
swRTC rtc;
#define LED_PIN 8 //change this to wathever you want
byte state=1;
byte second;

void setup() {
    rtc.stopRTC(); //stop the RTC
    rtc.setTime(12,0,0); //set the time here
    rtc.setDate(27,10,2011); //set the date here
    rtc.startRTC(); //start the RTC
    pinMode(LED_PIN, OUTPUT); //LED pin set to output
    digitalWrite(LED_PIN, state); //write the initial state of the LED
    second=rtc.getSeconds();
}

void loop() {
    //every second change the LED state
    if (second!=rtc.getSeconds()) {
        second=rtc.getSeconds();
        state^=1;
        digitalWrite(LED_PIN, state);
    }
}
