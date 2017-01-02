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
 require "pdu.php";
 require "rrd.php";

 $app 					= isset($_GET['app']) ? $_GET['app'] : "unknown";
 $cmd 					= isset($_GET['cmd']) ? $_GET['cmd'] : "unknown";
 $html_header 	= isset($_POST['html_header']) ? $_POST['html_header'] : 1;
 if ($html_header) {
?>
<html>
	<head>
		<meta http-equiv="Content-type" content="text/html;charset=UTF-8">
		<title>Aquarium Control Center</title>
	</head>
	<body>
<?php
}

switch($app) {
  case 'dsu':
    dsu_parseCmd($cmd);
    break;

	case 'dosingunit':
		dsu_parseCmd($cmd);
		goHome();
		break;

	case 'pdu':
		pdu_parseCmd($cmd);
		break;

	case 'rrd':
		rrd_parseCmd($cmd);
		break;

	default:
		break;
}

if ($html_header) {
?>
	</body>
</html>
<?php
}

function goHome() {
	echo "<script>window.location = 'index.php'</script>";
}
