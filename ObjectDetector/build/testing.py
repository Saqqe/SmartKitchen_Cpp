import subprocess, os, glob, shutil, sys, time 

##Copy pcitures from source to destination, destination must not contain
##files with the same name as source or error will occur.
##Destination directory must exist
def copyPictures():
    temp = os.getcwd()
    os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/Milk/")
    pics = glob.glob("*.jpg")
    destination = "/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/InComing/"
    if pics:
        for pic in pics:
            shutil.copyfile(pic, destination+pic)
    os.chdir(temp)

#------------------------------------------------------#

for x in range (0, 10):
    os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/InComing")
    pics = glob.glob("*.jpg")

    if ( len(pics) >= 20 ):
        os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build")
        print "Pics in folder"
        os.popen("./main /home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/InComing/*.jpg")
    else:
        print "To few pics in folder"
        copyPictures()
