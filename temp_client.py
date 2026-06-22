#!/usr/bin/env python3         
import socket
import re
import subprocess 

sensors_db = { 
    "2857993450082": "in_temp",
    "2866BC3C5006E": "out_temp"
}

ALERT_TEMP_IN_MIN = 27.5
ALERT_TEMP_IN_MAX = 29.5

print("Sensor DB")
for i in sensors_db:
    print(i, sensors_db[i])
print()

s = socket.socket()         
host = "localhost"
port = 5000

r = re.compile(r"^Epoch-Time:\s+(\d+)\s+Sensor:\s+(\w+),(\d+\.\d+),(\w+),(\d+\.\d+).*$")

s.connect((host, port))

# FIX: Create separate text-mode handles for reading and writing
f_read = s.makefile('r', encoding='utf-8', errors='ignore', buffering=1)
f_write = s.makefile('w', encoding='utf-8', buffering=1)

# Write to the write-handle
for i in range(0, 100):
    f_write.write("a")
f_write.write("\n")
f_write.flush()

# Read from the read-handle
while True:
    data = f_read.readline()
    if not data:
        break # Handle socket disconnection gracefully
        
    m = r.match(data)
    if m:
        epochTime    = m.group(1)
        sensorName1  = sensors_db[m.group(2)]
        sensorValue1 = float(m.group(3))
        sensorName2  = sensors_db[m.group(4)]
        sensorValue2 = float(m.group(5))

        sensor = {sensorName1: sensorValue1, sensorName2: sensorValue2}

        rrdString = f"/usr/bin/rrdtool update /www/multirPItemp.rrd --template {sensorName1}:{sensorName2} -- {epochTime}:{sensorValue1}:{sensorValue2}"
        print(rrdString)
        subprocess.call(rrdString, shell=True)

        ifttt = f"/usr/local/sbin/sendIFTTTmsg.sh new_temperature_event {sensor['in_temp']}"
        print(ifttt)
        subprocess.call(ifttt, shell=True)

        if (ALERT_TEMP_IN_MIN > sensor["in_temp"]) or (sensor["in_temp"] >= ALERT_TEMP_IN_MAX):
            ifttt = f"/usr/local/sbin/sendIFTTTmsg.sh new_temperature_alert {sensor['in_temp']}"
            print(ifttt)
            subprocess.call(ifttt, shell=True)
		
# Close both handles and the socket
f_read.close()
f_write.close()
s.close()
