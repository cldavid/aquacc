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

$SISPMCTL = "/usr/bin/sispmctl";

function pdu_parseCmd($cmd) {
	switch($cmd) {
		case 'outlet_on':
			$outlet  = isset($_POST['outlet_no'])  ? $_POST['outlet_no'] : 0;
			$result  = setPDU_outlet_on($outlet);
			echo ("Enabling output $outlet result $result");
			break;

		case 'outlet_off':
			$outlet  = isset($_POST['outlet_no'])  ? $_POST['outlet_no'] : 0;
			$result  = setPDU_outlet_off($outlet);
			echo ("Disabling output $outlet result $result");
			break;

		case 'status':
			printPDUstatus();
			break;

		default:
			break;
	}
}


function setPDU_outlet_off($outlet) {
	global $SISPMCTL;

	$output = exec($SISPMCTL . " -f $outlet", $arr);
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

function setPDU_outlet_on($outlet) {
	global $SISPMCTL;

	$output = exec($SISPMCTL . " -o $outlet", $arr);
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

function getPDU_outlet_status() {
	global $SISPMCTL;

	$output = exec($SISPMCTL . " -g all", $arr);
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

function getPDU_outlet_plannification($outlet) {
	global $SISPMCTL;
	$plan = array();

	if (($outlet < 1) || ($outlet > 4)) {
		return null;
	}
	$index = 0;
	$output = exec($SISPMCTL . " -a$outlet", $arr);
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
	$status = getPDU_outlet_status();
	for ($i = 0; $i < 4; $i++) {
		$outlet = $i + 1;
		if ($status[$i]) {
			echo "<li id=\"power-$outlet-switch\"><a title=\"Switch $outlet\" onClick=\"switch_outlet($outlet, 'off')\"></a></li>\n";
			echo "<li id=\"power-$outlet-on\"></li>\n";
		} else {
			echo "<li id=\"power-$outlet-switch\"><a title=\"Switch $outlet\" onClick=\"switch_outlet($outlet, 'on')\"></a></li>\n";
		}
	}
}

function printPDUstatus2() {
	$status = getPDU_outlet_status();
        echo "<br/>\n";
        echo "<table border=\"1\" width=\"90%\">\n";
        echo "<tr><th colspan=\"4\">PDU-x</th></tr>\n";
	for ($i = 0; $i < 4; $i++) {
        	echo "<tr>\n";	
		$outlet = $i + 1;
		if ($status[$i]) {
			echo "<td><a title=\"Switch $outlet\" onClick=\"switch_outlet($outlet, 'off')\">";
			echo "<img src=\"images/on.png\"></a></td>\n";
		} else {
			echo "<td><a title=\"Switch $outlet\" onClick=\"switch_outlet($outlet, 'on')\">";
			echo "<img src=\"images/off.png\"></a></td>\n";
		}
        	echo "</tr>\n";
	}
        echo "</table>\n";
}

function printPDU_outlet_plannification() {
	echo "<br/>\n";
	echo "<table border=\"1\" width=\"90%\">\n";
	echo "<tr><th colspan=\"4\">Planification</th></tr>\n";
	echo "<tr>\n";
	for ($i = 1; $i < 5; $i++) {
		echo "<td valign=\"top\" align=\"center\">\n";
		echo "<table  width=\"100%\">\n";
		$plan = getPDU_outlet_plannification($i);
		if ($plan) {
			echo "<tr><th colspan=\"4\">Outlet $i</th></tr>\n";
			foreach($plan as $key => $value) {
				$date 	= $plan[$key]['date'];
				$time 	= $plan[$key]['time'];
				$status = $plan[$key]['status'];
				$status = $status ? "on" : "off";
				echo "<tr><td><b>Date</b></td><td>$date $time</td><td><b>Status</b></td><td>$status</td></tr>\n";
			}
		} else {
			echo "<tr><th>Outlet $i</th></tr>\n";
			echo "<tr><th>Disabled</th></tr>\n";
		}
		echo "</table>\n";
		echo "</td>\n";
	}
	echo "</tr>\n";
	echo "</table>\n";
}

function printPDUinfo() {
	?>
		<ul id="sis-pm">
			<div id="div-sis-pm">
				<?php
					//printPDUstatus();
					printPDUstatus2();
				?>
			</div>
		</ul>
	<?php
	printPDU_outlet_plannification();
}
?>
