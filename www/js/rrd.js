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

	$.ajax({
		type: "GET",
		url: "/aquacc/xml/" + xmlFile + ".xml",
		dataType: "xml",
		success: function(xml) {
			var series = parseRRD_XML(xml);
			drawMe(series);
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
			$("#rrd-tabs-lhour").click({xmlFile: 'mhour'}, rrdGraph);
			$("#rrd-tabs-lday").click({xmlFile: 'mday'}, rrdGraph);
			$("#rrd-tabs-lweek").click({xmlFile: 'mweek'}, rrdGraph);
			$("#rrd-tabs-lmonth").click({xmlFile: 'mmonth'}, rrdGraph);
			$("#rrd-tabs-lyear").click({xmlFile: 'myear'}, rrdGraph);
			$("#rrd-tabs-lhour").trigger("click", {xmlFile: 'mhour'});
		},
		complete: function(){
						$('#loader').hide();
					}
	});
}
