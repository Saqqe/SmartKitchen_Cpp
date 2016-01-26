import picamera, time, subprocess, os
#Set up for the camera
#cam = picamera.PiCamera()
#cam.resolution = (640, 480)
picCounter = 0

#Camera warm-up time
time.sleep(2)
print"ready"
#os.system("raspistill -o test.jpg")

while True:
    #imageName = time.strftime("%Y%m%d%H%M%S-")
    imageName='test'
##    #cam.capture(imageName+str(picCounter)+'.jpg')
    #print"Picture %s" %picCounter
    filename = '/home/pi/ownDev/Camera/PicturesTaken/'+imageName+str(picCounter)+'.jpg'
    subprocess.call("raspistill -w 640 -h 480 -t 1  -o %s" %filename, shell = True)
    picCounter = picCounter + 1
##    #weightDetection()
    #subprocess.call("raspistill -h 480 -w 640 -q 15 -o test.jpg -t 10", shell =True)
##    #os.system("raspistill -o test.jpg")
    #process = subprocess.Popen('raspistill -t 10 -o test.jpg', shell = True, stdout=subprocess.PIPE)
    #print"Test"
    #ime.sleep(0.5)
