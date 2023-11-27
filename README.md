
# GSM & Facial Recognition Based Security System
---
### High level description
>The project will detect any movement in the problem then a record will start for 15 sec as the intruder must identify himself to the camera if he didn't stand in front of the Camera of the Pi controller, then a call is made to the supervisor and the record is reported to an email.


### Low level description
>This project will initially initialize the hw components Microcontrollers, sensors, and Pi camera inputs and outputs.

>Next the code will enter a loop waiting for any interrupt through motion sensor that will make the camera record a video for 15 sec to recognize the intruder through facial recognition code if he wasn't recognized the record would be sent through specified secure email, then Pi will send a certain msg to Arduino to send cmds to the GSM module

>Also, GSM role will be operational as GSM module will make a certain role if the intruder wasn't recognized

>GSM module can RX certain specified cmd through SIM memory to make call, to send SMS msg .... etc(as specified in the code). any Received cmd through sms msg to GSM module will be sent to the Arduino to apply CMDs within the sms, the GSM will be operational for specific cmds such as sw a led on or off, but it will make a call when interruption occurs and send sms to a specific number (MSSISDN)


### Installation
---
1.Raspberry pi must be connected to a sufficient 5v and aruduino is connected to it through usb port

2.Motion sensor is connected to TX RX pins in PI

3.Arduino has three leds each is connected to 

RED_LED   7   

GRN_LED   6

YEL_LED   8

also the GSM module is connected to TX RX pins in Arduino and has 4.7 power supply with rechargeable batteries

4.Burn c code on Arduino using arduino ide 
>the file sketch_may11d.ino

and just run py file on Raspberry pi remotly using putty 
>must put the folder facial_recognition along with test__.py file

and the PI settings:

ip = raspberrypi.local 

port = 22

username = raspberrypi

password = raspberry 

if the settings of the PI weren't changed


5.Some helpful images and a video for the circuit of the project to show how it operates
>  ![test__Page_1](https://github.com/MahmoodS79/GSM-Facial-Recognition-Based-Security-System/assets/87457802/b72a089a-a5c6-41a8-939f-de78c38c4a85)

>* phase 1![Phase 1](https://github.com/MahmoodS79/GSM-Facial-Recognition-Based-Security-System/assets/87457802/ce2e36a7-0130-48b0-a0d3-8749532c4f44)

>* phase 2![Phase 2](https://github.com/MahmoodS79/GSM-Facial-Recognition-Based-Security-System/assets/87457802/6879cc01-ba7f-47af-9fcf-d9b89bbdd829)

>* phase 3 ![phase 3 ](https://github.com/MahmoodS79/GSM-Facial-Recognition-Based-Security-System/assets/87457802/ea00a683-9eb3-4454-aed1-7be8335d6ded)

>[video link to demonstrate all features of the project](https://drive.google.com/file/d/1PINNDb63azDn3BRE3WM8Oy7ZxqFsNACw/view?usp=sharing)


### Usage/Examples


```c
//must use Arduino IDE to burn the .ino code on Arduino
```


```bash
#after logging into putty remotly to the PI
[root@client ~]#./test__.py
```
 