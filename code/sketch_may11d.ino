
 /*        Send command in this format:
 *            CMD: RED 1<
 *              - turn ON the red LED
 *            CMD: RED 0<
 *              - turn OFF the red LED
 *            CMD: GRN 1<
 *              - turn ON the green LED
 *            CMD: GRN 0<
 *              - turn OFF the green LED
 *            CMD: BLU 1<
 *              - turn ON the blue LED
 *            CMD: BLU 0<
 *              - turn OFF the blue LED
 *            CMD: ALL 1<
 *              - turn ON all the LED
 *            CMD: ALL 0<
 *              - turn OFF all the LED
 *            CMD: REP 1<
 *              - enable the reply for every SMS
 *            CMD: REP 0<
 *              - disable the reply, but will still reply to the following:
 *                CMD: REP 1<
 *                CMD: REP 0<
 *                CMD: GET S<
 *                and the NUM: function sending a reply after changing the default number
 *            CMD: GET S<
 *              - sends the current states of the LEDs
 *            NUM: +ZZxxxxxxxxxx<
 *              - changes the current default number
 *                ZZ - country code
 *                xx - 10-digit number
 */

 //the project is concerned with one command at a time and one message at a time
#include "SoftwareSerial.h"
SoftwareSerial SIM800L(5, 4);  // Arduino D5(as Rx pin) to GSM_Arduino Tx pin
                                 // Arduino D4(as Tx pin) to GSM_Arduino Rx pin
#include <TimerOne.h>                            
#define RED_LED   7   // Pin assignments for LED and Switch
#define GRN_LED   6
#define YEL_LED   8
#define BUFFER_SIZE   127       // incoming data buffer(memory) size

char bufferData[BUFFER_SIZE];   // holds the char data all the statements Ex: +CMT= "+201554931647"..................... and so on
char bufferIndex;               // holds the position of the char in the buffer (the index of each char in the array)

// This variable holds the SMS sender mobile number, it will be updated when new sms arrived
char sender_num[14]="+201554931647" ;//holds in the last index the '\0'

// This variable holds the mobile number of the owner, emergency sms will be sent to this
char default_num[14]="+201201514813";

char sender_cmd[7];     // holds the specific command when a new SMS is received   Ex:CMD RED 1< (<:the end char)

bool CMT_OK = false;    // CMT is for processing the SMS senders mobile number
int cmt_pos_index = 0;  
int cmt_pos_count = 0;

bool CMD_OK = false;    // CMD is for processing the parsing the sms command
int cmd_pos_index = 0;
int cmd_pos_count = 0;

bool NUM_OK = false;    // NUM is for processing the CHANGE NUMBER option
int num_pos_index = 0;
int num_pos_count = 0;

bool redLEDState = false;   // holds the state of LEDs
bool grnLEDState = true;   //    true:  means ON
bool yelLEDState = false;   //    false: means OFF

bool isReplyOn = true;     // hold the settings for sending the status update for every sms 
                            //    true:  reply to every sms
                            //    false: reply only to some sms request like  CMD: REP 1<
                                                //                CMD: REP 0<
                                               //             CMD: GET S<
                                               //           and the NUM: function sending a reply after changing the default number
                            
void setup(){
  Serial.begin(9600);
  Serial.println("Initializing...");//just print on the monitor of the Arduino_computer
  SIM800L.begin(9600);
  delay(2000);      // gives enough time for the GSM module to bootup  (very problematic)
  initializeGSM();  // set the necessary initializations for the GSM module
  pinMode(RED_LED, OUTPUT);           // set the pin directions of other pins
  pinMode(GRN_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  
  digitalWrite(RED_LED, redLEDState); // set the current states of LEDs
  digitalWrite(GRN_LED, grnLEDState);
  digitalWrite(YEL_LED, yelLEDState);
  memset(bufferData, 0, sizeof(bufferData)); // Initialize the string     
  bufferIndex = 0; 
  Serial.println("Setup is done");
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(interrupt_handling);

}
void loop(){
  manageSIM800();
  delay(500);
}

/*
 *  Function: void initializeGSM()
 *  Purpose:
 *    This function is for the SIM800L initializations
 */
void initializeGSM() {
#ifdef SIMULATE_BY_SERIAL   //GSM is listening,watching and waiting but he is just watching Arduino_computer communicating with him through one direction communication(gsm receives commands from the Arduino_Computer)
                            // one way communication GSM watching but he can understand what is happening(the language of gsm)
  Serial.println("AT");                   // Sends an ATTENTION command, reply should be OK
  Serial.println("AT+CMGF=1");            // Configuration for sending SMS
  Serial.println("AT+CNMI=1,2,0,0,0");    // Configuration for receiving SMS
  Serial.println("AT&W");                 // Save the configuration settings
#else // one way communication arduino_computer watching but they cannot understand what is happening(the language of gsm)
  SIM800L.println("AT");                  // Sends an ATTENTION command, reply should be OK
  delay(1000);
  SIM800L.println("AT+CMGF=1");           // Configuration for sending SMS
  delay(1000);
  SIM800L.println("AT+CNMI=1,2,0,0,0");   //ready to receive messages and ready to give them numbers ,mark as unsoilcited for unread messages, store the number of the sender,date & time
                                              //it prints the message automatically
  delay(1000);
  Serial.println("AT&W");                 // Save the configuration settings
  delay(1000);
#endif
}
/*
 *  Function: void manageSIM800()
 *  Purpose:
 *    This is for parsing the incoming serial data from SIM800L
 *    so that we can use it for controlling something.
 */
void manageSIM800() {
#ifdef SIMULATE_BY_SERIAL    //if you want to communicate with Arduino_Computer and GSM with arduino as middle man 
  while (Serial.available() > 0) { //the while works only one at a time(either the NUM OR THE CMT&CMD)   
                                      //the commands won't appear on serial monitor because there is no Serial.println()
    bufferData[bufferIndex] = Serial.read();
    // Find the string "CMD:"
    //we are just searching for the string CMD in the bufferdata as it has the whole statement  &&&& storing the command
    //*************************2***************************
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'D') && 
        (bufferData[bufferIndex] == ':')      )  {               
      Serial.println("CMD"); 
      CMD_OK = true;      
      memset(sender_cmd, 0, sizeof(sender_cmd));
      cmd_pos_index = bufferIndex;  // get the position ...... it has the position of : and counts the chars zero as it is concerned with the command only
      cmd_pos_count = 0;            // reset pos counter    
    }    
    // Finds the string "NUM:"  NUM: 01554931647<
    if( //(bufferData[bufferIndex-3] == 'N') &&
        //(bufferData[bufferIndex-2] == 'U') && 
        (bufferData[bufferIndex-1] == 'M') && 
        (bufferData[bufferIndex] == ':')      )  {              
      Serial.println("NUM"); 
      NUM_OK = true;
      memset(default_num, 0, sizeof(default_num));
      num_pos_index = bufferIndex;  // get the position ...... it has the position of : and counts the chars zero as it is concerned with the default number only
      num_pos_count = 0;            // reset pos counter  
      
    } 
    //we are just searching for the string CMT in the bufferdata as it has the whole statement  &&&& storing the sender number
    // Finds the string "CMT:"
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'T') && 
        (bufferData[bufferIndex] == ':')      )  {            
      //Serial.println("CMT");  
      CMT_OK = true;
      memset(sender_num, 0, sizeof(sender_num));    
      cmt_pos_index = bufferIndex;  // get the position ...... it has the position of : and counts the chars zero as it is concerned with the sender number only
      cmt_pos_count = 0;            // reset pos counter 
    }    
    // String "CMT:" is found, 
    //  store the sender number for the reply
    // +CMT: "+201554931647"
    if ( ( (bufferIndex - cmt_pos_index) > 2 /*=(3)*/ ) && (CMT_OK) ) {//we are not changing cmt_pos_index
      if ( cmt_pos_count < 13 ) {
        sender_num[cmt_pos_count] =  bufferData[bufferIndex];
        cmt_pos_count++;
      } else {
        Serial.println(sender_num);
        CMT_OK = false; // done
      }
    }
    
    // String "NUM:" is found
    //  parse it then save it as default number then send a reply
    //NUM: +201201514813<
    if ( ( (bufferIndex - num_pos_index) > 1 /*(=2)*/) && (NUM_OK) ) {
      if ( bufferData[bufferIndex] != '<' ) {
        default_num[num_pos_count] =  bufferData[bufferIndex];
        num_pos_count++;
      } else {
        char msg[32] = "Default # is now ";
        sendSMS((strcat( msg, default_num )), sender_num);                          // send it to the recipient number.
        NUM_OK = false; // done
        break;
      }
    } 
    // String "CMD:" is found,
    //  parse the command, then execute the command
    //  CMD: GRN 0<
    if ( ( (bufferIndex - cmd_pos_index) > 1 ) && (CMD_OK)) {
      if (bufferData[bufferIndex] == '<') {
        Serial.println(sender_cmd);
        processCommand();
        CMD_OK = false; // done
        break;
      } else {
        sender_cmd[cmd_pos_count] =  bufferData[bufferIndex];
        cmd_pos_count++;
      }
    } 
    bufferIndex++;                              
  }           

  memset(bufferData, 0, sizeof(bufferData));    // Initialize the string     
  bufferIndex = 0;
#else
  while (SIM800L.available() > 0) {
    bufferData[bufferIndex] = SIM800L.read();
    // Find the string "CMD:"
    //we are just searching for the string CMD in the bufferdata as it has the whole statement  &&&& storing the command
    //    if found, atore the command for processing
    /*************************2***************************/
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'D') && 
        (bufferData[bufferIndex] == ':')      )  {               
      Serial.println("CMD");//no communication in this case between GSM_computer but it is just Arduino_COMPUTER 
      CMD_OK = true;      
      memset(sender_cmd, 0, sizeof(sender_cmd));
      cmd_pos_index = bufferIndex;  // get the position ...... it has the position of : and counts the chars zero as it is concerned with the command only
      cmd_pos_count = 0;            // reset pos counter   
    }    
    // Finds the string "NUM:"      NUM: 01554931647<
    //optional to change the main number
    if( //(bufferData[bufferIndex-3] == 'N') &&
        //(bufferData[bufferIndex-2] == 'U') && 
        (bufferData[bufferIndex-1] == 'M') && 
        (bufferData[bufferIndex] == ':')      )  {              
      Serial.println("NUM"); 
      NUM_OK = true;
      memset(default_num, 0, sizeof(default_num));
      num_pos_index = bufferIndex;  // get the position...... it has the position of : and counts the chars zero as it is concerned with the default number only
      num_pos_count = 0;            // reset pos counter 
    } 
    //we are just searching for the string CMT in the bufferdata as it has the whole statement  &&&& storing the sender number
    // Finds the string "CMT:"
    //    if found, parse the command for processing
     /*************************1***************************/
    if( //(bufferData[bufferIndex-3] == 'C') && 
        //(bufferData[bufferIndex-2] == 'M') && 
        (bufferData[bufferIndex-1] == 'T') && 
        (bufferData[bufferIndex] == ':')      )  {            
      Serial.println("CMT");  
      CMT_OK = true;
      memset(sender_num, 0, sizeof(sender_num));    
      cmt_pos_index = bufferIndex;  // get the position...... it has the position of : and counts the chars zero as it is concerned with the sender number only
      cmt_pos_count = 0;            // reset pos counter 
    }    
    // String "CMT:" is found, 
    //  parse the sender number for the reply
    // +CMT: "+201554931647"
    /*************************3***************************/
    if (( ( (bufferIndex - cmt_pos_index) > 2 /*(=3)*/) ) && (CMT_OK) ) {//we are not changing cmt_pos_index
      if ( cmt_pos_count < 13 ) {
        sender_num[cmt_pos_count] =  bufferData[bufferIndex];
        cmt_pos_count++;
      } else {
        Serial.println(sender_num);
        CMT_OK = false; // done
        break;
      }
    }
    // String "NUM:" is found
    //  store it then save it as default number then send a reply
    //NUM: +201201514813<
    if ( ( (bufferIndex - num_pos_index) > 1 ) && (NUM_OK) ) {
      if ( bufferData[bufferIndex] != '<' ) {
        default_num[num_pos_count] =  bufferData[bufferIndex];
        num_pos_count++;
      } else {
        char msg[32] = "Default # is now ";
        sendSMS((strcat( msg, default_num )), sender_num);
        delay(1000);
        Serial.println(strcat( msg, default_num ));// send it to the recipient number.
        delay(1000);
        NUM_OK = false; // done
        break;
      }
    } 
    // String "CMD:" is found,
    //  parse the command, then execute the command
    //  CMD: GRN 0<
     /*************************4 & final stage for one command at a time***************************/
    if ( ( (bufferIndex - cmd_pos_index) > 1 /*(=2)*/ ) && (CMD_OK)) {
      if (bufferData[bufferIndex] == '<') {
        Serial.println(sender_cmd);
        processCommand();
        CMD_OK = false; // done
        break;
      } else {
        sender_cmd[cmd_pos_count] =  bufferData[bufferIndex];
        cmd_pos_count++;
      }
    } 
    bufferIndex++;                              
  }           
  memset(bufferData, 0, sizeof(bufferData));    // Initialize the string     
  bufferIndex = 0;
#endif //SIMULATE_BY_SERIAL
}

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
  digitalWrite(RED_LED, redLEDState);   // Sets the LED states according
  digitalWrite(GRN_LED, grnLEDState);   //  according to the received
  digitalWrite(YEL_LED, yelLEDState);   //  commands

}

/*
 *  Function: void sendSMS(String message, String mobile)
 *  Purpose:
 *    This function is use for sending the reply SMS
 */
void sendSMS(String message, String mobile) {
#ifdef SIMULATE_BY_SERIAL
  Serial.println("AT+CMGS=\"" + mobile + "\"\r"); //Mobile phone number to send message
  Serial.println(message);                        // This is the message to be sent.
  Serial.println((char)26);                       // ASCII code of CTRL+Z to finalized the sending of sms
#else 
  //SIM800L.println("AT+CMGF=1");                    //Sets the GSM Module in Text Mode
  //delay(1000);
  SIM800L.println("AT+CMGS=\"" + mobile + "\"\r"); //Mobile phone number to send message
  delay(1000);
  SIM800L.println(message);                        // This is the message to be sent.
  delay(1000);
  SIM800L.println((char)26);                       // ASCII code of CTRL+Z to finalized the sending of sms
  delay(1000);
  #endif
}
void interrupt_handling(){
  char communication[10];
  byte comm_count=0;
  while(Serial.available() && Serial.read()!='\n') {
    communication[comm_count]=Serial.read();
    comm_count++;
    }
  if(communication[comm_count-5]=='s' && communication[comm_count-4]=='t')
      call(default_num);

}
  

void call(String mobile){
  SIM800L.println("AT"); //Once the handshake test is successful, it will back to OK
  delay(1000);
  SIM800L.println("ATD"+mobile+";");//call a specific number 
  delay(10000); // wait for 20 seconds...
  SIM800L.println("ATH"); //hang up
  delay(1000);
  initializeGSM();
  }
