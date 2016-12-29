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
$pdu_config_file = "/www/aquacc/config/pdu.db";
$SISPMCTL = "/opt/apps/sispmctl/bin/sispmctl";

function pdu_parseCmd($cmd) {
	switch($cmd) {
		case 'outlet_on':
			$serial  = isset($_POST['serial'])  	? $_POST['serial'] : 0;
			$outlet  = isset($_POST['outlet_no'])	? $_POST['outlet_no'] : 0;
			if ($outlet >=1 && $outlet <= 4) {
				$result  = setPDU_outlet_on($serial, $outlet);
			}
			break;

		case 'outlet_off':
			$serial  = isset($_POST['serial'])  	? $_POST['serial'] : 0;
			$outlet  = isset($_POST['outlet_no'])	? $_POST['outlet_no'] : 0;
			if ($outlet >=1 && $outlet <= 4) {
				$result  = setPDU_outlet_off($serial, $outlet);
			}
			break;

		case 'disable_plannification':
			$serial  = isset($_POST['serial'])  	? $_POST['serial'] : 0;
			$outlet  = isset($_POST['outlet_no'])	? $_POST['outlet_no'] : 0;

			if ($outlet >=1 && $outlet <= 4) {
				$result  = disablePDU_plannifcation($serial, $outlet);
			}
			break;

		case 'get_plannification':
			$serial  = isset($_POST['serial'])  	? $_POST['serial'] : 0;
			$outlet  = isset($_POST['outlet_no'])	? $_POST['outlet_no'] : 0;

			if ($outlet >=1 && $outlet <= 4) {
				$result  = getPDU_plannifcation($serial, $outlet);
			}
			break;

		case 'set_plannification':
			$serial  			= isset($_POST['serial'])  		? $_POST['serial'] : 0;
			$outlet  			= isset($_POST['outlet_no'])	? $_POST['outlet_no'] : 0;
			$scheduler  	= isset($_POST['scheduler'])	? $_POST['scheduler'] : null;
			$loopMinutes  = isset($_POST['loop_min'])		? $_POST['loop_min'] : 1440;
			$plan		 			= json_decode($scheduler, true);

			if ($plan) {
				$result  = setPDU_plannifcation($serial, $outlet, $plan, $loopMinutes);
			}
			break;

		case 'getStatus':
			$serial = isset($_POST['serial'])  		? $_POST['serial'] : 0;
			$status = getPDU_outlet_status($serial);
			$data 	= array("serial" => $serial, "outlet" => array());
			for ($outlet = 1; $outlet < 5; $outlet++)
			{
				$plan = getPDU_outlet_plannification($serial, $outlet);
				array_push($data["outlet"], array("status" => $status[$outlet-1], "scheduler" => $plan));
			}
			$json_string = json_encode($data);
			echo($json_string);
			break;

		case 'scan':
			scanPDU();
			break;

		case 'save':
			writePDUconfig($pdu_config);
			break;

		default:
			break;
	}
}


function setPDU_outlet_off($serial, $outlet) {
	global $SISPMCTL;

	$data = 1;
	$output = exec($SISPMCTL . " -D $serial -f $outlet", $arr, $rc);
	foreach ($arr as &$value) {
		$pattern = "/^Switched outlet\s(\d+)\soff/";
		if (preg_match($pattern, $value, $matches)) {
			$port = $matches[1];
			if($outlet == $port) {
				$data = 0;
				break;
			}
		}
	}
	unset($value);

	$cmdOut = array("command" => $cmdString, "output" => $arr, "rcode" => $rc, "data" => array("serial" => $serial, "outlet" => $outlet, "status" => $data));
	$json_string = json_encode($cmdOut);
	echo($json_string);
	return $cmdOut;
}

function setPDU_outlet_on($serial, $outlet) {
	global $SISPMCTL;

	$data = 0;
	$output = exec($SISPMCTL . " -D $serial -o $outlet", $arr, $rc);
	foreach ($arr as &$value) {
		$pattern = "/^Switched outlet\s(\d+)\son/";
		if (preg_match($pattern, $value, $matches)) {
			$port = $matches[1];
			if($outlet == $port) {
				$data = 1;
			}
		}
	}
	unset($value);

	$cmdOut = array("command" => $cmdString, "output" => $arr, "rcode" => $rc, "data" => array("serial" => $serial, "outlet" => $outlet, "status" => $data));
	$json_string = json_encode($cmdOut);
	echo($json_string);
	return $cmdOut;
}

function getPDU_outlet_status($serial) {
	global $SISPMCTL;
	$output = exec($SISPMCTL . " -D " . $serial . " -g all", $arr);
	foreach ($arr as &$value) {
		$pattern = "/^Status of outlet (\d+):\s(\w+)/";
		if (preg_match($pattern, $value, $matches)) {
			$port   = $matches[1] - 1;
			$status[$port]  = $matches[2] == "on" ? 1 : 0;
		}
	}
	unset($value);
	return $status;
}

function scanPDU() {
	global $SISPMCTL;
	$status = array();
	$output = exec($SISPMCTL . " -s", $arr, $rc);
	foreach ($arr as &$value) {
		$pattern = "/^serial\snumber:\s+([\w:]+)/";
		if (preg_match($pattern, $value, $matches)) {
			$serial = $matches[1];
			array_push($status, $serial);
		}
	}
	unset($value);

	$cmdOut = array("command" => $cmdString, "output" => $arr, "rcode" => $rc, "data" => $status);
	$json_string = json_encode($cmdOut);
	echo($json_string);
	return $json_string;
}

function setPDU_plannifcation($serial, $outlet, $plan, $loopMinutes) {
	global $SISPMCTL;

	$cmdString = $SISPMCTL . " -D ". $serial . " -A" . $outlet . " ";
	for ($i = 0; $i < count($plan); $i++) {
		$dateString = $plan[$i]['date'] . ' ' . $plan[$i]['time'];
		$status 		= $plan[$i]['status'] ? "on" : "off";
		$cmdString .= "--Aat \"" . $dateString . "\" --Ado " . $status . " ";
	}
	$cmdString .= " --Aloop " . $loopMinutes;
	exec($cmdString, $arr, $rc);

	$cmdOut = array("command" => $cmdString, "output" => $arr, "rcode" => $rc, "data" => array("serial" => $serial, "outlet" => $outlet, "scheduler" => $plan));
	$json_string = json_encode($cmdOut);
	echo($json_string);
	return $cmdOut;
}

function disablePDU_plannifcation($serial, $outlet) {
	global $SISPMCTL;

	$cmdString = $SISPMCTL . " -D " . $serial . " -A" . $outlet;
	exec($cmdString, $arr, $rc);

	$cmdOut = array("command" => $cmdString, "output" => $arr, "rcode" => $rc);
	$json_string = json_encode($cmdOut);
	echo($json_string);
	return $cmdOut;
}

function getPDU_plannifcation($serial, $outlet) {
	$plan = getPDU_outlet_plannification($serial, $outlet);
	$dev_plan = array("serial" => $serial, "outlet" => $outlet, "scheduler" => $plan);
	$json_string = json_encode($dev_plan);
	echo($json_string);
}

function getPDU_outlet_plannification($serial, $outlet) {
	global $SISPMCTL;
	$plan = array();

	if (($outlet < 1) || ($outlet > 4)) {
		return null;
	}
	$index = 0;
	$output = exec($SISPMCTL . " -D $serial -a$outlet", $arr);
	foreach ($arr as &$value) {
		$pattern = "/\s+On\s+([\w-]+)\s+([\w:]+)\s+switch\s+(\w+)/";
		if (preg_match($pattern, $value, $matches)) {
			$date   = $matches[1];
			$time   = $matches[2];
			$status = $matches[3] == "on" ? 1 : 0;
			$plan[$index] = array('date' 		=> $date,
								  'time' 		=> $time,
								  'status' 	=> $status);
			$index++;
		}
	}
	unset($value);
	return $plan;
}

function readPDUconfig(){
	global $pdu_config_file;
	return unserialize(file_get_contents($pdu_config_file));
}

function writePDUconfig($config) {
	global $pdu_config_file;
	$pdu_config = array(
			"01:01:54:47:ba" => array(
				"name" => "PDU-1",
				"port-1" => array(
					"name" => "Voorste lampen",
					"type" => "lamp",
					"icon" => "unknown"),
				"port-2" => array(
					"name" => "Achterste lampen",
					"type" => "lamp",
					"icon" => "unknown"),
				"port-3" => array(
					"name" => "Verwarmer",
					"type" => "lamp",
					"icon" => "unknown"),
				"port-4" => array(
					"name" => "Eheim Filter",
					"type" => "filter",
					"icon" => "unknown"),
				),
			"01:01:53:c8:20" => array(
				"name" => "PDU-2",
				"port-1" => array(
					"name" => "Voeding 1",
					"type" => "lamp",
					"icon" => "unknown"),
				"port-2" => array(
					"name" => "Voeding 2",
					"type" => "lamp",
					"icon" => "unknown"),
				"port-3" => array(
					"name" => "Voeding 3",
					"type" => "lamp",
					"icon" => "unknown"),
				"port-4" => array(
					"name" => "Voeding 4",
					"type" => "lamp",
					"icon" => "unknown"),
				),
			);
	$config = $pdu_config;
	file_put_contents($pdu_config_file,serialize($config));
}

?>
