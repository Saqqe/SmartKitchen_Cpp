#Author: Fredrik Edlund & Saqib Sarker
#Version: 20150917
#Starts Rpi camera, (needs 2 seconds varm up) and after keypress
#takes 5 pictures/s and moves these to a different directory
#each picture has a unique name
import serial, io, time, os, shutil, picamera, subprocess

#Set up the GPIO
import RPi.GPIO as GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(40, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

#Set up for the camera
cam = picamera.PiCamera()
cam.resolution = (640, 480)
cam.framerate = 30
picCounter = 0
fileFormat = '.bmp'
frames = 1
nFrame = 0
def filenames():
    global picCounter
    frame = 0
    imageName = time.strftime("%Y%m%d-%H%M%S-")
    filename = '/home/pi/ownDev/Camera/PicturesTaken/'+imageName+str(picCounter)+fileFormat
    while frame < frames:
        yield filename
        frame += 1
        picCounter += 1

#start script for weight AND for newSVM
subprocess.Popen("sudo python /home/pi/ownDev/Camera/weightDifference.py", shell = True)
##subprocess.Popen("sudo python /home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/UnknownPics/TrainNewObject.py", shell = True)
#Camera warm-up time
time.sleep(2)
print"Ready"

#Moves pcitures from source to destination, destination must not contain
#files with the same name as source or error will occur.
#Destination directory must exist
def movePictures(moveDestination):

    os.chdir("/home/pi/ownDev/Camera/PicturesTaken")
    source = os.listdir("/home/pi/ownDev/Camera/PicturesTaken")
    for files in source:
        if files.endswith(fileFormat):
            shutil.move(files,moveDestination)
    os.chdir("/home/pi/ownDev/Camera")
            
#Deletes all .jpg files from the targeted directory
#Can be modified to other file types or directories
def removePictures(removeDestination):
    global picCounter
    os.chdir(removeDestination)
    for i in range (0, picCounter):
        filelist = [f for f in os.listdir(".") if f.endswith(fileFormat) ]
        for f in filelist:
            os.remove(f)
    #change back to the previous directory
    os.chdir("/home/pi/ownDev/Camera")

#Example for starting C++ program, needs modifiacation
#Currently only prints Hello world message from HelloRpi.cpp
def startRecognition():
    pRecognition = subprocess.Popen('sudo python recognition.py', shell = True)
    #f = os.popen('./HelloRpi')
    #message = f.read()
    #print"Run C++: ", message

def detectPicture():
    os.chdir("/home/pi/Downloads/dlib-18.17/examples/build")
    process = subprocess.Popen('./train_object_detector /home/pi/ownDev/Camera/PicturesTaken/*' + fileFormat, shell =False, stdout=subprocess.PIPE)
    #process.wait()
    #print process.returncode
    os.chdir("/home/pi/ownDev/Camera")

#Startloop, starts taking pictures with the camera every 0.2s
#each pictures is named "year month day hour minute second picCounter
#Started by pressing ctrl + c and aborted with the same press
#once stopped, moves the pictures to a different directory and
#executes a C++ program

def mainProgram():
    global picCounter
    doorOpen = GPIO.input(40)
    if (doorOpen == 0):
        if(picCounter == 0):
            print"Door open!"
        cam.capture_sequence(filenames(), use_video_port=True)
        time.sleep(0.05)

        #print"Picture %s" %picCounter
    if(doorOpen != 0 and picCounter != 0):
        startRecognition()
        print"Door closed!"
        removePictures("/home/pi/ownDev/Camera/PicturesTaken")
        picCounter = 0        
        
#Main, loops between start and stop
while True:
    mainProgram()
