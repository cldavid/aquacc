#!/usr/bin/python         
import serial
from serial import SerialException

import re
import subprocess 
import time
import sys

readinterval 	= 60
if len(sys.argv) > 1:
	usbport 	= sys.argv[1]
else:
	usbport 	= "/dev/tty.usbserial-DA004IM3"

if __name__ == "__main__":
    print("Aquacc pH controller script using Atlas Scientific pH module")
    print("Pressing ctrl-c will stop the script")

def read_data():
    line = ""
    try:
        s_time = time.time()
        n = 0
        while True:
            data = ser.read()
            if data == "\r":		# Atlas sensors are sending CR at the end of every response.
                return line
            else:
                line = line + data
            n += 1
            if time.time() - s_time > ser.timeout:
                # print "UART reading timeout."
                return None

    except SerialException as e:
        print "Error, ", e
        return None

print "Trying to open serial port: " + usbport
try:
	ser = serial.Serial(usbport, 38400, timeout=3, rtscts=1) 
except serial.SerialException as e:
	print "Error, ", e
	sys.exit(0)

ser.write("e\r")
while True:
	ser.write("r\r")
	data = read_data()
	if data is not None:
		print data
	time.sleep(readinterval)

ser.close                
