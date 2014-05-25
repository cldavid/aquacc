#!/bin/bash
RRDPATH=/home/pi/aquacc
GRAPHPATH=/var/www/rrd
RAWCOLOUR1="#00FF00"
RAWCOLOUR2="#BBFF00"
MINCOLOUR="#0000FF"
AVGCOLOUR="#FF9900"
MAXCOLOUR="#FF0000"

mkdir -p $GRAPHPATH

#hour
rrdtool graph $GRAPHPATH/mhour.png --start -6h\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C" \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_mhour.png --start -6h -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR:"Min inside temperature\t" \
		HRULE:avg_intemp$AVGCOLOUR:"Average inside temperature\t" \
		HRULE:max_intemp$MAXCOLOUR:"Max inside temperature\t" \
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C\n" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR:"Min outside temperature\t" \
		HRULE:avg_outtemp$AVGCOLOUR:"Average outside temperature\t" \
		HRULE:max_outtemp$MAXCOLOUR:"Max outside temperature\t" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C\n" 

#day
rrdtool graph $GRAPHPATH/mday.png --start -1d\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_mday.png --start -1d  -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR:"Min inside temperature\t" \
		HRULE:avg_intemp$AVGCOLOUR:"Average inside temperature\t" \
		HRULE:max_intemp$MAXCOLOUR:"Max inside temperature\t" \
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C\n" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR:"Min outside temperature\t" \
		HRULE:avg_outtemp$AVGCOLOUR:"Average outside temperature\t" \
		HRULE:max_outtemp$MAXCOLOUR:"Max outside temperature\t" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C\n" 

#week
rrdtool graph $GRAPHPATH/mweek.png --start -1w \
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_mweek.png --start -1w -w 1024 -h 300 -E\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		AREA:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR:"" \
		HRULE:avg_intemp$AVGCOLOUR:"" \
		HRULE:max_intemp$MAXCOLOUR:"" \
		GPRINT:intemp:MIN:"Min\: %.2lf°C\t" \
		GPRINT:intemp:MAX:"Max\: %.2lf°C\t" \
		GPRINT:intemp:AVERAGE:"Avg\: %.2lf°C\t" \
		GPRINT:intemp:LAST:"Current\: %.2lf°C\n" \
		AREA:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		GPRINT:outtemp:MIN:"Min\: %.2lf°C\t" \
		GPRINT:outtemp:MAX:"Max\: %.2lf°C\t" \
		GPRINT:outtemp:AVERAGE:"Avg\: %.2lf°C\t" \
		GPRINT:outtemp:LAST:"Current\: %.2lf°C\n"

#month
rrdtool graph $GRAPHPATH/mmonth.png --start -1m\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

		rrdtool graph $GRAPHPATH/big_mmonth.png --start -1m -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR:"Min inside temperature\t" \
		HRULE:avg_intemp$AVGCOLOUR:"Average inside temperature\t" \
		HRULE:max_intemp$MAXCOLOUR:"Max inside temperature\t"\
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C\n" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR:"Min outside temperature\t" \
		HRULE:avg_outtemp$AVGCOLOUR:"Average outside temperature\t" \
		HRULE:max_outtemp$MAXCOLOUR:"Max outside temperature\t" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C\n"

#year
rrdtool graph $GRAPHPATH/myear.png --start -1y\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" 

rrdtool graph $GRAPHPATH/big_myear.png --start -1y -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR:"Min inside temperature\t" \
		HRULE:avg_intemp$AVGCOLOUR:"Average inside temperature\t"\
		HRULE:max_intemp$MAXCOLOUR:"Max inside temperature\t"\
		GPRINT:intemp:LAST:"Current temp.\: %.2lf°C\n" \
		GPRINT:intemp:AVERAGE:"Average temp.\: %.2lf°C\n" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR:"Min outside temperature\t" \
		HRULE:avg_outtemp$AVGCOLOUR:"Average outside temperature\t" \
		HRULE:max_outtemp$MAXCOLOUR:"Max outside temperature\t" \
		GPRINT:outtemp:LAST:"Current temp.\: %.2lf°C\n"
