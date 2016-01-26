#Author: Fredrik Edlund & Saqib Sarker
#Version: 20150917
#Starts Rpi camera, (needs 2 seconds varm up) and after keypress
#takes 5 pictures/s and moves these to a different directory
#each picture has a unique name

import picamera, time, os, subprocess, shutil, serial, io

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
    GPIO.output(36, True)
    arduinoData = ser.readline()
    GPIO.output(36, False)
    return arduinoData

#Reboot for the Arduino
def rebootArduino():
    GPIO.output(38, False)
    time.sleep(0.1)
    GPIO.output(38, True)
    time.sleep(3)


#Set up for the camera
cam = picamera.PiCamera()
cam.resolution = (640, 480)
cam.framerate = 30
picCounter = 0

frames = 1
nFrame = 0
def filenames():
    global picCounter
    frame = 0
    imageName = time.strftime("%Y%m%d%H%M%S-")
    filename = '/home/pi/ownDev/Camera/PicturesTaken/'+imageName+str(picCounter)+'.jpg'
    while frame < frames:
        yield filename#'image%02d.jpg' % picCounter
        frame += 1
        picCounter += 1

#Need to check this name with command: "ls /dev/tty*"
arduinoName = '/dev/ttyACM0'

#This rate need to be set in Arduino program
baudRate    = 38400

#Init for weight part of the script
ser = serial.Serial(arduinoName, baudRate)
weight = 0
initialWeight = readArduino()
weightStart = True
startWeight = 0
finalWeight = 0

#Camera warm-up time
time.sleep(2)


#Moves pcitures from source to destination, destination must not contain
#files with the same name as source or error will occur.
#Destination directory must exist
def movePictures(moveDestination):
    os.chdir("/home/pi/ownDev/Camera/PicturesTaken")
    source = os.listdir("/home/pi/ownDev/Camera/PicturesTaken")
    for files in source:
        if files.endswith(".jpg"):
            shutil.move(files,moveDestination)
    os.chdir("/home/pi/ownDev/Camera")
            
#Deletes all .jpg files from the targeted directory
#Can be modified to other file types or directories
def removePictures(removeDestination):
    global picCounter
    os.chdir(removeDestination)
    for i in range (0, picCounter):
        filelist = [f for f in os.listdir(".") if f.endswith(".jpg") ]
        for f in filelist:
            os.remove(f)
    #change back to the previous directory
    os.chdir("/home/pi/ownDev/Camera")

#Example for starting C++ program, needs modifiacation
#Currently only prints Hello world message from HelloRpi.cpp
def startRecognition():
    f = os.popen('./HelloRpi')
    message = f.read()
    print"Run C++: ", message    

def detectPicture():
    os.chdir("/home/pi/Downloads/dlib-18.17/examples/build")
    process = subprocess.Popen('./train_object_detector /home/pi/ownDev/Camera/PicturesTaken/*.jpg', shell =True, stdout=subprocess.PIPE)
    #process.wait()
    #print process.returncode
    os.chdir("/home/pi/ownDev/Camera")

#Detects the weight and determines if the weight has increased or decreased
#and moves it to the correct directory
def weightDetection():
    global initialWeight, weightStart, startWeight, finalWeight, picCounter

    weight = readArduino()
    #print"Weight %s" %weight
    weightDifference = abs(float(initialWeight) - float(weight))
    #print"Weightdifference %s" %weightDifference
    if(weightDifference >2 and weightStart == True):
        startWeight = float(weight)
        weightStart = False
        #print"Start"
        
    if(weightDifference < 2 and weightStart == False):

        finalWeight = float(weight)
        weightStart = True
        
        if (startWeight > finalWeight):
            movePictures("/home/pi/ownDev/Camera/OutGoing/")
            picCounter = 0

        if (startWeight < finalWeight):
            movePictures("/home/pi/ownDev/Camera/InComing/")
            picCounter = 0

    initialWeight = weight

#Startloop, starts taking pictures with the camera every 0.2s
#each pictures is named "year month day hour minute second picCounter
#Started by pressing ctrl + c and aborted with the same press
#once stopped, moves the pictures to a different directory and
#executes a C++ program
def startLoop():
    global picCounter
    try:
        while True:
            
            #cam.capture(imageName+str(picCounter)+'.jpg')
            cam.capture_sequence(filenames(), use_video_port=True)
            print"Picture %s" %picCounter
            #picCounter = picCounter + 1
            weightDetection()
            #time.sleep(0.1)
    except KeyboardInterrupt:

       # movePictures("/home/pi/ownDev/Camera/PicturesTaken/")
        #detectPicture()
        startRecognition()
        removePictures("/home/pi/ownDev/Camera/PicturesTaken")

#Stoploop, resets picCounter to zero and then nothing happens until
#the next press of ctrl + c
def stopLoop():
    global picCounter
    try:
        while True:
            picCounter = 0
    except KeyboardInterrupt:
        pass

#Main, loops between start and stop
while True:
    print"Ready"
    stopLoop()
    startLoop()
