import picamera, time, os

cam = picamera.PiCamera()
imageName = 'Test'

cam.resolution = (640, 480)

#Camera warm-up time
picCounter = 0

time.sleep(2)

def stopLoop():
    global picCounter
    try:
        while True:
            picCounter = 0
    except KeyboardInterrupt:
        pass
def testCpp():
    f = os.popen('./HelloRpi')
    message = f.read()
    print"Test says: ", message

def startLoop():
    global picCounter
    try:
        while True:
            cam.capture(imageName+str(picCounter)+'.jpg')
            print"Picture %s" %picCounter
            picCounter = picCounter + 1
            time.sleep(0.2)
    except KeyboardInterrupt:
        testCpp()

while True:
    stopLoop()
    startLoop()
