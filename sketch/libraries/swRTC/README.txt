***************
** s w R T C **
***************

The swRTC is a software implementation of a Real-Time Clock for Arduino boards and Atmel microcontrollers.
It's based on an internal timer and it requires no external hardware.

********************************************************************

VERSION HISTORY

2018/02/13 - v. 1.2.8: Fixed a typo that broke the compatibility with Atmega644
2017/01/27 - v. 1.2.7: added support for ATmega32U4 (thanks to Esteban Torre and piluex)
2016/05/21 - v. 1.2.6: fixed timestamp functions when we are in leap years (I hope definitely..)
2016/02/21 - v. 1.2.5: fixed a bug into the deltaT arithmetics that leads to double count the computation.
2016/02/14 - v. 1.2.4: updated to use with latest Arduino IDE branches (>=1.6.7)
2016/02/14 - v. 1.2.3: fixed the type of the prototype of the function setClockWithTimestamp
2016/01/09 - v. 1.2.2: fixed a bug in the timestamp code that returned a wrong value until february, 29th for leap year
2013/05/31 - v. 1.2.1: changed a couple ov variables from int to byte type.
2013/05/11 - v. 1.2.0: the libray now supports the Real-Time Counter module of some MCUs
2013/05/01 - v. 1.1.2: now getDeltaT return the deltaT with the correct sign
2013/04/01 - v. 1.1.1: fixed a bug in the new setDeltaT method
2013/03/31 - v. 1.1.0: setDeltaT now works with ints - new getDeltaT method
2013/01/14 - v. 1.0.6: fixed a bug in the function used to set the clock given a timestamp
2012/06/04 - v. 1.0.5: introduced new method getWeekDay
2012/05/05 - v. 1.0.3: added support for 4 MHz clock (read below)
2012/04/23 - v. 1.0.2: first fix of a bug in the algorithm of getTimestamp
2012/25/01 - v. 1.0.1: latest versione, distribuited under the terms of the GNU GPL v.3.0 and any later version
2012/01/22 - v. 1.0:   first stable & complete release of the library
2012/01/20 - v. 0.8.4: the function now can set the internal clock using a timestamp
2011/11/27 - v. 0.8.3:  introduced support for Atmega1284
2011/10/09 - v. 0.8.2:  fixed a glitch in deltaT and introduced support for deltaT with tenths od a second
2011/10/09 - v. 0.8.1:  new deltaT based on seconds/day
2011/10/08 - v. 0.8:    introduced deltaT to correct deviations between computed & real time and Timestamp
2011/10/04 - v. 0.7.1:  dropped millis sketch due to the fact that milliseconds is no longer required
2011/10/01 - v. 0.7:    corrected some bug; the support for Attinyx4 now works
2011/09/30 - v. 0.6:    added supporto for Attiny2313
2011/09/29 - v. 0.5:    corrected some bugs
2011/09/28 - v. 0.4:    addedd support for Atmega640/1280/1281/2560/2561
2011/09/28 - v. 0.3:    added support for Attiny24/44/84 and Atmega644 (Attinyx4 don't work at the moment: FIXED!)
2011/09/28 - v. 0.2:    added support for Attiny25/45/85 and Atmega8/8A
2011/09/26 - v. 0.1:    first release - only works on Arduino and Atmega328 at 1/8/16 MHz

********************************************************************

REQUIREMENTS

* The swRTC library needs Arduino IDE version 002x/1.0.x to work with Atmega48/88/168/328/328P, Atmega640/1280/1281/2560/2561 and Atmega8/8A.
* To work with Attiny24/44/84, Attiny25/45/85 and Attiny2313/4313 you need the Tiny Core libraries that you can download from:
http://code.google.com/p/arduino-tiny/ (1)
* To work with Atmega644/1284 you need the 644P/1284P package that you can download from my website (only for IDE 1.0.x):
http://www.leonardomiliani.com/?p=625&lang=en (2)

(1) Directions to install and use the Tiny core are provided from its author.
(2) Directions to install the mod files can be found inside the package

********************************************************************

INSTALLATION

1) Uncompress the archive and copy the swRTC folder inside the "libraries" folder inside your sketchbooks' folders. If the "libraries" folder doesn't exist, you have to create it.
To know where the sketchbooks folder is located, open the Arduino IDE, then select from the menu "File/Preferences" and look at the path in "Sketchbook location".
At the end, you should have something similar to this (for Linux): /home/username/sketchbook/libraries/swRTC

2) After that, you have to replace a file of the Tiny core. Make a backup copy in case you need to roll back to the original one.
To do this, locate the file named core_build_options.h that is located in your-arduino-IDE-folder/hardware/tiny/cores/tiny/ and rename it to core_build_options.bck (or a name you like).
Then, copy the core_build_options.h that comes from the swRTC into the previous path.
This will permit to have both the swRTC library and the delay/millis functions from Arduino IDE up and running on Tinyx5/x4/2313 microcontrollers.

********************************************************************

USAGE & METHODS

Include the library in your sketch with
#include <swRTC.h>

To use the library, first you have to create an instance of the library, i.e.:
swRTC rtc;

Then you have to set up the internal clock with valid time&date. This can be done via software or by the user through a user interface (i.e. LCD display) using the following methods:
rtc.setTime(hours, minutes, seconds);
rtc.setDate(day, month, year);

It is suggested to stop the swRTC before to change the time and/or the date with:
rtc.stopRTC();
and then to start it again, after changes are done, with:
rtc.startRTC();

The startRTC() method also sets the HW timer according to the clock frequency of your Arduino/Atmel microcontroller, synchronizing the prescaler of the timer to match the correct timings.

To get the time&date you can use the following methods:
rtc.getSeconds();
rtc.getMinutes();
rtc.getHours();
rtc.getDay();
rtc.getMonth();
rtc.getYear();

Most of the above methods return a byte value, only getYear() returns a signed integer value.

If you want to know if a specific year is a leap year, you just have to call:
rtc.isLeapYear([year])

to get a boolean value that is true in case of a leap year. The argument "year" is optional: if you don't provide it, the library will check the current year.

If you want to know the current week day, just call
rtc.getWeekDay()

to get a number indicating the current day of the week:
from 0 for sunday to 6 for saturday

The library can work with timestamps too.
rtc.getTimestamp([epoch]);
rtc.setClockWithTimestamp(timestamp[, epoch]);
rtc.weekDay(timestamp);

The first function returns the number of seconds since the beginning of a particular date epoch (1900 or 1970). By default, it returns the number of seconds since 1970.0 (1970/01/01), the UNIX epoch, but you can also use the epoch 1900.0 (1900/01/01). The second one sets the internal clock using a timestamp (i.e. a timestamp provided from an NTP server). If specified, epoch will be used.
The third function returns a number indicating the weekday of a given timestamp, starting from 0 for sunday to 6 for saturday.

There are two more functions that are used to correct the difference between the time of the internal clock and the real time.
rtc.setDeltaT(value);
rtc.getDeltaT();

This is an important feature. Version 0.8 introduces the deltaT correction, a value to correct the deviation between the computed time and the real time, the time that is calculated by the library and the time that is effectively passed on. This function has been introduced to adjust the counter with particular sketches that can use other interrupt functions (i.e. NewSoftSerial library) or systems based on external resonators (like the new Arduino UNO) or interl oscillators, that can be affected by a tollerance of +/-10% over the nominal clock.
Allowed values are integer values between -8,400 and 8,400 (max -14.0 or +14.0 minutes/day). The deltaT is the number of tenths of seconds per day to apply to the RTC to correct the deviation from the real time. If your RTC is faster than the real time, try adjusting deltaT with negative values, if your RTC is slower, try a deltaT with a positive values.
I.e. 102 means a correction of 10200 ms/day, 10.2 seconds/day.
getDeltaT(), on the other hand, returns the current value of the time correction.

** I M P O R T A N T **: the use of floating point values in setDelatT() is now DEPRECATED. Old versions of the library that used floats still continue to work (accepting a value that indicates the number of seconds to add/subtract with a single decimal cipher, in the range -840.0..+840.0) but this does not mean that they will do in the future, so it is reccomended that you upgrade your code and start using the new syntax with integer values.

Since release 1.2.0, the library supports the use of the internal Real-Time Counter (RTC) present on several MUCs. This module permits to use the timer 2 in asynchronous mode with an external 32,768 Hz crystal. Setting up the timer 2 with a prescaler of 128, we get 1 overflow every 1 secon so that we can increment the software clock in a more accurate manner. At the moment, the use of deltaT is not supported with the use of the RTC module.
To use the RTC module (that is present only in Atmega88/168/328, Atmega8, Atmega344/644/1284 and Atmega1280/256x), just add the following #define before the inclusion of the library:
#define USE_INTERNAL_RTC
#include "swRTC.h"

The library has been modified and now the .cpp file has been removed, while its content has been included in the header file. This does not change the usage of the library

********************************************************************

TIMESTAMPS

The library can use timestamps to set the internal clock as it can return a timestamp from its clock.
To get a timestamp based on the internal clock, call:
rtc.getTimestamp(epoch);

"epoch" can be 1900 or 1970, depending if you want a timestamp based on the NTP epoch (1900.0) or the UNIX epoch (1970.0)

To set hte clock using a timestamp, call:
rtc.setClockWithTimestamp(timestamp, epoch);

timestamp is the timestamp to be used, epoch is the epoch (1900 or 1970) of the timestamp.

********************************************************************

SUPPORTED FREQUENCIES, MICROS AND LIMITATIONS

The library is interrupt driven and uses an internal timer.

ATMEL Atmegax0: Atmega640/1280/1281/2560/2561
The library supports the Atmel microcontrollers Atmega640/1280/1281/2560/2561 - the Atmega1280/2560 are the chips mounted on Arduino MEGA/MEGA2560. The library uses the timer 2. No limitations are on Arduino Timer 0 based functions like delay() or millis(). Limits are on the usage of PWM signals on pins 9 and 10 because the library alters the frequency of the output PWM signals of Timer 2.

ATMEL Atmegax8: Atmega48/88/168/328/328 with or without the "P" (PicoPower) core (this does not mean that the specific micro is supported from the Arduino IDE and/or avrdude).
The library supports the Atmel microcontrollers Atmega48/88/168/328. The library uses the timer 2. No limitations are on Arduino Timer 0 based functions like delay() or millis(). Limits are on the usage of PWM signals on pins 3 and 11 because the library alters the frequency of the output PWM signals of Timer 2.

ATMEL Atmega8/8A
This is the micro prior to Atmegax8 series. It has been used on ArduinoNG. The library uses the timer 2.

ATMEL Attinyx5: Attiny25/45/85
On this family of MCUs, the library uses the Timer 0. Delay() & millis() still continue to function because the Tiny core sets them to work with Timer 1.

ATMEL Attinyx4: Attiny24/44/84
The library uses the Timer 0 because Timer 1 is a 16 bit counter that is a little bit more difficult to set. For this reason, delay() and millis() functions are moved on timer 1 (follow the directions in INSTRUCTION)

ATMEL Attiny2313/4313
The library uses the Timer 0 because Timer 1 is a 16 bit counter that is a little bit more difficult to set. For this reason, delay() and millis() functions are moved on timer 1 (follow the directions in INSTRUCTION)

ATMEL Atmega644/1284
The library uses the Timer 2, so keep it in mind if you think to use PWM on phisical pins 20 & 21.

ATMEL ATmega32U4:
The library uses the Timer 3 on this chip.

** I M P O R T A N T **
The library supports, due to rounding issues, only the following speed clocks:
- Atmega644/1284, Atmega48/88/168/328, Atmega8, Atmega640/1280/1281/2560/2561: 1, 4, 8, and 16 MHz
- Attiny25/45/85, Attiny24/44/84, Attiny2313: 1, 8, and 16 MHz
- Atmega32U4: only 16 MHz

********************************************************************

HOW IT WORKS

The library sets an internal timer in counter mode to have 1 overflow interrupt exactly every 1 millisecond.
Every 1000 ms it increments the internal registers starting from seconds to years to keep the running of the time.

To do this, is calculates the correct starting value of the counter and the prescaler of the timer, depending on the system frequency. It also chooses the correct timer because this trick only works with 8-bit counters. On Attiny micros, the library must move the delay/millis functions on timer 1 because these micros have only 2 timers, and the 8-bit timer is timer 0.

********************************************************************

LICENCE

This library is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

********************************************************************

AUTHOR

Written by Leonardo Miliani <leonard AT leonardomiliani DOT com>

********************************************************************

CONTRIBUTORS

lesto - from <www.arduino.cc> forum

********************************************************************

Last document revision: 2018/02/13
