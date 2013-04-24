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
		<meta http-equiv="Content-type" content="text/html;charset=UTF-8">
		<title>Aqua Control Center</title>
		            <script LANGUAGE="JavaScript" TYPE="text/javascript">
					function humanToTime(theForm) {
						var humDate = new Date(theForm.year.value,
									(stripLeadingZeroes(theForm.month.value)-1),
									stripLeadingZeroes(theForm.day.value),
									stripLeadingZeroes(theForm.hours.value),
									stripLeadingZeroes(theForm.min.value),
									stripLeadingZeroes(theForm.sec.value));
						var epoch = humDate.getTime() / 1000.0;
						return epoch;
					}

					function stripLeadingZeroes(input) {
						if((input.length > 1) && (input.substr(0,1) == "0"))
							return input.substr(1);
						else
							return input;
					}
		
					function set_motor_start(theForm) {
						var epoch = humanToTime(theForm);
						var hiddenField = document.createElement("input");
						hiddenField.setAttribute("type", "hidden");
						hiddenField.setAttribute("name", "motor_start");
						hiddenField.setAttribute("value", epoch);

						theForm.appendChild(hiddenField);
					}
</script>
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
