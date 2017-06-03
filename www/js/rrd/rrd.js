function testMe(series) {
	var svg = d3.select("svg");
	var margin = {top: 20, right: 80, bottom: 30, left: 50};
  var width = 600 - margin.left - margin.right;
  var height = 300 - margin.top - margin.bottom;

	series.forEach(function(s) {
		console.log("S: " + s.name);
		s.data.forEach(function(d) {
			console.log("T: " + d.time + " V: " + d.value);
		})
	});

	var timeFormat = d3.timeFormat("%H:%M");

	var x = d3.scaleTime().range([0, width]);
	var y = d3.scaleLinear().range([height, 0]);
	var z = d3.scaleOrdinal(d3.schemeCategory10);

	// Scale the range of the data
	x.domain(d3.extent(series[0].data, function(d) { console.log("time: " + d.time); return d.time; }));
	//y.domain([0, 50]);
	y.domain([0,
//		d3.min(series, function(s) { return d3.min(s.data, function(d) { return d.value; }); }),
		d3.max(series, function(s) { return d3.max(s.data, function(d) { return d.value; }); })
	]);

	var line = d3.line()
		.curve(d3.curveBasis)
		.x(function(d) { console.log("xl: " + d.time);return x(d.time); })
		.y(function(d) { console.log("yl: " + d.value);return y(d.value); });

		var xAxis = d3.axisBottom(x).tickFormat(timeFormat);
		var yAxis = d3.axisLeft(y);

		var svg = d3.select("svg")
		.attr("width", width + margin.left + margin.right)
		.attr("height", height + margin.top + margin.bottom);

		var g = svg.append("g").attr("transform", "translate(" + margin.left + "," + margin.top + ")");
		g.append("g").attr("class", "axis").attr("transform", "translate(0," + height + ")").call(xAxis);
		g.append("g").attr("class", "axis").call(yAxis)
		  .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", "0.71em")
      .attr("fill", "white")
      .text("Temperature, ºC");

		var temp = g.selectAll(".temp").data(series).enter().append("g").attr("class", "temp");
		temp.append("path").attr("class", "line").attr("d", function(d) { console.log("n: " + d.name); return line(d.data); });

}

function drawMe() {
	$.ajax({
		type: "GET",
		url: "/aquacc/xml/mhour.xml",
		dataType: "xml",
		success: function(xml) {
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
			testMe(series);
		},
		error: function(request, msg) {
			alert(msg);
		}
	})
}
