
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
from flask import Flask, render_template
import os
import time
import datetime
from temp import temperature

red_pin = 13
green_pin = 15
blue_pin = 16
GPIO.setup(red_pin, GPIO.OUT)
GPIO.setup(green_pin, GPIO.OUT)
GPIO.setup(blue_pin, GPIO.OUT)
GPIO.output(red_pin, False)
GPIO.output(green_pin, False)
GPIO.output(blue_pin, False)
app = Flask(__name__)

@app.route('/')
def index():
	return render_template('home.html')

@app.route('/red/')
def red():
            GPIO.output(red_pin, True)
            GPIO.output(green_pin, False)
  	    GPIO.output(blue_pin, False)
            time.sleep(5)
	    GPIO.output(red_pin, False)
            return render_template('home.html')
@app.route('/blue/')
def green():
            GPIO.output(red_pin, False)
            GPIO.output(green_pin, False)
            GPIO.output(blue_pin, True)
            time.sleep(5)
            GPIO.output(blue_pin, False)
            return render_template('home.html')
	
@app.route('/temperature/')
def temp():
	return temperature()

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
