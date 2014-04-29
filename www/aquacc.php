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
?>
<html>
	<head>
		<meta http-equiv="Content-type" content="text/html;charset=UTF-8">
		<title>Aquarium Control Center</title>
	</head>
	<body bgcolor="#C6CED9">
<?php
require "dosingunit.php";
require "pdu.php";

$app = isset($_GET['app']) ? $_GET['app'] : "unknown";
$cmd = isset($_GET['cmd']) ? $_GET['cmd'] : "unknown";

$fp = dosingOpenSocket();
if (!$fp) {
	return;
}

switch($app) {
	case 'dosingunit':
		dsu_parseCmd($fp, $cmd);
		break;

	case 'pdu':
		pdu_parseCmd($cmd);
		break;

	default:
		break;
}

fclose($fp);
	goHome();

function goHome() {
	echo "<script>window.location = 'index.php'</script>";
}

function dsu_parseCmd($fp, $cmd) {
	switch($cmd) {
		case 'drivemotor':
			parseDriveMotor($fp);
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
?>
	</body>
</html>
