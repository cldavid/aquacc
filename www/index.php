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
	require "dosingunit.php";
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
		<script src="aquacc.js" language="javascript" type="text/javascript"></script>
		<title>Aqua Control Center</title>
	</head>
<body bgcolor="#C6CED9">
<?php
	echo "<table border=\"1\" width=\"100%\"><tr><td>";
	dosingShowSchedule();	
	echo "</td></tr><tr><td>";
	echo "<table width=\"100%\" vallign=\"top\" allign=\"left\"><tr><td>";
	driveMotor_form(1);
	echo "</td><td>";
	driveMotor_form(2);
	echo "</td><td>";
	driveMotor_form(3);
	echo "</td></tr></table>";
	echo "</td></tr><tr><td align=\"center\">";
	setMotorInfo_form(0);
	echo "</td></tr></table>";

?>
</body>
</html>
