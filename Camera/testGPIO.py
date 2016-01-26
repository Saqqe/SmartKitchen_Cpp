import RPi.GPIO as GPIO
import time
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)

GPIO.setup(33, GPIO.OUT)
GPIO.setup(36, GPIO.OUT)
GPIO.setup(40, GPIO.IN)

##while True:
##    print "HIGH"
##    #GPIO.output(33, True)
##    GPIO.output(38, True)
##    test = GPIO.input(40)
##    print "Test is %s" %test
##    time.sleep(1)
##    print "LOW"
##    #GPIO.output(33, False)
##    GPIO.output(38, False)
##    test = GPIO.input(40)
##    print "Test is %s" %test
##    time.sleep(1)
    
##while True:
##    GPIO.output(38, True)
##    time.sleep(3)
##    GPIO.output(38, False)
##    time.sleep(0.1)
