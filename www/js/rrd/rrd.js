function drawMe(series, formater) {
	var margin = {top: 20, right: 80, bottom: 30, left: 50};
	var width = 600 - margin.left - margin.right;
	var height = 300 - margin.top - margin.bottom;
	var timeFormat = d3.timeFormat(formater);

	var x = d3.scaleTime().range([0, width]);
	var y = d3.scaleLinear().range([height, 0]);
	var z = d3.scaleOrdinal(d3.schemeCategory10);

	// Scale the range of the data
	x.domain(d3.extent(series[0].data, function(d) {return d.time;}));
	y.domain([0,
//		d3.min(series, function(s) { return d3.min(s.data, function(d) { return d.value; }); }),
		d3.max(series, function(s) { return d3.max(s.data, function(d) { return d.value; }); })
	]);

	var line = d3.line()
		.curve(d3.curveBasis)
		.x(function(d) {return x(d.time); })
		.y(function(d) {return y(d.value); });

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
	temp.append("path").attr("class", "line").attr("d", function(d) {return line(d.data); });

}
