from flask import Flask, render_template
app = Flask(__name__)

from gpiozero import MotionSensor
import time

pir = MotionSensor(26)

@app.route("/")
def motion():
    now = time.strftime("%c")
    movementTime = 0
    while True:
        with open('test2.txt','r') as ins:
            array = []
            for line in ins:
                array.append(line)
        #pir.wait_for_motion()
        #logfile = open('test.txt', 'a')
        #now = time.strftime("%c")
        #logfile.write('Motion at: '+now+"\n")
        #logfile.close()
        return render_template("index.html",lastmotion=array[len(array)-1])
        time.sleep(3)
        
@app.route('/shutdown', methods=['POST'])
def shutdown():
    shutdown_server()
    return 'Server shutting down...'

@app.route('/list')
def listing():
    with open('test2.txt','r') as ins:
        array = []
        for line in ins:
            array.append(line)
    return render_template("list.html",list=array)
   
if __name__ == "__main__":
	app.run(debug=True, host="0.0.0.0")