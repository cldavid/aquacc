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
				series[index].data.push({time: t, value: +v});
			};
		});
	});
	return series;
}

function rrdGraph(event) {
	var xmlFile  = event.data.xmlFile;
	var formater = event.data.timeFormat;
	var ticks		 = event.data.ticks;

	$('#d3js-rrd-graph').empty();

	$.ajax({
		type: "GET",
		url: "/aquacc/xml/" + xmlFile + ".xml",
		dataType: "xml",
		success: function(xml) {
			var series = parseRRD_XML(xml);
			drawMe(series, formater, ticks);
		},
		error: function(request, msg) {
			alert(msg);
		}
	})
}

function load_rrd_page() {
	clearTimeout(pduReloadTimer);
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
			$("#rrd-tabs-lyear").click({xmlFile: 'myear', timeFormat: "%e %B", ticks: 14}, rrdGraph);
			$("#rrd-tabs-lhour").trigger("click", {xmlFile: 'mhour', timeFormat: "%H:%M"});
		},
		complete: function(){
						$('#loader').hide();
					}
	});
}
