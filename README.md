# RF24SH - RF24 Smart Home Protocol
======

NOTE: This is under development! A lot of stuff needs to be fixed.

Goals:
* Provides an easy to use Network Library for NRF24L01+.

Dependencies:
* RF24 Library (Arduino) - librf24/arduino/ - https://github.com/maniacbug/RF24
* RF24 Library (Teensy)  - librf24/teensy/ - 
* RF24 Library (RPi)	 - librf24/rpi/ - https://github.com/gnulnulf/RF24

How this library works:
The NRF24L01+ module can listen up to 6 channels. First channel can be 0x7878787878 for example all other channels must be in the same range for example 0xABABABABYY. More information about this you can get on NRF24L01+ datasheet. Channels:
* Channel 0 is for protocol communication
* Channel 1 is for parent communication
* Channel 2-5 is for child communication

How to use it:
* Setup your NRF24L01+ and make sure they are working with RF24 Library and their examples. (Guarantee everything is wired correctly)
* Create a MASTER station
* Create less then 250 regular stations
* Just connect one STATION by time (1 minute to stabilish connection - This is a huge secure factor)

Basic example of communication:
* Master ON listening on Protocol Channel
* Child connects without an ID - SENDS WHO_LISTEN (Every station that listen to it, will send a message I_LISTEN)
* Master sends I_LISTEN
* Child keep track of 5 nodes that he can talk to. If station receives more then 5 I_LISTEN it will preserve the smaller level ones (So it keeps near MASTER station)
* Child send ASK_CONFIG
* Master reply with SET_CONFIG sending ID, PARENT, LEVEL and other informations
* If a child can't communicate directly to MASTER who can listen to him will forward the ASK_CONFIG (and the SET_CONFIG later)

TODO:
* Interrupt driven read

About:
* This library is being developed by Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
* Feel free to contact me with suggestions, questions and others (I will do my best to help)
* http://www.matbra.com
* http://www.matbra.com/en/code/rf24sh-internet-das-coisas/
