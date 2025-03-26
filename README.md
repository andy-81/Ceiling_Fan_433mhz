# Ceiling_Fan_433mhz
Chinese Ceiling Fan controller using MQTT

Recently I purchased a ceiling fan from AliExpress which came with a 433mhz fan controller and remotes. Of course I wanted to integrate this with my HA instance so went about exploring how I could intercept the communication and then build an ESP32 based remote I could integrate with the home automation platform. Whilst this has been created for my HA instance it should be possible to integrate it with any platform that supports MQTT as this is how the communication between HA and the ESP board are working.

I started off by getting my Flipper Zero out to check the frequency (as it didnt actually say what frequency the remotes and controller ran on). After a quick analysis I was able to determine it was working on 433Mhz and actually had the Flipper receive the code and play back the code with the controller responding, so I knew I was on the right track.

I went about searching how to integrate and started off with ESPHome as I have other devices which are using this platform but struggled to read the signal due to the amount of interference and was also unable to get ESPHome to send the commands successfully once I had the data. At this point I was scratching my head until I came across a great article by Johnny who whilst had a different fan to me did manage to create an Arduino based controller for his own 433Mhz fan, he did a great write up which you can find here: https://blog.yonatan.dev/controlling-ceiling-fan-home-assistant-433mhz-rf/

As a result I was able to take his code he had created and manipulate it to enable it to work with my fan, as well as add secuity login for the MQTT server as I didnt want to enable unauthenticated login from the ESP board.

In order to make use of this sketch (.ino) you will need to ensure you have the relevant modules installed as it makes use of knolleary/pubsubclient (https://github.com/knolleary/pubsubclient) & sui77/rc-switch (https://github.com/sui77/rc-switch) both of which will need adding by going to Sketch -> Include Library -> Manage Libraries in Arduino IDE.
