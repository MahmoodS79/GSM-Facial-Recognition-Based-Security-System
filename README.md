
# GSM & Facial Recognition Based Security System

This project will initially initialize the hw components Microcontrollers, sensors, and Pi camera inputs and outputs.

Next the code will enter a loop waiting for any interrupt through motion sensor that will make the camera record a video for 15 sec to recognize the intruder through facial recognition code if he wasn't recognized the record would be sent through specified secure email, then Pi will send a certian msg to Arduino to send cmds to the GSM module

Also, GSM role will be operational as GSM module will make a certain role if the intruder wasn't recognized

GSM module can RX certain specified cmd through SIM memory to make call, to send SMS msg .... etc(as specified in the code). any Received cmd through sms msg to GSM module will be sent to the Arduino to apply CMDs within the sms, the GSM will be operational for specific cmds such as sw a led on or off, but it will make a call when interruption occurs and send sms to a specific number (MSSISDN)


## Authors

- Amir Ragab
- Mahmood Farhat Ali




## Usage/Examples

```python
def interrupt_handling():
    '''this function is used when interrupts occur but there must  be at least 2 min between each interrupt and the next one '''
    
    global name_count,name,ext
    name_count=name_count+1
    camera.start_recording(path+name+str(name_count)+ext)
    print("Camera on")
    camera.wait_recording(10)
    camera.stop_recording()
    print("camera off")
    send_email()
```

```c
/*
 *  Function: void processCommand()
 *  Purpose:
 *    This function execute the receive commands via SMS
 *    CMD: RED 1<
 */   
void processCommand() {
  if        (strcmp(sender_cmd, "RED 1") == 0) {
    redLEDState = true;
    if (isReplyOn)
      sendSMS("Red LED On", sender_num);
  } else if (strcmp(sender_cmd, "RED 0") == 0) {
    redLEDState = false;
    if (isReplyOn)
      sendSMS("Red LED Off", sender_num);
  } else if (strcmp(sender_cmd, "GRN 1") == 0) {
    grnLEDState = true;
    if (isReplyOn)
      sendSMS("Green LED On", sender_num);
  } else if (strcmp(sender_cmd, "GRN 0") == 0) {
    grnLEDState = false;
    if (isReplyOn)
      sendSMS("Green LED Off", sender_num);
  } else if (strcmp(sender_cmd, "BLU 1") == 0) {
    yelLEDState = true;
    if (isReplyOn)
      sendSMS("Blue LED On", sender_num);
  } else if (strcmp(sender_cmd, "BLU 0") == 0) {
    yelLEDState = false;
    if (isReplyOn)
      sendSMS("Blue LED Off", sender_num);
  } else if (strcmp(sender_cmd, "ALL 0") == 0) {
    redLEDState = false;
    grnLEDState = false;
    yelLEDState = false;
    if (isReplyOn)
      sendSMS("All LED Off", sender_num);
  } else if (strcmp(sender_cmd, "ALL 1") == 0) {
    redLEDState = true;
    grnLEDState = true;
    yelLEDState = true;
    if (isReplyOn)
      sendSMS("All LED On", sender_num);
  } else if (strcmp(sender_cmd, "REP 1") == 0) {       // This command is to turn ON the reply to every sms
    isReplyOn = true;
    sendSMS("Reply is now ON", sender_num);
  } else if (strcmp(sender_cmd, "REP 0") == 0) {      // This command is to turn OFF the reply to every sms
    isReplyOn = false;
    sendSMS("Reply is now OFF", sender_num);
  } else if (strcmp(sender_cmd, "GET S") == 0) {    // This command sends the current status of all LEDs
    String statusSMS = "";                               // Create the status reply
    statusSMS = "Red LED is ";
    if (redLEDState) {
      statusSMS += "ON";
    } else statusSMS += "OFF";
    statusSMS += ". Green LED is ";
    if (grnLEDState) {
      statusSMS += "ON";
    } else statusSMS += "OFF";
    statusSMS += ". Blue LED is ";
    if (yelLEDState) {
      statusSMS += "ON";
    } else statusSMS += "OFF";
    Serial.println(statusSMS);
    sendSMS(statusSMS, sender_num);
  } 
  ```

## Installation
Raspberry pi must be connected to a sufficient 5v and aruduino is connected to it through usb port

Motion sensor is connected to TX RX pins in PI

Arduino has three leds each is connected to 
RED_LED   7   
GRN_LED   6
YEL_LED   8
also the gsm module is connected to TX RX pins in arduino and has 4.7 power supply with rechargable batteries

Install c code on Arduino and py file on Raspberry pi

 