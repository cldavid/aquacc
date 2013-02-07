<html>
	<head>
		<title>Aquarium Control Center</title>
	</head>
	<body bgcolor="#C6CED9">
<?php
require('dosingunit.php');
$app = isset($_GET['app']) ? $_GET['app'] : "unknown";
$cmd = isset($_GET['cmd']) ? $_GET['cmd'] : "unknown";

$fp = dosingOpenSocket();
if (!$fp) {
	return;
}

switch($app) {
	case 'dosingunit':
		parseCmd($fp, $cmd);
		break;
	default:
		break;
}

fclose($fp);
	goHome();

function goHome() {
	echo "<script>window.location = 'index.php'</script>";
}

function parseCmd($fp, $cmd) {
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
