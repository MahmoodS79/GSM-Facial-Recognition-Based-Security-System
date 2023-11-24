#import libraries
import smtplib, email.utils
from email.mime.base import MIMEBase
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email import encoders

import RPi.GPIO as GPIO
import time
from picamera import PiCamera
import serial

#creating objects for managing serial communication through USB
if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0',9600, timeout=1)
    ser.flush()
    
led = 40
sensor=7
#Initializing GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.setup(led,GPIO.OUT)
GPIO.setup(sensor,GPIO.IN,pull_up_down=GPIO.PUD_UP)

#camera Basic initialization
camera = PiCamera()
camera.resolution = (1024,720)
camera.brightness = 70
path="/home/pi/Desktop/"
name="test"
ext=".h264"
name_count=0
def name_of_video():
    name_count=name_count+1
    name=name+str(name_count)
    
def pin7_handler():
    if ((time.time()-prev_time)>120 or Interrupte_count==0):
        prev_state=GPIO.input(sensor)
        prev_time=time.time()
        Interrupte_count=Interrupte_count+1
        interrupt_handling()
    
#capturing movemento

prev_state=GPIO.input(sensor)
prev_time=time.time()
Interrupte_count=0
GPIO.add_event_detect(sensor,GPIO.RISING,pin7_handler())

        


def interrupt_handling():
    name_of_video()
    camera.start_recording(path+name+ext)
    print("Camera on")
    camera.wait_recording(10)
    camera.stop_recording()
    print("camera off")
    send_email()
 




#*********************************************** Email parameters *************************************************
subject='Security Alert: A motion has been detected'
bodyText="""\
Hi,
A motion has been detected in your room.
Please check the attachement sent from rasperry pi security system.
Regards
AS Tech-Workshop
"""
SMTP_SERVER='smtp.gmail.com'
SMTP_PORT=587
USERNAME='mahmood7ali9@gmail.com'
PASSWORD='Sence@79'
RECIEVER_EMAIL='sence79.s7.s9@gmail.com'    
def send_email():
     #holds the body, recipient, transmitter and header
     message=MIMEMultipart()
     message["From"]=email.utils.formataddr("PI",USERNAME)
     message["To"]=email.utils.formataddr("Recipient",RECIEVER_EMAIL)
     message["Subject"]=subject
     message.attach(MIMEText(bodyText, 'plain'))
     
     #holds the path, the name and the extension of the picture or video
     attachment=open(path+name+ext, "rb")
    
     #encodes and encrypts the picture or the video   
     mimeBase=MIMEBase('application','octet-stream')
     mimeBase.set_payload((attachment).read())
     encoders.encode_base64(mimeBase)
     mimeBase.add_header('Content-Disposition', "attachment; filename= " +(name+ext))
    
    #transforming data into bytes and bits and attaching it to message
     message.attach(mimeBase)
     text=message.as_string()
    
     #sends the email to the recipient and managing the sending and the allowance 
     server=smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
     server.ehlo()
     server.starttls()
     server.ehlo()
     server.login(USERNAME, PASSWORD)
     server.sendmail(USERNAME, RECIEVER_EMAIL, text)
     server.quit
     print("Email sent")


    
while 1:
    # time in seconds
    time.sleep(1)
        