import time
import picamera

frames = 1
nFrame = 0
def filenames():
    global nFrame
    frame = 0
    while frame < frames:
        yield 'image%02d.jpg' % nFrame
        frame += 1
        nFrame += 1

camera = picamera.PiCamera()
camera.resolution = (640, 480)
camera.framerate = 30
    #camera.start_preview()
    # Give the camera some warm-up time
time.sleep(2)
#start = time.time()
def printName():
    print"Picture %s" %nFrame



while True:
    camera.capture_sequence(filenames(), use_video_port=True)
    printName()
#finish = time.time()
##print('Captured %d frames at %.2ffps' % (
##    frames,
##    frames / (finish - start)))
