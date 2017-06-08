function drawMe(series, formater, ticks) {
	var margin = {top: 20, right: 150, bottom: 30, left: 50};
	var width = 680 - margin.left - margin.right;
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

	var xAxis = d3.axisBottom(x).tickFormat(timeFormat).ticks(ticks);
	var yAxis = d3.axisLeft(y);

	var svg = d3.select("svg")
		.attr("width", width + margin.left + margin.right)
		.attr("height", height + margin.top + margin.bottom);

	var g = svg.append("g").attr("transform", "translate(" + margin.left + "," + margin.top + ")");

	//Grid Lines
	g.append("g").attr("class", "grid").attr("transform", "translate(0," + height + ")")
		.call(d3.axisBottom(x).ticks(ticks).tickSize(-height).tickFormat(""));
	g.append("g").attr("class", "grid").call(d3.axisLeft(y).tickSize(-width).tickFormat(""));

	//Axis
	g.append("g").attr("class", "axis").attr("transform", "translate(0," + height + ")").call(xAxis);
	g.append("g").attr("class", "axis").call(yAxis)
		.append("text")
		.attr("transform", "rotate(270)")
		.attr("y", 10)
		.attr("dy", "0.71em")
		.attr("fill", "white")
		.text("Temperature, ºC");

	//Plot data
	var temp = g.selectAll(".temp").data(series).enter().append("g").attr("class", "temp");
	temp.append("path").attr("class", "line").attr("d", function(d) {return line(d.data); }).style("stroke", function(d,i) { return z(i); });
	temp.append("text")
		.datum(function(d) { return {id: d.name, value: d.data[d.data.length - 1]}; })
		.attr("transform", function(d) { return "translate(" + x(d.value.time) + "," + y(d.value.value) + ")"; })
		.attr("x", 3)
		.attr("dy", "0.35em")
		.style("font", "12px sans-serif")
		.text(function(d) { return d.id; });

}
