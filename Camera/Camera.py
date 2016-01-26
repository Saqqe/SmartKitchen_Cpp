import picamera, time
#from SimpleCV import*


cam = picamera.PiCamera()
imageName = 'Test'

cam.resolution = (640, 480)
#cam.vflip = True
cam.start_preview()

#Camera warm-up time
time.sleep(2)

#Wait for user input
raw_input("Press Enter to continue...")

#Take n: pics
for i in range(0,30):
    cam.capture('BMP/InComing/'+imageName+str(i+30)+'.bmp')

#While loop


print 'stop'

#Shutdown cam
cam.stop_preview()
cam.close()



