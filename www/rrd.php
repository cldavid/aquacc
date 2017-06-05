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

function rrd_parseCmd($cmd) {
	switch($cmd) {
		case 'show-rrd':
			printRRD();
			break;

		default:
			break;
	}
}

function	printRRD() {
?>
<div id="rrd-tabs">
	<ul>
		<li id="rrd-tabs-lhour"><a href="#temperature-svg">Hour</a></li>
		<li id="rrd-tabs-lday"><a href="#temperature-svg">Day</a></li>
		<li id="rrd-tabs-lweek"><a href="#temperature-svg">Week</a></li>
		<li id="rrd-tabs-lmonth"><a href="#temperature-svg">Month</a></li>
		<li id="rrd-tabs-lyear"><a href="#temperature-svg">Year</a></li>
	</ul>
	<div id="temperature-svg">
		<svg id="d3js-rrd-graph"/>
	</div>
</div>

<?php
	}
?>
