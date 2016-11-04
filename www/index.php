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
 date_default_timezone_set('Europe/Brussels');
	require "dosingunit.php";
	require "pdu.php";
	require "rrd.php";
?>
<html>
<head>
		<meta name="generator" content="007"/>
		<meta name="author" content="David Cluytens"/>
		<meta name="date" content="2013-04-24T12:33:10+0100"/>
		<meta name="copyright" content=""/>
		<meta name="keywords" content=""/>
		<meta name="description" content=""/>
		<meta name="ROBOTS" content="NOINDEX, NOFOLLOW"/>
		<meta http-equiv="Content-type" content="text/html;charset=UTF-8">
		<script src="js/jquery-2.1.0.min.js" type="text/javascript"></script>
		<script src="js/jquery-ui-1.10.4.custom.min.js"></script>
		<script src="js/aquacc.js" language="javascript" type="text/javascript"></script>
		<link rel="stylesheet" type="text/css" href="css/cupertino/jquery-ui-1.10.4.custom.css">
		<link rel="stylesheet" type="text/css" href="css/aquacc.css">
		<title>Aqua Control Center</title>
		<script>
			$(function() {
				$( "#tabs" ).tabs();
			});
		</script>
	</head>
	<body bgcolor="#C6CED9">
		<div id="tabs">
		<ul>
			<li><a href="#tabs-1">Dosing Unit</a></li>
			<li><a href="#tabs-2">PDU</a></li>
			<li><a href="#tabs-3">RRD</a></li>
		</ul>
		<div id="tabs-1">
		<table border="1" width="100%">
			<tr>
				<td>
				<?php
					dosingShowSchedule();	
				?>
				</td>
			</tr>
			<tr>
				<td>
					<table width="100%" vallign="top" allign="left">
						<tr>
							<td>
							<?php
								driveMotor_form(0);
							?>
							</td>
							<td>
							<?php
								driveMotor_form(1);
							?>
							</td>
							<td>
							<?php
								driveMotor_form(2);
							?>
							</td>
						</tr>
					</table>
				</td>
			</tr>
			<tr>
				<td align="center">
				<?php
					setMotorInfo_form(0);
				?>
				</td>
			</tr>
			</table>
			</div>

			<div id="tabs-2">
			<table border="1" width="100%">
			<tr>
				<td align="center">
				<?php
					printPDUinfo();
				?>
				</td>
			</tr>
			</table>
			</div>
			<div id="tabs-3">
			<table border="1" width="100%">
			<tr>
				<td align="center">
				<?php
					printRRD();
				?>
				</td>
			</tr>
			</table>
			</div>
		</div>
	</body>
</html>
