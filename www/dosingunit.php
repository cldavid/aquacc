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

function dsu_parseCmd($cmd) {
	$fp = dosingOpenSocket();
	if (!$fp) {
		return;
	}

	switch($cmd) {
    case 'show-dsu':
      printDSU();
      break;

		case 'drivemotor':
			parseDriveMotor($fp);
			break;

		case 'getMotorInfo':
			$id = isset($_POST['motor_id'])  	? $_POST['motor_id'] : 0;

			$cmdOut = getMotorInfo($fp, $id);
			$json_string = json_encode($cmdOut);
			echo($json_string);
			break;

		case 'setmotorinfo':
			parseSetMotorInfo($fp);
			break;

		case 'disablemotor':
			parseDisableMotor($fp);
			break;

		default:
			break;
	}
	fclose($fp);
}

function parseSetMotorInfo($fp) {
	$id 	= isset($_POST['motor_id']) 	? $_POST['motor_id'] 	: 0;
	$start  = isset($_POST['motor_start']) 	? $_POST['motor_start'] : 0;
	$for    = isset($_POST['motor_for']) 	? $_POST['motor_for'] 	: 0;
	$every  = isset($_POST['motor_every']) 	? $_POST['motor_every'] : 0;

	echo "Scheduling drive_motor_$id start $start for $for every $every<br/>";
	if ($fp && $id && $start && $for && $every) {
			setMotorInfo($fp, $id, $start, $for, $every);
	} else {
		echo "invalid input detected";
	}
}

function parseDriveMotor($fp) {
	$motorid = isset($_POST['motor_id']) 		? $_POST['motor_id'] : 0;
	$volume  = isset($_POST['motor_volume']) 	? $_POST['motor_volume'] : 0;
	$t = dosingVolumeToTime($volume);
	echo "Driving motor $motorid for $t seconds<br/>";
	if ($fp && $motorid && $t) {
		if (10 > $t) {
			driveMotor($fp, $motorid, $t);
		}
	} else {
		echo "invalid input detected";
	}
}

function parseDisableMotor($fp) {
	$motorid = isset($_POST['motor_id']) 		? $_POST['motor_id'] : 0;
	echo "Disabling motor $motorid<br/>";
	if ($fp && $motorid) {
		disableMotor($fp, $motorid);
	} else {
		echo "invalid input detected";
	}
}

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
	for ($i = 0; $i < $MAXMOTOR; $i++) {
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
	if ($milis > 0 && $id >= 0) {
		$out = "drive_motor $id $milis\r\n";
		fwrite($fp, $out);
	} else {
		echo "Error incorrect values";
	}
}

function setMotorInfo($fp, $id, $start, $for, $every) {
	$time = time();

	if ($id >= 0 && $for > 0 && $every > 0 && $start > $time) {
		$out = "set_motor $id start $start for $for every $every\r\n";
		fwrite($fp, $out);
	} else {
		echo "Error incorrect values";
	}
}

function disableMotor($fp, $id) {
	global $MAXMOTOR;

	if (($id >= 0) && ($id <= $MAXMOTOR)) {
		$out = "disable_motor $id\r\n";
		fwrite($fp, $out);
	} else {
		echo "Error incorrect values";
	}
}

function dosingVolumeToTime($v) {
	global $DOSINGCONSTANT;
	return round($v / $DOSINGCONSTANT);
}

function dosingTimeToVolume($t) {
	global $DOSINGCONSTANT;
	return round($t * $DOSINGCONSTANT);
}

function getMotorInfo($fp, $id) {
	$motor_id 	 = 0;
	$motor_start = 0;
	$motor_stop  = 0;
	$motor_for	 = 0;
	$motor_every = 0;
	$rc				   = 0;
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
	$arrayName = array('rcode' => $rc, 'data' => array('motorid' => $motor_id, 'starttime' => $motor_start, 'endtime' => $motor_stop, 'duration' => $motor_for, 'frequency' => $motor_every));
	return $arrayName;
}

function dosingOpenSocket() {
	$fp = fsockopen("localhost", 5000, $errno, $errstr, 30);
	if (!$fp) {
		echo "$errstr ($errno)<br />\n";
	}
	return $fp;
}

function printDSU() {
	?>
	<div id="dsu-tabs">
		<ul>
			<li id="dsu-tabs-pump1"><a href="#dsu-show-pump">Pump 1</a></li>
			<li id="dsu-tabs-pump2"><a href="#dsu-show-pump">Pump 2</a></li>
			<li id="dsu-tabs-pump3"><a href="#dsu-show-pump">Pump 3</a></li>
		</ul>
		<div id="dsu-show-pump" class="dsu-class">
			<p>
				<label for="dsu-show-motor">Motor:</label>
				<span id="dsu-show-motor"></span>
			</p>
			<p>
				<label for="dsu-show-starttime">Start:</label>
				<span id="dsu-show-starttime"></span>
			</p>
			<p>
				<label for="dsu-show-stoptime">Stop:</label>
				<span id="dsu-show-endtime"></span>
			</p>
			<p>
				<label for="dsu-show-duration">Duration:</label>
				<span id="dsu-show-duration"></span>
			</p>
			<p>
				<label for="dsu-show-frequency">Frequency:</label>
				<span id="dsu-show-frequency"></span>
			</p>
			<button id="dsu-button-edit" class="ui-button ui-widget ui-corner-all">Edit</button>
		</div>
	</div>
	<div id="dsuModal" class="dsu-class modal">
		<div class="modal-content">
			<div class="modal-header">
				<span class="close">&times;</span>
				<h2>Pump Configuration</h2>
			</div>
			<div class="modal-body">
				<div id="maatbeker-img"></div>
				<div id="dsu-form">
					<p>
						<label for="dsu-edit-motor">Motor:</label>
						<input id="dsu-edit-motor" name="dsu-edit-motor" class="ui-widget ui-corner-all ui-widget-content">
					</p>
					<p>
						<label for="dsu-edit-startdate">Date:</label>
						<input id="dsu-edit-startdate" name="dsu-edit-startdate" class="ui-widget ui-corner-all ui-widget-content">
					</p>
					<p>
						<label for="dsu-edit-starttime">Time:</label>
	  				<input id="dsu-edit-starttime" name="dsu-edit-starttime" class="ui-widget ui-corner-all ui-widget-content">
					</p>
					<p>
						<label for="dsu-edit-duration">Duration:</label>
						<input name="dsu-edit-duration" id="dsu-edit-duration" value="0">
					</p>
					<p>
						<label for="dsu-edit-frequency">Frequency:</label>
						<select id="dsu-edit-frequency" name="dsu-edit-frequency">
							<option value="3600"  >1h</option>
							<option value="7200"  >2h</option>
							<option value="10800" >3h</option>
							<option value="14400" >4h</option>
							<option value="21600" >6h</option>
							<option value="28800" >8h</option>
							<option value="43200" >12h</option>
							<option value="86400" selected>1d</option>
							<option value="172800">2d</option>
							<option value="604800">1w</option>
						</select>
					</p>
					<br/>
					<div class="modal-buttons">
						<button id="dsu-button-disable" class="ui-button ui-widget ui-corner-all">Disable</button>
						<button id="dsu-button-ok" class="ui-button ui-widget ui-corner-all">OK</button>
						<button id="dsu-button-cancel" class="ui-button ui-widget ui-corner-all">Cancel</button>
					</div>
				</div>
			</div>
			<div class="modal-footer"></div>
		</div>
	</div>
	<?php
}
?>
