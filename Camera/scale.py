import serial, io, time, os, shutil

#Set up the GPIO
import RPi.GPIO as GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(36, GPIO.OUT)
GPIO.setup(38, GPIO.OUT)
#Reset pin, if true nothing happens, if false, the Arduino reboots
GPIO.output(38, True)
#Call data pin, if false nothing happens, if true the Arduino sends data
GPIO.output(36, False)

#To read incoming data from Arduino
def readArduino():
    while True:
        try:
            GPIO.output(36, True)
            arduinoData = float(ser.readline())
            GPIO.output(36, False)
        except ValueError:
            continue
        break

    return arduinoData

#Reboot for the Arduino
def rebootArduino():
    GPIO.output(38, False)
    time.sleep(0.1)
    GPIO.output(38, True)
    time.sleep(3)

#Need to check this name with command: "ls /dev/tty*"
arduinoName = '/dev/ttyACM1'

#This rate need to be set in Arduino program
baudRate    = 9600

#Init for weight part of the script
ser = serial.Serial(arduinoName, baudRate)
weight = 0
initialWeight = readArduino()
#print initialWeight
weightStart = True
startWeight = 0
finalWeight = 0

#Detects the weight and determines if the weight has increased or decreased
#and moves it to the correct directory
def weightDetection():
    global initialWeight, weightStart, startWeight, finalWeight, picCounter

    weight = readArduino()
    print weight
    time.sleep(1)
    
while True:
    weightDetection()
