#!/bin/bash
RRDPATH=/home/pi/aquacc
GRAPHPATH=/var/www/rrd
RAWCOLOUR1="#00FF00"
RAWCOLOUR2="#BBFF00"
MINCOLOUR1="#0000FF"
AVGCOLOUR1="#FF9900"
MAXCOLOUR1="#FF0000"
MINCOLOUR2="#ffc0cb"
AVGCOLOUR2="#ee82ee"
MAXCOLOUR2="#a020f0"

mkdir -p $GRAPHPATH

#hour
rrdtool graph $GRAPHPATH/mhour.png --start -6h\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		GPRINT:intemp:AVERAGE:"Average temperature\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		GPRINT:outtemp:AVERAGE:"Average temperature\: %.2lf°C"

rrdtool graph $GRAPHPATH/big_mhour.png --start -6h -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		AREA:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR1:"Min" \
		GPRINT:intemp:MIN:"%.2lf°C\t" \
		HRULE:avg_intemp$AVGCOLOUR1:"Avg" \
		GPRINT:intemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_intemp$MAXCOLOUR1:"Max" \
		GPRINT:intemp:MAX:"%.2lf°C\t" \
		GPRINT:intemp:LAST:"Current\: %.2lf°C\n" \
		AREA:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR2:"Min" \
		GPRINT:outtemp:MIN:"%.2lf°C\t" \
		HRULE:avg_outtemp$AVGCOLOUR2:"Avg" \
		GPRINT:outtemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_outtemp$MAXCOLOUR2:"Max" \
		GPRINT:outtemp:MAX:"%.2lf°C\t" \
		GPRINT:outtemp:LAST:"Current\: %.2lf°C\n"

#day
rrdtool graph $GRAPHPATH/mday.png --start -1d\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		GPRINT:intemp:AVERAGE:"Average temperature\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		GPRINT:outtemp:AVERAGE:"Average temperature\: %.2lf°C"

rrdtool graph $GRAPHPATH/big_mday.png --start -1d  -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		AREA:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR1:"Min" \
		GPRINT:intemp:MIN:"%.2lf°C\t" \
		HRULE:avg_intemp$AVGCOLOUR1:"Avg" \
		GPRINT:intemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_intemp$MAXCOLOUR1:"Max" \
		GPRINT:intemp:MAX:"%.2lf°C\t" \
		GPRINT:intemp:LAST:"Current\: %.2lf°C\n" \
		AREA:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR2:"Min" \
		GPRINT:outtemp:MIN:"%.2lf°C\t" \
		HRULE:avg_outtemp$AVGCOLOUR2:"Avg" \
		GPRINT:outtemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_outtemp$MAXCOLOUR2:"Max" \
		GPRINT:outtemp:MAX:"%.2lf°C\t" \
		GPRINT:outtemp:LAST:"Current\: %.2lf°C\n"

#week
rrdtool graph $GRAPHPATH/mweek.png --start -1w \
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		GPRINT:intemp:AVERAGE:"Average temperature\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		GPRINT:outtemp:AVERAGE:"Average temperature\: %.2lf°C"

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
		HRULE:min_intemp$MINCOLOUR1:"Min" \
		GPRINT:intemp:MIN:"%.2lf°C\t" \
		HRULE:avg_intemp$AVGCOLOUR1:"Avg" \
		GPRINT:intemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_intemp$MAXCOLOUR1:"Max" \
		GPRINT:intemp:MAX:"%.2lf°C\t" \
		GPRINT:intemp:LAST:"Current\: %.2lf°C\n" \
		AREA:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR2:"Min" \
		GPRINT:outtemp:MIN:"%.2lf°C\t" \
		HRULE:avg_outtemp$AVGCOLOUR2:"Avg" \
		GPRINT:outtemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_outtemp$MAXCOLOUR2:"Max" \
		GPRINT:outtemp:MAX:"%.2lf°C\t" \
		GPRINT:outtemp:LAST:"Current\: %.2lf°C\n"

#month
rrdtool graph $GRAPHPATH/mmonth.png --start -1m\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		GPRINT:intemp:AVERAGE:"Average temperature\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		GPRINT:outtemp:AVERAGE:"Average temperature\: %.2lf°C"

		rrdtool graph $GRAPHPATH/big_mmonth.png --start -1m -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		AREA:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR1:"Min" \
		GPRINT:intemp:MIN:"%.2lf°C\t" \
		HRULE:avg_intemp$AVGCOLOUR1:"Avg" \
		GPRINT:intemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_intemp$MAXCOLOUR1:"Max" \
		GPRINT:intemp:MAX:"%.2lf°C\t" \
		GPRINT:intemp:LAST:"Current\: %.2lf°C\n" \
		AREA:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR2:"Min" \
		GPRINT:outtemp:MIN:"%.2lf°C\t" \
		HRULE:avg_outtemp$AVGCOLOUR2:"Avg" \
		GPRINT:outtemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_outtemp$MAXCOLOUR2:"Max" \
		GPRINT:outtemp:MAX:"%.2lf°C\t" \
		GPRINT:outtemp:LAST:"Current\: %.2lf°C\n"

#year
rrdtool graph $GRAPHPATH/myear.png --start -1y\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		LINE2:intemp$RAWCOLOUR1:"Inside temperature" \
		GPRINT:intemp:AVERAGE:"Average temperature\: %.2lf°C" \
		LINE2:outtemp$RAWCOLOUR2:"Outside temperature" \
		GPRINT:outtemp:AVERAGE:"Average temperature\: %.2lf°C"

rrdtool graph $GRAPHPATH/big_myear.png --start -1y -w 1024 -h 300\
		DEF:intemp=$RRDPATH/multirPItemp.rrd:in_temp:AVERAGE \
		DEF:outtemp=$RRDPATH/multirPItemp.rrd:out_temp:AVERAGE \
		VDEF:min_intemp=intemp,MINIMUM \
		VDEF:min_outtemp=outtemp,MINIMUM \
		VDEF:avg_intemp=intemp,AVERAGE \
		VDEF:avg_outtemp=outtemp,AVERAGE \
		VDEF:max_intemp=intemp,MAXIMUM \
		VDEF:max_outtemp=outtemp,MAXIMUM \
		AREA:intemp$RAWCOLOUR1:"Inside temperature\t" \
		HRULE:min_intemp$MINCOLOUR1:"Min" \
		GPRINT:intemp:MIN:"%.2lf°C\t" \
		HRULE:avg_intemp$AVGCOLOUR1:"Avg" \
		GPRINT:intemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_intemp$MAXCOLOUR1:"Max" \
		GPRINT:intemp:MAX:"%.2lf°C\t" \
		GPRINT:intemp:LAST:"Current\: %.2lf°C\n" \
		AREA:outtemp$RAWCOLOUR2:"Outside temperature\t" \
		HRULE:min_outtemp$MINCOLOUR2:"Min" \
		GPRINT:outtemp:MIN:"%.2lf°C\t" \
		HRULE:avg_outtemp$AVGCOLOUR2:"Avg" \
		GPRINT:outtemp:AVERAGE:"%.2lf°C\t" \
		HRULE:max_outtemp$MAXCOLOUR2:"Max" \
		GPRINT:outtemp:MAX:"%.2lf°C\t" \
		GPRINT:outtemp:LAST:"Current\: %.2lf°C\n"
