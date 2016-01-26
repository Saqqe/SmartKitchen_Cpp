import subprocess, os

os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/")

#if list of dir is not empty, run main program
dir = "/home/pi/ownDev/Camera/InComing/"
if os.listdir(dir):
    subprocess.call('./main /home/pi/ownDev/Camera/InComing/*.bmp', shell = True)


#if list of dir is not empty, run main program
dir = "/home/pi/ownDev/Camera/OutGoing/"
if os.listdir(dir):    
    subprocess.call('./main /home/pi/ownDev/Camera/OutGoing/*.bmp', shell = True)


print "Processing complete"
