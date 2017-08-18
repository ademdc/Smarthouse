from flask import Flask, render_template
app = Flask(__name__)

import RPi.GPIO as GPIO
import dht11
import time
import datetime
import calendar
from datetime import date

# initialize GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.cleanup()


instance = dht11.DHT11(pin=11)
#lastResult = instance.read()
#lastResultTemp = lastResult.temperature

# read data using pin 14
@app.route("/")
def hello():
     result = instance.read()
     return("Temperature: %d C" % result.temperature)
     time.sleep(1)
    
@app.route("/temperature/")
def temperature():
    weekday = calendar.day_name[date.today().weekday()]
    monthdate = date.today();
    result = instance.read()
    test = result.temperature

    if test < 25:
	GPIO.setmode(GPIO.BOARD)
	GPIO.setup(13,GPIO.OUT)
	GPIO.output(13,1)
	GPIO.setup(16,GPIO.OUT)
	GPIO.output(16,0)
	print "It's Cold"
    if test >= 25:
	GPIO.setmode(GPIO.BOARD)
	GPIO.setup(16,GPIO.OUT)
	GPIO.output(16,1)
	GPIO.setup(13,GPIO.OUT)
	GPIO.output(13,0)
	print "It's Hot"
    return render_template("temperature.html",temp=test, weekday=weekday, monthdate=monthdate)	

@app.route("/")
def light():
    	return render_template("home.html")	
#@app.route("/temperature")
#def test():
 #   return render_template("temperature.html")

if __name__ == "__main__":
	app.run(debug=True, host="0.0.0.0")
	



