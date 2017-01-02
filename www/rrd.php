<?php

function rrd_parseCmd($cmd) {
	switch($cmd) {
		case 'show-rrd':
			printRRD();
			break;

		default:
			break;
	}
}

function	printRRD() {
	?>
<table class="rrd-table-class">
	<tr>
		<td align="center">
			<b>Hour</b>
		</td>
	</tr>
	<tr>
		<td>
			<a href="rrd/big_mhour.png"><img src="rrd/mhour.png"/></a>
		</td>
	</tr>
	<tr>
		<td align="center">
			<b>Day</b>
		</td>
	</tr>
	<tr>
		<td>
			<a href="rrd/big_mday.png"><img src="rrd/mday.png"/></a>
		</td>
	</tr>
	<tr>
		<td align="center">
			<b>Week</b>
		</td>
	</tr>
	<tr>
		<td>
			<a href="rrd/big_mweek.png"><img src="rrd/mweek.png"/></a>
		</td>
	</tr>
	<tr>
		<td align="center">
			<b>Month</b>
		</td>
	</tr>
	<tr>
		<td>
			<a href="rrd/big_mmonth.png"><img src="rrd/mmonth.png"/></a>
		</td>
	</tr>
	<tr>
		<td align="center">
			<b>Year</b>
		</td>
	</tr>
	<tr>
		<td>
			<a href="rrd/big_myear.png"><img src="rrd/myear.png"/></a>
		</td>
	</tr>
</table>
<?php
	}
?>
