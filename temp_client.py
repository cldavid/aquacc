#!/usr/bin/python         
import socket
import re
import subprocess 

ret = subprocess.call("/bin/ls -al", shell=True)
print ret
sensors_db = { 
	"285A2B3C50018": "in_temp",
	"2866BC3C5006E": "out_temp"
}

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
while 1:
	data = f.readline()
	m = r.match(data)
	if m :
		epochTime 	= m.group(1)
		sensorName1	= sensors_db[m.group(2)]
		sensorValue1	= m.group(3)
		sensorName2	= sensors_db[m.group(4)]
		sensorValue2	= m.group(5)
		rrdString = "/usr/bin/rrdtool update /www/multirPItemp.rrd --template " + sensorName1 + ":" + sensorName2 + " -- " + str(epochTime) + ":" + str(sensorValue1) + ":" + str(sensorValue2)
		print rrdString
		subprocess.call(rrdString, shell=True)
s.close                
