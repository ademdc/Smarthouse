from flask import Flask, render_template
#from energenie import switch_on, switch_off
import RPi.GPIO as GPIO
import os
from temp import temperature 

ledPin = 5

GPIO.setmode(GPIO.BCM)
GPIO.setup(ledPin, GPIO.OUT)
app = Flask(__name__)


@app.route('/')
def index():
	return render_template('home.html')

@app.route('/on/')
def on():
	GPIO.output(ledPin,1)
	return render_template('home.html')

@app.route('/off/')
def off():
	GPIO.output(ledPin,0)
	return render_template('home.html')
@app.route('/temperature/')
def temp():
	return temperature()
	#return render_template('temperature.html')


if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0')
