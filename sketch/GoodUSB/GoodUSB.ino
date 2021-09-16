#include <sha1.h>
#include <TOTP.h>
#include <swRTC.h>
#include <Regexp.h> // http://gammon.com.au/Arduino/Regexp.zip
#include <Keyboard.h>


// ----- Secret Codes - Do not checkin your real secret codes into source control. -----
uint8_t hmacKey[2][40] = {
		{0x33,0x3e,0x5d,0xd5,0xd2,0x14,0x3e,0x24,0x82,0x55,0x1a,0x73,0x12,0xd6,0xba,0x20,0xa0,0x13,0xa5,0xe5},
    {0xff, 0x70, 0xeb, 0x47, 0xae, 0x17, 0xc3, 0x5c, 0x69, 0x6e}
	};

// otpauth://totp/goodusbpoc@protonmail.com?secret=GM7F3VOSCQ7CJASVDJZRFVV2ECQBHJPF&issuer=Proton&algorithm=SHA1&digits=6&period=30
// otpauth://totp/GitHub:goodusb?secret=75YOWR5OC7BVY2LO&issuer=GitHub
// We only need the secret for the hmax above. It is base32. We need to convert it to hex.
// ----------------------------------- End Secrets -------------------------------------

// Resources:
// google auth uses base32 http://tomeko.net/online_tools/base32.php?lang=en
// http://www.lucadentella.it/OTP/
// https://github.com/google/google-authenticator/wiki/Key-Uri-Format

int keySizes[2] = {20, 10};

swRTC rtc;
char code[7];

String line;
int account = 0;
int buttonState = 0;
int pressesCount = 0;
long downCount = 0;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	while(Serial.available()) {
		line = Serial.readString();
		Serial.println("readString: " + line);
		
		char buf[32];
		char bufMatch[8];
		
		line.toCharArray(buf, 128);
		
		MatchState ms;
		ms.Target (buf);  // what to search
		char result = ms.Match("(%d+)-(%d+)-(%d+) (%d+):(%d+):(%d+) (%d+) (%d+)", 0); //DD-MM-YYYY HH:MM:SS ## #
		if (result == REGEXP_MATCHED)
		{
			int day, month, year, hour, minute, second, doType;
			
			ms.GetCapture (bufMatch, 0);
			day = atoi(bufMatch);
			
			ms.GetCapture (bufMatch, 1);
			month = atoi(bufMatch);
			
			ms.GetCapture (bufMatch, 2);
			year = atoi(bufMatch);
			
			ms.GetCapture (bufMatch, 3);
			hour = atoi(bufMatch);
			
			ms.GetCapture (bufMatch, 4);
			minute = atoi(bufMatch);
			
			ms.GetCapture (bufMatch, 5);
			second = atoi(bufMatch);
			
			ms.GetCapture (bufMatch, 6);
			account = atoi(bufMatch);

      ms.GetCapture (bufMatch, 7);
      doType = atoi(bufMatch);
			
			Serial.print("day: ");
			Serial.print(day);
			Serial.print(" month: ");
			Serial.println(month);
			Serial.print(" year: ");
			Serial.print(year);
			Serial.print(" hour: ");
			Serial.print(hour);
			Serial.print(" minute: ");
			Serial.print(minute);
			Serial.print(" second: ");
			Serial.print(second);
			Serial.print(" account: ");
			Serial.print(account);
      Serial.print(" doType: ");
      Serial.println(doType);
			
			rtc.stopRTC();
			// Adjust the following values to match the current date and time
			// and power on Arduino at the time set (use GMT timezone!)
			rtc.setDate(day, month, year);
			rtc.setTime(hour, minute, second);
			rtc.startRTC();

      if(doType == 1){
        delay(2000);
        typeCode(account);
      }
		}
		else if (result == REGEXP_NOMATCH)
		{
			// no match
			Serial.println("unknown command");
		}
		else
		{
			// some sort of error
			Serial.println("regex error");
		}
	}
	
	buttonState = digitalRead(2);
	if (buttonState == HIGH) {
		if(downCount <= 0){
			Serial.print("Button press! ");
			Serial.println(pressesCount);
			pressesCount++;
		}
	    downCount++; // how long the putton is held down
  	}
  	else {
  		// button released?...has to be down at least 1000 otherwise spurious press
  		if(downCount > 1000 && downCount < 50000){
  			// Button just released
  			Serial.print("button release at ");
  			Serial.println(downCount);
        typeCode(account);
  		}
  		else if(downCount > 50000){
  			// long press
  			Keyboard.begin();
			Keyboard.println("Hello World!");
			Keyboard.end();
  		}
  		downCount = 0; // reset the downCount back to zero. It has been released.
  	}
}

void typeCode(int account){
  TOTP totp = TOTP(hmacKey[account], keySizes[account]);
  long GMT = rtc.getTimestamp();
  char* newCode = totp.getCode(GMT);
  Keyboard.begin();
  Keyboard.println(newCode);
  Keyboard.end();
}
