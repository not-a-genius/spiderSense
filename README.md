# SpiderSense
Smartphone app to visualize data from a nucleo STM32-F401RE board and to broadcast messages on Telegram.

<!-- image of acrhitecture
<img src="">
-->

## Description

This is a group project made by three students of Sapienza University of Rome for Iot course. The task was to design, develope and document a personal project making use of Nucleo-F401RE board. Our idea was to use it along with a simple homemade "radar architecture" to do obstacle proximity detection (based on distance and angle) and then visualize real-time data on a smartphone homemade app using BLE connection to pass data. Finally we broadcast a message with location of device to other smartphones through Telegram APIs.

<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/diagramFinalIot.jpg">

##Files
Files are organized as follows:
- Android: here you have the Android app files    
- 'Nucleo Board': here you have Nucleo-F401RE code and custom libraries
- 'Telegram Bot': here you have the code to do the telegram bot
- our_doc: here you have images for readme and other stuff



## Installation

<!--
```java
//Constant.java
package com.example.easy2park;

public class Constant {
    public static final String BLE_STATE_CHANGED_ACTION = "android.bluetooth.adapter.action.STATE_CHANGED";
    public static final String SENSORO_DEVICE1="0117C59B4EC7";
    public static final String SENSORO_DEVICE2="0117C582CAD7";
    public static final String SENSORO_DEVICE3="0117C5578442";
}
```
-->

### First step:


### Second step:


### Third step:

### Run your android app

## Usage
### Set the Nucleo board

First thing to do is to compile on [Mbed online compiler](https://ide.mbed.com/compiler/), or on an offline version, the code for nucleo board you can find in folder: " master/'Nucleo board' ". Here you can find the main.cpp code file and also the needed custom libraries.

Then you will have a .bin file ready to be copied and pasted in the folder of Nucleo board after you have mounted the usb drive.

As soon as you gain power to the board the code will be executed.

### Set Android App

On android  studio you have to open the folder of the project you can find here: "master/Android/spiderSense". Then you need to build and run the app on your device (you cannot emulate on android studio a smartphone cause of bluetooth connection requirements). To do so you need to connect your phone through usb, give permission to trust your computer and enable debug mode on your phone. At this point as long as you press "Run" button and you select your device the app will start automatically on your phone.


## Future Features
Features to be implemented:
- other android versions compatibility
- improve UI and app look and feel

## Credits:
- Giuseppe Capaldi [<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/gitIcon.png" height="20" width="20" >](https://github.com/not-a-genius)
					[<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/inIcon.png" height="20" width="20" >](https://www.linkedin.com/in/giuseppe-capaldi-56688a171/)
- Marco Costa [<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/gitIcon.png" height="20" width="20" >](https://github.com/marcocosta96/)
					[<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/inIcon.png" height="20" width="20" >](https://www.linkedin.com/in/marco-costa-ecs)
- Artem Savchuck [<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/gitIcon.png" height="20" width="20" >](https://github.com/Artem723)
 					[<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/inIcon.png" height="20" width="20" >](https://www.linkedin.com/in/artem-savchuk-7278a7170/)


## License:
 Code is under Apache License 2.0.
