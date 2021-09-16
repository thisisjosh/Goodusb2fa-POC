/* This file is part of swRTC library.
   Please check the README file and the notes
   inside the swRTC.h file to get more info
   
   This example will print the time every second
   to the computer through the serial port using the 
   format HH:MM or HH:MM:SS
   
   Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.
*/

#include <swRTC.h>
swRTC rtc; //create a new istance of the lib
const byte WITHOUT_SECONDS = 0;
const byte WITH_SECONDS = 1;

void setup() {
	rtc.stopRTC(); //stop the RTC
	rtc.setTime(12,0,0); //set the time here
	rtc.setDate(4,6,2012); //set the date here
	rtc.startRTC(); //start the RTC
	Serial.begin(19200); //choose the serial speed here
	delay(2000); //delay to let the user opens the serial monitor
}

void loop() {
    printTime(WITHOUT_SECONDS);
    printTime(WITH_SECONDS);
    Serial.println("");
    delay(1000);
}


void printTime(byte withSeconds) {
    sendNumber(rtc.getHours());
    Serial.print(":");
    sendNumber(rtc.getMinutes());
    if (withSeconds) {
        Serial.print(":");
        sendNumber(rtc.getSeconds());
    }
    Serial.println("");
}


void sendNumber(byte number) {
    byte temp;
    if (number>9) {
        temp=int(number/10);
        Serial.print(char(temp+48));
        number-=(temp*10);
    } else {
        Serial.print("0");
    }
    Serial.print(char(number+48));
}
