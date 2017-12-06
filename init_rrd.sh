#!/bin/bash
rrdtool create /www/multirPItemp.rrd  --step 60 \
			DS:in_temp:GAUGE:600:-30:50 \
			DS:out_temp:GAUGE:600:-30:50 \
			RRA:AVERAGE:0.5:1:60 \
			RRA:AVERAGE:0.5:1:1440 \
			RRA:AVERAGE:0.5:12:840 \
			RRA:AVERAGE:0.5:12:3600 \
			RRA:AVERAGE:0.5:288:1825
