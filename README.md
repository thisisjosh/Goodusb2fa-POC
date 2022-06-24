# [GoodUSB aka DIY YubiKey](http://optimumunknown.com/goodusb.html)
An Arduino based 2-factor-authentication key. This project consists of two parts. The Arduino which types in 2FA codes by emulating a keyboard and a companion computer app for instructing the Arduino on which service's 2FA code you want to type. The secret codes for generating the 2FA codes only lives inside the Arduino instead of within an authenticator app on your computer like Authy. Using a GoodUSB saves time since it types the 2FA code for you, it is a lot cheaper than a YubiKey, and it could be more secure than using an authenticator app with secrets stored on your computer. This is a proof of concept project. Do not use for protecting any important account.

## Why is it called GoodUSB?
There are a number of small micro controllers for sale labeled as BadUSB. Basically they look like innocent USB sticks, but once plugged in they pretend to be a keyboard and start typing commands to take control of a computer. This project takes the same hardware used for BadUSBs and uses them to increase security. Therefore, by doing something good instead of bad it becomes a GoodUSB.

## Is this truly secure?
Not completely. It is possible to pull the code and keys out of the Arduino. There are fuse bytes inside the Arduino you could try using to prevent the machine code from being copied, but I have not looked into how to do this yet and I have not looked into how secure it is.

## Why not use a YubiKey?
This is a cheap DIY project I made for fun. Use at your own risk. Definitely use something like a YubiKey for better security.

## Requirements
This project needs an Arduino that can emulate a keyboard such as the Arduino Leonardo.

## Electron App
This is the latest companion app for controlling the Arduino. Originally I made the companion app as a Chrome App, but [Chrome Apps are being deprecated](https://blog.chromium.org/2020/08/changes-to-chrome-app-support-timeline.html). I kept the Chrome App working for a while using [NWJS](https://nwjs.io/), but later ran into trouble, so I decided to finally switch to [Electron](https://www.electronjs.org/).  I got started by cloning this [electron-serialport](https://github.com/serialport/electron-serialport) example.
## Build and run
Plug in the Arduino to your computer.
```
cd electron-app
npm install
npm start
```

## TODOs
This project has a lot of hard coded values for the 2FA auth keys. It would be nice to be able to add/delete these using the companion app.
* Dynamically add auth keys. Store keys in the EEPROM.
* Add a realtime clock, display, and UI to the Arduino in order to work without the companion app.
* Encrypt secrets on the Arduino with a master key.
* Detect the site with a Chrome extension and auto select the auth key.
* Port the companion app to something else like a Chrome extension.
* Try out Web Serial API (still experimental) https://stackoverflow.com/questions/44217146/chrome-extension-how-to-use-serial-port-now-that-apps-are-sunsetting/44877138
