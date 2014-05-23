#!/bin/bash
RRDPATH=/home/pi/aquacc
GRAPHPATH=/var/www/rrd
RAWCOLOUR1="#FF0000"
RAWCOLOUR2="#00FF00"
TRENDCOLOUR="#0000FF"

mkdir -p $GRAPHPATH

#hour
rrdtool graph $GRAPHPATH/mhour.png --start -6h\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		CDEF:intrend=intemp,1800,TREND \
		CDEF:outtrend=outtemp,1800,TREND \
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C" \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE1:intrend$TRENDCOLOUR:"30 min average" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		LINE1:outtrend$TRENDCOLOUR:"30 min average" 

rrdtool graph $GRAPHPATH/big_mhour.png --start -6h -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		CDEF:intrend=intemp,1800,TREND \
		CDEF:outtrend=outtemp,1800,TREND \
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C" \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE1:intrend$TRENDCOLOUR:"30 min average" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		LINE1:outtrend$TRENDCOLOUR:"30 min average" 

#day
rrdtool graph $GRAPHPATH/mday.png --start -1d\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		CDEF:intrend=intemp,1800,TREND \
		CDEF:outtrend=outtemp,1800,TREND \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE1:intrend$TRENDCOLOUR:"1h min average" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		LINE1:outtrend$TRENDCOLOUR:"1h min average"

rrdtool graph $GRAPHPATH/big_mday.png --start -1d  -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		CDEF:intrend=intemp,1800,TREND \
		CDEF:outtrend=outtemp,1800,TREND \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE1:intrend$TRENDCOLOUR:"1h min average" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		LINE1:outtrend$TRENDCOLOUR:"1h min average"
#week
rrdtool graph $GRAPHPATH/mweek.png --start -1w \
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_mweek.png --start -1w -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

#month
rrdtool graph $GRAPHPATH/mmonth.png --start -1m\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_mmonth.png --start -1m -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

#year
rrdtool graph $GRAPHPATH/myear.png --start -1y\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_myear.png --start -1y -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 
