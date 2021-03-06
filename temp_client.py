#!/usr/bin/python         
import socket
import re
import subprocess 

sensors_db = { 
	"2857993450082": "in_temp",
	"2866BC3C5006E": "out_temp"
}

ALERT_TEMP_IN_MIN = 27.5
ALERT_TEMP_IN_MAX = 29.5

print "Sensor DB"
for i in sensors_db:
	print i, sensors_db[i]
print

s = socket.socket()         
host = "localhost"
port = 5000

r = re.compile("^Epoch-Time:\s+(\d+)\s+Sensor:\s+(\w+),(\d+\.\d+),(\w+),(\d+\.\d+).*$")

s.connect((host, port))
f = s.makefile()
for i in range (0, 100) :
	f.write("a")
f.write("\n");
f.flush();

while 1:
	data = f.readline()
	m = r.match(data)
	if m :
		epochTime 	= m.group(1)
		sensorName1	= sensors_db[m.group(2)]
		sensorValue1	= float(m.group(3))
		sensorName2	= sensors_db[m.group(4)]
		sensorValue2	= float(m.group(5))

		sensor = { sensorName1: sensorValue1, sensorName2: sensorValue2 }

		rrdString = "/usr/bin/rrdtool update /www/multirPItemp.rrd --template " + sensorName1 + ":" + sensorName2 + " -- " + str(epochTime) + ":" + str(sensorValue1) + ":" + str(sensorValue2)
		print rrdString
		subprocess.call(rrdString, shell=True)

		ifttt = "/usr/local/sbin/sendIFTTTmsg.sh new_temperature_event " + str(sensor["in_temp"])
		print ifttt
		subprocess.call(ifttt, shell=True)

		if ((ALERT_TEMP_IN_MIN > sensor["in_temp"]) or (sensor["in_temp"] >= ALERT_TEMP_IN_MAX)) :
			ifttt = "/usr/local/sbin/sendIFTTTmsg.sh new_temperature_alert " + str(sensor["in_temp"])
			print ifttt
			subprocess.call(ifttt, shell=True)
		
s.close                
