<?php
/*
 * This file is part of Aquarium Control Center (aquacc).
 *
 * Copyright (C) 2012-2013 Cluytens David <david.cluytens@gmail.com>
 *
 * Aquarium Control Center (aquacc) is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.
 *
 * Aquarium Control Center (aquacc) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aquarium Control Center (aquacc). If not, see <http://www.gnu.org/licenses/>.
 */

$DOSINGCONSTANT = 0.6; /* 5s = 3ml */
$MAXMOTOR = 3;

function driveMotor_form($id) {
	global $MAXMOTOR;

	echo "
		<br/>
		<center>
		<form action=\"aquacc.php?app=dosingunit&cmd=drivemotor\" method=\"post\">
		<table>
			<tr>
				<td><label>Motor</label></td>
				<td>
					<select name=\"motor_id\">";
	for ($i = 0; $i < $MAXMOTOR+1; $i++) {
		if ($i == $id) {
			echo "<option value=\"$i\" selected>$i</option>";
		} else {
			echo "<option value=\"$i\">$i</option>";
		}
	}
	echo "
					</select>
				</td>
				<td><label>Volume</label></td>
				<td><input type=\"text\" name=\"motor_volume\"></td>
			</tr>
			<tr>
				<td colspan=\"8\" align=\"right\"><input type=\"submit\" value=\"Submit\"></td>
			</tr>
		</table>
		</form>
		</center>
		";
}

function driveMotor($fp, $id, $seconds) {
	$milis = $seconds * 1000;
	if ($milis > 0 && $id > 0) {
		$out = "drive_motor $id $milis\r\n";
		fwrite($fp, $out);
	} else {
		echo "Error incorrect values";
	}
}

function setMotorInfo($fp, $id, $start, $for, $every) {
	$time = time();

	if ($id > 0 && $for > 0 && $every > 0 && $start > $time) {
		$out = "set_motor $id start $start for $for every $every\r\n";
		fwrite($fp, $out);
	} else {
		echo "Error incorrect values";
	}
}

function disableMotor($fp, $id) {
	if ($id > 0) {
		$out = "disable_motor $id\r\n";
		fwrite($fp, $out);
	} else {
		echo "Error incorrect values";
	}
}

function setMotorInfo_form($id) {
	global $MAXMOTOR;

	echo "
		<br/>
		<center>
		<form id=\"setMotorInfo\" action=\"aquacc.php?app=dosingunit&cmd=setmotorinfo\" method=\"post\">
		<table>
			<tr>
				<td><label>Motor</label></td>
				<td>
					<select name=\"motor_id\">";
	for ($i = 0; $i < $MAXMOTOR+1; $i++) {
		if ($i == $id) {
			echo "<option value=\"$i\" selected>$i</option>";
		} else {
			echo "<option value=\"$i\">$i</option>";
		}
	}
	$today = getdate();
	echo "
					</select>
				</td>
				<td><label>Start</label></td>
				<td><input type=\"text\" name=\"day\"   size=\"2\" maxlength=\"2\" value=\"$today[mday]\"   >-</td>
				<td><input type=\"text\" name=\"month\" size=\"2\" maxlength=\"2\" value=\"$today[mon]\"    >-</td>
				<td><input type=\"text\" name=\"year\"  size=\"4\" maxlength=\"4\" value=\"$today[year]\"   ></td>
				<td><input type=\"text\" name=\"hours\" size=\"2\" maxlength=\"2\" value=\"$today[hours]\"  >:</td>
				<td><input type=\"text\" name=\"min\"   size=\"2\" maxlength=\"2\" value=\"$today[minutes]\">:</td>
				<td><input type=\"text\" name=\"sec\"   size=\"2\" maxlength=\"2\" value=\"$today[seconds]\"></td>
				<td><label>For</label></td>
				<td>
					<select name=\"motor_for\">";
	for ($i = 0; $i < 11; $i++) {
		echo "<option value=\"$i\">$i</option>";
	}
	echo "			</select>
				</td>
				<td><label>Every</label></td>
				<td>
					<select name=\"motor_every\">
						<option value=\"3600\"  >1h</option>
						<option value=\"7200\"  >2h</option>
						<option value=\"10800\" >3h</option>
						<option value=\"14400\" >4h</option>
						<option value=\"21600\" >6h</option>
						<option value=\"28800\" >8h</option>
						<option value=\"43200\" >12h</option>
						<option value=\"86400\" >1d</option>
						<option value=\"172800\">2d</option>
						<option value=\"604800\">1w</option>
					</select>
				</td>
			</tr>
			<tr>
				<td colspan=\"8\" align=\"right\"><input type=\"submit\" onclick=\"set_motor_start(this.form)\" value=\"Submit\"></td>
			</tr>
		</table>
		</form>
		</center>
		";

}

function dosingVolumeToTime($v) {
	global $DOSINGCONSTANT;
	return round($v / $DOSINGCONSTANT);
}

function dosingTimeToVolume($t) {
	global $DOSINGCONSTANT;
	return round($t * $DOSINGCONSTANT);
}

function displayEvery($id) {
	switch($id) {
		case 1:
			$every = "second";
			break;

		case 60:
			$every = "min";
			break;

		case 3600:
			$every = "hour";
			break;

		case 7200:
			$every = "two hours";
			break;

		case 14400:
			$every = "four hours";
			break;

		case 28800:
			$every = "eight hours";
			break;

		case 43200:
			$every = "twelve hours";
			break;

		case 86400:
			$every = "day";
			break;

		case 172800:
			$every = "two days";
			break;

		case 604800:
			$every = "week";
			break;

		case 1209600:
			$every = "two weeks";
			break;

		default:
			$every=$id;
	}
	return $every;
}

function getMotorInfo($fp, $id) {
	$motor_id 	 = 0;
	$motor_start = 0;
	$motor_stop  = 0;
	$motor_for	 = 0;
	$motor_every = 0;


	$out = "get_motor $id\r\n";
	fwrite($fp, $out);
	while (!feof($fp)) {
		$line = fgets($fp, 128);
		$pattern = "/^Motor (\d+)/";
		if (preg_match($pattern, $line, $matches)) {
			$motor_id = $matches[1];
			continue;
		}

		$pattern = "/^Start: (\d+)/";
		if (preg_match($pattern, $line, $matches)) {
			$motor_start = $matches[1];
			continue;
		}

		$pattern = "/^Stop: (\d+)/";
		if (preg_match($pattern, $line, $matches)) {
			$motor_stop = $matches[1];
			continue;
		}

		$pattern = "/^Rotate for: (\d+)/";
		if (preg_match($pattern, $line, $matches)) {
			$motor_for = $matches[1];
			continue;
		}

		$pattern = "/^Every: (\d+)/";
		if (preg_match($pattern, $line, $matches)) {
			$motor_every = $matches[1];
			break;
			continue;
		}
	}
	date_default_timezone_set('Europe/Brussels');
	echo "<form action=\"aquacc.php?app=dosingunit&cmd=disablemotor\" method=\"post\">";
	echo "<table>";
	echo "<tr><td>Motor:</td><td>$motor_id</td>
			  <td>
			  	<input type=\"hidden\" name=\"motor_id\" value=\"$motor_id\"/>
				<input type=\"submit\" value=\"Disable\"/>
			  </td>
		  </tr>\n";
	echo "<tr><td>Start:</td><td colspan\"2\">" . date('d-m-Y H:i:s', $motor_start) . "</td></tr>\n";
	echo "<tr><td>Stop:</td><td colspan\"2\">" . date('d-m-Y H:i:s', $motor_stop) . "</td></tr>\n";
	echo "<tr><td>Duration:</td><td colspan\"2\">$motor_for seconds (". dosingTimeToVolume($motor_for)  . "ml )</td></tr>\n";
	echo "<tr><td>Frequency:</td><td colspan\"2\">every " . displayEvery($motor_every) . "</td></tr>\n";
	echo "</table>";
	echo "</form>";
}

function dosingOpenSocket() {
	$fp = fsockopen("localhost", 5000, $errno, $errstr, 30);
	if (!$fp) {
		echo "$errstr ($errno)<br />\n";
	}
	return $fp;
}

function dosingShowSchedule() {
	$fp = dosingOpenSocket();
	if (!$fp) {
		return;
	}
	echo "<center>";
	echo "<table width=\"100%\">
		<tr><td colspan=\"3\" align=\"center\"><b>Schedule dosing unit</b></td></tr>
		<tr><td>";
	getMotorInfo($fp, 1);
	echo "</td><td>";
	getMotorInfo($fp, 2);
	echo "</td><td>";
	getMotorInfo($fp, 3);
	echo "</td></tr></table>";
	echo "</center>";
	fclose($fp);
}
?>
