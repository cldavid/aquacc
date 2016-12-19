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
$SISPMCTL = "/usr/bin/sispmctl";

function pdu_parseCmd($cmd) {
	switch($cmd) {
		case 'outlet_on':
			$serial  = isset($_POST['serial'])  	? $_POST['serial'] : 0;
			$outlet  = isset($_POST['outlet_no'])  	? $_POST['outlet_no'] : 0;
			$result  = setPDU_outlet_on($serial, $outlet);
			echo ("Enabling serial $serial output $outlet result $result");
			break;

		case 'outlet_off':
			$serial  = isset($_POST['serial'])  	? $_POST['serial'] : 0;
			$outlet  = isset($_POST['outlet_no'])  	? $_POST['outlet_no'] : 0;
			$result  = setPDU_outlet_off($serial, $outlet);
			echo ("Disabling serial $serial output $outlet result $result");
			break;

		case 'status':
			printPDUstatus();
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

	$output = exec($SISPMCTL . " -D $serial -f $outlet", $arr);
	foreach ($arr as &$value) {
		$pattern = "/^Switched outlet\s(\d+)\soff/";
		if (preg_match($pattern, $value, $matches)) {
			$port = $matches[1];
			if($outlet == $port) {
				return 1;
			}
		}   
	}   
	unset($value);
	return 0;
}

function setPDU_outlet_on($serial, $outlet) {
	global $SISPMCTL;

	$output = exec($SISPMCTL . " -D $serial -o $outlet", $arr);
	foreach ($arr as &$value) {
		$pattern = "/^Switched outlet\s(\d+)\son/";
		if (preg_match($pattern, $value, $matches)) {
			$port = $matches[1];
			if($outlet == $port) {
				return 1;
			}
		}   
	}   
	unset($value);
	return 0;
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
	$output = exec($SISPMCTL . " -s", $arr);
	foreach ($arr as &$value) {
		$pattern = "/^serial\snumber:\s+([\w:]+)/";
		if (preg_match($pattern, $value, $matches)) {
			$serial = $matches[1];
			array_push($status, $serial);
		}   
	}   
	unset($value);
	return $status;
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

function printPDUstatus() {
	$pdu_config = readPDUconfig();
	$ser_arr = scanPDU();
	for ($dev = 0; $dev < count($ser_arr); $dev++) {	
		$serial = $ser_arr[$dev];
		$status = getPDU_outlet_status($serial);
		echo "<br/>\n";
		echo "<table border=\"1\" width=\"90%\">\n";
		echo "<tr><th colspan=\"5\">PDU-$serial</th></tr>\n";
		for ($i = 0; $i < 4; $i++) {
			echo "<tr>\n";	
			$outlet = $i + 1;
			$port_name = $pdu_config[$serial]["port-$outlet"]['name'];
			echo "<td width=\"20\"><center><b>$outlet</b></center></td>";
			echo "<td><center><b>" . $port_name . "</b></center></td>";
			echo "<td>";
			printPDU_outlet_plannification($serial, $outlet);
			echo "</td>";

			if ($status[$i]) {
				echo "<td width=\"75\" height=\"75\"><a title=\"Switch $outlet\" onClick=\"switch_outlet('$serial', '$outlet', 'off')\">";
				echo "<img src=\"/aquacc/images/on.png\" width=\"75\" height=\"75\"/></a></td>\n";
			} else {
				echo "<td width=\"75\" height=\"75\"><a title=\"Switch $outlet\" onClick=\"switch_outlet('$serial', '$outlet', 'on')\">";
				echo "<img src=\"/aquacc/images/off.png\" width=\"75\" height=\"75\"/></a></td>\n";
			}
			echo "</tr>\n";
		}
		echo "</table>\n";
	}
}

function printPDU_outlet_plannification($serial, $outlet) {
	echo "<table width=\"100%\">\n";
	$plan = getPDU_outlet_plannification($serial, $outlet);
	if ($plan) {
		foreach($plan as $key => $value) {
			$date 	= $plan[$key]['date'];
			$time 	= $plan[$key]['time'];
			$status = $plan[$key]['status'];
			$status = $status ? "on" : "off";
			echo "<tr><td><b>Date</b></td><td>$date $time</td><td><b>Status</b></td><td>$status</td></tr>\n";
		}
	} else {
		echo "<tr><th>Scheduler disabled</th></tr>\n";
	}
	echo "</table>\n";
}

function printPDU_outlet_plannifications($serial) {
	echo "<br/>\n";
	echo "<table border=\"1\" width=\"90%\">\n";
	echo "<tr><th colspan=\"4\">Planification</th></tr>\n";
	echo "<tr>\n";
	for ($i = 1; $i < 5; $i++) {
		echo "<td valign=\"top\" align=\"center\">\n";
		printPDU_outlet_plannification($serial, $i);
		echo "</td>\n";
	}
	echo "</tr>\n";
	echo "</table>\n";
}

function printPDUinfo2() {
	echo "<div id=\"div-sis-pm\">";
	printPDUstatus();
	echo "</div>";
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
