import picamera, time
from SimpleCV import*

cam = picamera.PiCamera()


cam.resolution = (640, 480)
#cam.vflip = True
cam.start_preview()

raw_input("Press Enter to close...")

cam.stop_preview()
cam.close()
