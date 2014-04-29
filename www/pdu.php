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

$SISPMCTL = "/usr/local/bin/sispmctl";

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

function printPDUinfo() {
	$status = getPDU_outlet_status();
	?>
		<ul id="sis-pm">
			<li id="power-all-switch"><a title="Switch all" href="#"></a></li>
			<li id="power-4-switch"><a title="Switch fourth" onClick="switch_outlet(4, 'off');"></a></li>
			<li id="power-2-switch"><a title="Switch second" onClick="switch_outlet(2, 'off');"></a></li>
			<li id="power-1-switch"><a title="Switch first"  onClick="switch_outlet(1, 'on');"></a></li>
	<?php
		if ($status[3]) {
			echo "<li id=\"power-4-on\"></li>\n";
		}
		if ($status[2]) {
			echo "<li id=\"power-3-switch\"><a title=\"Switch third\"  onClick=\"switch_outlet(3, 'off')\"></a></li>\n";
			echo "<li id=\"power-3-on\"></li>\n";
		} else {
			echo "<li id=\"power-3-switch\"><a title=\"Switch third\"  onClick=\"switch_outlet(3, 'on')\"></a></li>\n";
		}
		if ($status[1]) {
			echo "<li id=\"power-2-on\"></li>\n";
		}
		if ($status[0]) {
			echo "<li id=\"power-1-on\"></li>\n";
		}
		?>
		</ul>
	<?php
}
?>
