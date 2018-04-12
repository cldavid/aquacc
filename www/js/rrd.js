var rrdReloadTimer = null;

function getMinValue(data) {
	var max;
	data.forEach(function(e) {
		if (max == undefined) {
			max = e.value;
		} else if (e.value < max) {
			max = e.value;
		}
	});
	return max;
}

function getMaxValue(data) {
	var max;
	data.forEach(function(e) {
		if (max == undefined) {
			max = e.value;
		} else if (e.value > max) {
			max = e.value;
		}
	});
	return max;
}

function setValue(src, dst, value) {
	src.forEach(function(e) {
		dst.push({time: e.time, value: value});
	});
	return;
}

function printData(data) {
	data.forEach(function(e) {
		console.log("t: " + e.time + " value: " + e.value);
	});
	return;
}

function printSeries(series) {
	series.forEach(function(s) {
		console.log("name: " + s.name);
		printData(s.data);
	});
	return;
}


function parseRRD_XML(xml) {
	var series = [];
	//define series
	$(xml).find("entry").each(function() {
		var n = $(this).text();
		series.push({name: n, data: []});
	});

	//populate with data
	$(xml).find("row").each(function() {
		var t = parseInt($(this).find("t").text()) * 1000;
		t = new Date(t);
		$(this).find("v").each(function(index) {
			var v = parseFloat($(this).text());
			v = v || null;
			if (v != null) {
				v = v.toFixed(2);
				series[index].data.push({time: t, value: +v});
			};
		});
	});

	series.forEach(function(s, i) {
			var n_min 	= s.name + "_min";
			var n_max 	= s.name + "_max";
			var min 	= getMinValue(s.data);
			var max 	= getMaxValue(s.data);
			var minData	= Array();
			var maxData	= Array();
			setValue(s.data, minData, min);
			setValue(s.data, maxData, max);
			//series.splice((i * 3) + 1, 0, {name: n_min, data: minData}, {name: n_max, data: maxData});
			series.push({name: n_min, data: minData});
			series.push({name: n_max, data: maxData});

	});
	return series;
}

function rrdGraph(event) {
	var interval = 60000;   //number of mili seconds between each call
	var xmlFile  = event.data.xmlFile;
	var formater = event.data.timeFormat;
	var ticks		 = event.data.ticks;

	clearTimeout(rrdReloadTimer);
	$.ajax({
		type: "GET",
		url: "/aquacc/xml/" + xmlFile + ".xml",
		dataType: "xml",
		success: function(xml) {
			var series = parseRRD_XML(xml);

			$('#d3js-rrd-graph').empty();
			drawMe(series, formater, ticks);
		},
		error: function(request, msg) {
			alert(msg);
		}
	})
	rrdReloadTimer = setTimeout(rrdGraph, interval, event);
}

function load_rrd_page() {
	clearTimeout(pduReloadTimer);
	clearTimeout(rrdReloadTimer);
	$('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=rrd&cmd=show-rrd",
		success: function(html) {
			$('#rrd-page').html(html);
			$("#rrd-tabs").tabs();
			$("#rrd-tabs-lhour").click({xmlFile: 'mhour', timeFormat: "%H:%M", ticks: 10}, rrdGraph);
			$("#rrd-tabs-lday").click({xmlFile: 'mday', timeFormat: "%A %H:%M", ticks: 4}, rrdGraph);
			$("#rrd-tabs-lweek").click({xmlFile: 'mweek', timeFormat: "Day %e", ticks: 8}, rrdGraph);
			$("#rrd-tabs-lmonth").click({xmlFile: 'mmonth', timeFormat: "Week %W", ticks: 4}, rrdGraph);
			$("#rrd-tabs-lyear").click({xmlFile: 'myear', timeFormat: "%e-%m", ticks: 14}, rrdGraph);
			$("#rrd-tabs-lhour").trigger("click", {xmlFile: 'mhour', timeFormat: "%H:%M"});
		},
		complete: function(){
						$('#loader').hide();
					}
	});
}
