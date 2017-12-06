#!/bin/bash
rrdtool create /www/multirPItemp.rrd  --step 60 \
			DS:in_temp:GAUGE:600:-30:50 \
			DS:out_temp:GAUGE:600:-30:50 \
			RRA:AVERAGE:0.5:1:12 \
			RRA:AVERAGE:0.5:1:288 \
			RRA:AVERAGE:0.5:12:168 \
			RRA:AVERAGE:0.5:12:720 \
			RRA:AVERAGE:0.5:1440:365
