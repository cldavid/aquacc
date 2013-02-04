<?php 
	require "dosingunit.php";
?>
<html>
	<head>
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
