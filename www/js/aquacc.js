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

function get_plannification(serial, outlet_no) {
    $('#loader').show();
		$.ajax({
			type: "POST",
			url: "aquacc.php?app=pdu&cmd=get_plannification",
			data: { html_header: 0, serial: serial, outlet_no: outlet_no },
			success: show_plannification,
			complete: function(){
      	$('#loader').hide();
      }
		});
}

function switch_outlet(serial, no, state) {
	if (state == "off") {
		outlet_off(serial, no, state);
	} else if (state == "on") {
		outlet_on(serial, no, state);
	}
}

function outlet_on(serial, outlet_no) {
        	$('#loader').show();
		$.ajax({
			type: "POST",
			url: "aquacc.php?app=pdu&cmd=outlet_on",
			data: { serial: serial, outlet_no: outlet_no },
			success: reload_outlet_status,
			complete: function(){
        			$('#loader').hide();
      			}
		});
}

function outlet_off(serial, outlet_no) {
        	$('#loader').show();
		$.ajax({
			type: "POST",
			url: "aquacc.php?app=pdu&cmd=outlet_off",
			data: { serial: serial, outlet_no: outlet_no },
			success: reload_outlet_status,
			complete: function(){
        			$('#loader').hide();
      			}
		});
}

function reload_outlet_status() {
  $('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=pdu&cmd=status",
		success: function(html) {
			$('#pdu-page').html(html);
		},
		complete: function(){
      $('#loader').hide();
    }
	});
}

function load_pdu_page() {
  $('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=pdu&cmd=status",
		success: function(html) {
			$('#pdu-page').html(html);
		},
		complete: function(){
			$('#loader').hide();
    }
	});
}

function load_rrd_page() {
	$('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=rrd&cmd=show-rrd",
		success: function(html) {
			$('#rrd-page').html(html);
		},
		complete: function(){
						$('#loader').hide();
					}
	});
}

setTimer_reload_pdu_page();
function setTimer_reload_pdu_page() {
	$(document).ready(function () {
		var interval = 10000;   //number of mili seconds between each call
		var refresh = function() {
			$.ajax({
				url: "aquacc.php?app=pdu&cmd=status",
				cache: false,
				success: function(html) {
					$('#pdu-page').html(html);
					setTimeout(function() {
						refresh();
					}, interval);
				}
			});
		};
		refresh();
	});
}

$(function() {
	$("#tabs").tabs();
	$("#PDU").click(load_pdu_page);
	$("#RRD").click(load_rrd_page);
});

function cMinutes2TimeString(minutes) {
	var h 	= Math.floor(minutes / 60);
	var m = ui.values[0] - (h * 60);
	return(h +':' + m);
}

function cTimeString2Minutes(timeString) {
	var t = timeString.split(":");
	var h = t[0];
	var m = t[1];
	m = parseInt(m, 10) + (parseInt(h, 10) * 60);
	return m;
}

function show_plannification(data) {
	var plan = JSON.parse(data);
	var modal = document.getElementById('myModal');
	var span = document.getElementsByClassName("close")[0];
	modal.style.display = "block";

	// When the user clicks on <span> (x), close the modal
	span.onclick = function() {
	    modal.style.display = "none";
	}

	var startTime = cTimeString2Minutes(plan[0].time);
	var endTime   = cTimeString2Minutes(plan[1].time);
	$("#pdu-start").html(plan[0].time);
	$("#pdu-stop").html(plan[1].time);
	$("#slider-range").slider({
		range: true,
		min: 0,
		max: 1440,
		step: 15,
		values: [ startTime, endTime ],
		slide: function(event, ui ) {
			var s_hours 	= Math.floor(ui.values[0] / 60);
			var s_minutes = ui.values[0] - (s_hours * 60);
			var e_hours 	= Math.floor(ui.values[1] / 60);
			var e_minutes = ui.values[1] - (e_hours * 60);

			if(s_hours.toString().length == 1) s_hours = '0' + s_hours;
			if(s_minutes.toString().length == 1) s_minutes = '0' + s_minutes;
			if(e_hours.toString().length == 1) e_hours = '0' + e_hours;
			if(e_minutes.toString().length == 1) e_minutes = '0' + e_minutes;


			//Example:[{"date":"2016-12-21","time":"08:00","status":1},{"date":"2016-12-21","time":"20:00","status":0}]
			$("#pdu-start").html(s_hours +':' + s_minutes);
			$("#pdu-stop").html(e_hours +':' + e_minutes);
		}
	});
}
