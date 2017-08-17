from gpiozero import MotionSensor
import time

pir = MotionSensor(26)

while True: 
    pir.wait_for_motion()   
    logfile = open('test2.txt', 'a')
    now = time.strftime("%c")
    logfile.write('Motion at: '+now+"\n")
    logfile.close()
    time.sleep(3)
    