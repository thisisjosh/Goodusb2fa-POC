# GoodUSB aka DIY YubiKey
An arduino based 2-factor-authentication key. This project consists of two parts. The arduino which types in 2FA codes by emulating a keyboard and a companion computer app for instructing the arduino on which service's 2FA code you want to type. The companion app started out as a Chrome App, then was migrated to a NW.js app, and finally migrated to an Electron app. The secret codes for generating the 2FA codes only lives inside the arduino instead of within an authenticator app on your computer like Authy. Using a GoodUSB saves time since it types the 2FA code for you, it is a lot cheaper than a YubiKey, and it could be more secure than using an authenticator app on your computer.

## Why is it called GoodUSB?
There are a number of small micro controller for sale labeled as BadUSB. Basically they look like innocent USB sticks, but once plugged in they pretend to be a keyboard and start typing commands to take control of a computer. This project takes the same hardware used for BadUSBs and uses them to increase security. Therefore, by doing something good instead of bad it becomes a GoodUSB.

## Is this truly secure?
Not completely. It is possible to pull the code and keys out of the arduino. There are fuse bytes inside the arduino you could try using to prevent the machine code from being copied, but I have not looked into how to do this yet and I have not looked into how secure it is.

## Why not use a YubiKey?
This is a cheap DIY project I made for fun. Use at your own risk. Definitely use something like a YubiKey for better security.

## Requirements
This project needs an arduino that can emulate a keyboard such as the arduino leonardo.

## Electron App
This is the latest companion app for controlling the arduino. I got started by cloning this [electron-serialport](https://github.com/serialport/electron-serialport) example.
## Build and run
cd electron-app
npm install
npm start

## Chrome App (deprecated)
Chrome dropped support for Chrome apps on Windows, Linux, and Mac. [Chrome App Transition](https://developers.chrome.com/apps/migration)
I left the old Chrome app code in the repo for reference.
### See Chrome Arduino LED toggle
The Chrome LED Toggle Sample App was used get started in communicating with the Arduino.
[Chrome LED Toggle Sample App](https://github.com/GoogleChrome/chrome-app-samples/tree/master/samples/serial/ledtoggle)

Note: on Mac OS X Lion, it's necessary to update USB Serial drivers:
http://blog.geekscape.org/wordpress/2011/07/22/mac-os-x-17-lion-upgrading-ftdi-usb-serial-dr/

### Chrome APIs used
* [Serial API](http://developer.chrome.com/apps/app.hardware.html#serial)
* [Runtime](http://developer.chrome.com/apps/app.runtime.html)
* [Window](http://developer.chrome.com/apps/app.window.html)

### NW.js Runtime
I switched to nwjs.io as a runtime in an attempt to salvage the Chrome App.  This worked ok on some computers, but gave me trouble on others. This for the most part let me use the Chrome App unmodified for a while.
* [NW.js](https://nwjs.io/)

#### NW.js Build
Install [node](https://nodejs.org/en/download/)
Add to the project [nwjs-builder-phoenix](https://github.com/evshiron/nwjs-builder-phoenix)
// Add nwjs-builder-phoenix with running:
npm install nwjs-builder-phoenix --save-dev
// Then run the build with:
npm run dist
// Add other targets by adding to package.json "dist" win-x86,win-x64,linux-x86,linux-x64

## TODOs
This project has a lot of hard coded values for the 2FA auth keys. It would be nice to be able to add/delete these using the companion app.
* Dynamically add auth keys. Store keys in the EEPROM.
* Detect the site with a Chrome extension and auto select the auth key.
* Port the companion app to something else like a Chrome extension
** Try out Web Serial API (still experimental) https://stackoverflow.com/questions/44217146/chrome-extension-how-to-use-serial-port-now-that-apps-are-sunsetting/44877138