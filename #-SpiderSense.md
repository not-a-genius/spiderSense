# SpiderSense
Smartphone app to visualize data from a nucleo STM32-F401RE board and to broadcast messages on Telegram.

<p align="center">
  <a href="https://www.hackster.io/163479/spider-sense-e3912e">
    <img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/allProj.png" alt="Logo">
  </a>
</p>

# Table of Contents

* [About the Project](#about-the-project)
  * [Architecture](#architecture)
* [Built With](#built-with)
* [Prerequisites](#prerequisites)
* [Files](#files)
* [Getting started](#getting-started)
  * [Set the nucleo board hardware](#set-the-nucleo-board-hardware)
  * [Set the nucleo board code](#set-the-nucleo-board-code)
  * [Set the Android app](#set-the-android-app)
  * [Set Telegram bot](#set-the-telegram-bot)
* [Usage](#usage)
* [Future Features](#future-features)
* [Contributing](#contributing)
* [Credits](#credits)
* [License](#license)


# About the project

This is a group project made by three students of Sapienza University of Rome for Iot course. The task was to design, develope and document a personal project making use of Nucleo-F401RE board.
Following the design strategy learnt in the course we started from persona, problem space and use case scenarios to come with an idea.
- Slides about the design idea:
<p align="center">

  <a href="https://www.slideshare.net/GiuseppeCapaldi/spidersense-project-design-idea">
    <img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/Slideshare-logo.jpeg" width="80" height="80">
  </a>
</p>

Then we moved towards an mvp presentation starting to focus in technology. We moved to a simpler architecture without camera YOLO based detection but adding a servo motor and a radar animation that is possible thanks to it and to Processing APIs.
- Slides about project mvp:
<p align="center">

  <a href="https://www.slideshare.net/GiuseppeCapaldi/spider-sense-project-first-mvp">
    <img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/Slideshare-logo.jpeg" width="80" height="80">
  </a>
</p>
From this we discussed with professor and analyzed feedbacks. After that we changed some architectural parts, replacing the Raspberry-PI board with a stm32 bluetooth module the professor gave to us and developing a brand new Android app from scratch.

So we used nucleo board along with a simple homemade "radar architecture" to do obstacle proximity detection (based on distance and angle). Then we visualize real-time data on a smartphone homemade app using BLE connection (using IDB05A1 module) to pass data. Finally we broadcast a message every time the alarm (obstacle in proximity of the user) with location of device to other smartphones through Telegram APIs.

# Architecture
At the end our globlal system architecture is this:
<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/diagramFinalIot.jpg"></img>

# Built with

**Nucleo board**
* [MbedOs](https://www.mbed.com/en/platform/mbed-os/)

**Android app**
* [Android](https://developer.android.com/)
* [Processing for Android](https://android.processing.org/)

**Telegram bot**
* [Node js](https://nodejs.org/it/)
* [Mongo db](https://www.mongodb.com/it)
* [Heroku](https://elements.heroku.com/addons/mongolab)

# Prerequisites
- Android 9.0 (app has been tested on samsung s8 smartphone)
- Nucleo stm32-F401RE board
- Nucleo IDB05A1 bluetooth shield
- Telegram app installed

# Files
Files are organized as follows:
- Android: here you have the Android app files
- 'Nucleo Board': here you have Nucleo-F401RE code and used libraries
- 'Telegram Bot': here you have the code to do the telegram bot
- our_doc: here you have images for readme and other stuff
- 'Compiled Files': here you have apk file to install on your Android smartphone and bin file to copy on Nucleo board



# Getting started
## Set the nucleo board hardware
Follow the schematic you find on [Hackster.io page](https://www.hackster.io/163479/spider-sense-e3912e).
## Set the nucleo board code
1. Compile on [Mbed online compiler](https://ide.mbed.com/compiler/), or on an offline version, the code for nucleo board you can find in folder: **master/'Nucleo board'**. Here you can find the **"main.cpp"** code file, the needed custom libraries.
Now you will have a **.bin** file with the executable for nucleo board.

2.  Copy and paste **.bin** file in the folder of Nucleo board after you have mounted the usb drive.

3. As soon as you give power to the board the code will be executed.

## Set the Android App
1. On android  studio you have to open the folder of the project you can find here: **master/Android/spiderSense**.
Then you need to build and run the app on your device (you cannot emulate on android studio a smartphone cause of bluetooth connection requirements).
To do so:
2. Connect your phone through usb, give permission to trust your computer
3. Enable debug mode on your phone.
4. Press "Run" button and select your device
5. The app will start automatically on your phone if no error occurs.

## Set the Telegram bot
To do so you need do add the bot **@SpiderSenseBot** on Telegram and then register your device sending the message: **/tie \<deviceId\>**, where **\<deviceId\>**  is the username you want to be registered with on the bot.
Now you are ready to receive notification of messages from the bot.

The purpose of this telegram bot is to let people who are interested in particular device, receive urgent notifications from it.
A device sends GET request with its id to the server which also serves as a Telegram bot. Then, the bot will send notifications to users who are associated with the device. It's also possible to send geo data(latitude and longitude) about current position to the server so people in charge of will also know where the device is.

To associate a Telegram account with a device, user should speak with the bot and send to him the following command:
```
/tie {device}
```
Where **device** is a device username chosen by user.

So, if the device with such id will send a request, the user will be notified.

To store telegram accounts associated with devices, the server uses MongoDB.




# Usage

Having the nucleo board connected and running the IDB05A1 module attached on top of nucleo board will start "advertising", broadcasting data to the devices in the signal field.

Start SpiderSense app and click on Settings -> Connect.
If the board is recognized you will see the text Spidersense appear on the settings sceen.

Now the nucleo board will start moving the servo and receving data from ultrasonic sensor.

On your app, coming back to home screen, you should see a radar animation showing what data the app is receiving from the nucleo board, so the angle in which the servo is positioned and the obstacle distance (if it is detected in a close range).
<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/mock2.jpg" display="block" margin-left="auto" margin-right="auto"
 ></img>

Now if the number of detected objects in a close field goes over a fixed threshold a timer will start.
You should see a countdown text animation on the settings screen.
If within 10 sec you don't press the stop the alarm button a message with your id and location will be sent to all the devices registered on the SpiderSense telegram bot.

If you press the stop the alarm button the counter of objects detected will be reset, ready to trigger the alarm timer again.

You can also disconnect and connect again to the board.
<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/mock1.jpg" display="block" margin-left="auto" margin-right="auto"
 ></img>

What all the devices registered on telegram bot should see is a message with the id linked to the device which triggered the alarm and an image from google maps apis showing its gps location.

Example of a message from the bot:

<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/Message_example.png" display="block" margin-left="auto" margin-right="auto"
 ></img>


# Future Features
Features to be implemented:
- other android versions compatibility
- improve UI and app look and feel
- encrypt data towards telegram servers
- make auth login in app
- improve nucleo hw/sw

# Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

# Credits:
- Giuseppe Capaldi [<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/gitIcon.png" height="20" width="20" >](https://github.com/not-a-genius)
					[<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/inIcon.png" height="20" width="20" >](https://www.linkedin.com/in/giuseppe-capaldi-56688a171/)
- Marco Costa [<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/gitIcon.png" height="20" width="20" >](https://github.com/marcocosta96/)
					[<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/inIcon.png" height="20" width="20" >](https://www.linkedin.com/in/marco-costa-ecs)
- Artem Savchuck [<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/gitIcon.png" height="20" width="20" >](https://github.com/Artem723)
 					[<img src="https://raw.githubusercontent.com/not-a-genius/spiderSense/master/our_doc/readme_images/inIcon.png" height="20" width="20" >](https://www.linkedin.com/in/artem-savchuk-7278a7170/)


# License:
 Code is under Apache License 2.0.
