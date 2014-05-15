#!/bin/bash
rrdtool create multirPItemp.rrd  --step 300 \
			DS:in_temp:GAUGE:600:-30:50 \
			DS:out_temp:GAUGE:600:-30:50 \
			RRA:AVERAGE:0.5:1:12 \
			RRA:AVERAGE:0.5:1:288 \
			RRA:AVERAGE:0.5:12:168 \
			RRA:AVERAGE:0.5:12:720 \
			RRA:AVERAGE:0.5:288:365
