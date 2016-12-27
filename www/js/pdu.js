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

function disable_plannification(serial, outlet_no) {
	$('#loader').show();
	$.ajax({
		type: "POST",
		url: "aquacc.php?app=pdu&cmd=disable_plannification",
		data: {
			html_header: 0,
			serial: serial,
			outlet_no: outlet_no,
		},
		success: function(html) {$('#debug-message').text(html);},
		complete: function(){
			$('#loader').hide();
		}
	});
}

function set_plannification(serial, outlet_no, scheduler, loopMinutes) {
	$('#loader').show();
	$.ajax({
		type: "POST",
		url: "aquacc.php?app=pdu&cmd=set_plannification",
		data: {
			html_header: 0,
			serial: serial,
			outlet_no: outlet_no,
			scheduler: JSON.stringify(scheduler),
			loop_min: loopMinutes,
		 },
		success: function(html) {$('#debug-message').text(html);},
		complete: function(){
			$('#loader').hide();
		}
	});
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

/*
 * dev_plan.scheduler contains following data structure
 * Example:
 * [
 *	{
 *		"date":"2016-12-21",
 *		"time":"08:00",
 *		"status":1
 *	},
 *	{
 *		"date":"2016-12-21",
 *		"time":"20:00",
 *		"status":0
 *	}
 * ]
*/
function show_plannification(data) {
	var dev_plan = JSON.parse(data);
	var plan  = dev_plan.scheduler;
	var modal = document.getElementById('myModal');
	var span = document.getElementsByClassName("close")[0];
	modal.style.display = "block";

	// When the user clicks on <span> (x), close the modal
	span.onclick = function() {
	    modal.style.display = "none";
	}

	var startTime_str;
	var endTime_str;
	if (plan.length) {
		startTime_str = plan[0].time;
		endTime_str   = plan[1].time;
	} else {
		startTime_str = '00:00';
		endTime_str   = '00:00';
	}

	var startTime = cTimeString2Minutes(startTime_str);
	var endTime   = cTimeString2Minutes(endTime_str);
	if (endTime < startTime) {
		var tmp = endTime;
		endTime = startTime;
		startTime = tmp;

		tmp = endTime_str;
		endTime_str = startTime_str;
		startTime_str = tmp;
	}
	$("#pdu-schedule-serial").text(dev_plan.serial);
	$("#pdu-schedule-outlet").text(dev_plan.outlet);
	$("#pdu-schedule-time").text(startTime_str + '-' + endTime_str);
	$("#pdu-schedule-slider").slider({
		range: true,
		min: 0,
		max: 1440,
		step: 15,
		values: [ startTime, endTime ],
		slide: function(event, ui ) {
			var start = cMinutes2TimeString(ui.values[0]);
			var stop  = cMinutes2TimeString(ui.values[1]);

			$("#pdu-schedule-time").text(start + '-' + stop);
		}
	});
}

function create_scheduler_data(startTime, endTime) {
		var curDate			= new Date();
		var arr_sTime 	= startTime.split(':');
		var arr_eTime 	=	endTime.split(':');
		var	sDate 			= new Date(curDate.getFullYear(), curDate.getMonth(), curDate.getDate(), arr_sTime[0], arr_sTime[1], 0, 0);
		var	eDate 			= new Date(curDate.getFullYear(), curDate.getMonth(), curDate.getDate(), arr_eTime[0], arr_eTime[1], 0, 0);

		/*
		 * For simplicity I've hardcoded everything to resemble basic scheduling functionality
		 */
		var on 	= 1;
		var off = 0;
		var loopMinutes	= 1440;

		/*
		 * Swap dates if current time equals or greater then start or end time
		 */
		if (curDate.getTime() >= sDate.getTime()) {
			if (curDate.getTime() >= eDate.getTime()) {
				var tomorrow = new Date();
				tomorrow.setDate(curDate.getDate() + 1);
				var date1 	= getDateString(tomorrow);
				var date2 	= getDateString(tomorrow);
				var time1 	= startTime;
				var time2 	= endTime;
				var status1 = on;
				var status2 = off;
			} else {
				var tomorrow = new Date();
				tomorrow.setDate(curDate.getDate() + 1);
				var date1 	= getDateString(curDate);
				var date2 	= getDateString(tomorrow);
				var time1 	= endTime;
				var time2 	= startTime;
				var status1 = off;
				var status2 = on;
			}
		} else {
			var date1 	= getDateString(curDate);
			var date2 	= getDateString(curDate);
			var time1 	= startTime;
			var time2 	= endTime;
			var status1 = on;
			var status2 = off;
		}
		var scheduler = [ {date: date1, time: time1, status: status1},
											{date: date2, time: time2, status: status2}];
		return scheduler;
}

function handle_scheduler_button_disable(event) {
	event.preventDefault();
	var modal = document.getElementById('myModal');
  var serial 		= $("#pdu-schedule-serial").text();
  var outlet_no = $("#pdu-schedule-outlet").text();

  disable_plannification(serial, outlet_no);

  modal.style.display = "none";
}

function handle_scheduler_button_ok(event) {
	event.preventDefault();
	var modal = document.getElementById('myModal');
	var serial 		= $("#pdu-schedule-serial").text();
	var outlet_no = $("#pdu-schedule-outlet").text();
	var dTime 		= $("#pdu-schedule-time").text();

	var aTime 		= dTime.split('-');
	var startTime = aTime[0];
	var endTime 	= aTime[1];
	var loopMinutes	= 1440;
	var scheduler = create_scheduler_data(startTime, endTime, loopMinutes)
	set_plannification(serial, outlet_no, scheduler);
	modal.style.display = "none";
}

function handle_scheduler_button_cancel(event) {
	event.preventDefault();
	var modal = document.getElementById('myModal');
	modal.style.display = "none";
}

function handle_scheduler_button_apply(event) {
	event.preventDefault();
	var serial 		= $("#pdu-schedule-serial").text();
	var outlet_no = $("#pdu-schedule-outlet").text();
	var dTime 		= $("#pdu-schedule-time").text();

	var aTime 		= dTime.split('-');
	var startTime = aTime[0];
	var endTime 	= aTime[1];
	var loopMinutes	= 1440;
	var scheduler = create_scheduler_data(startTime, endTime)
	set_plannification(serial, outlet_no, scheduler, loopMinutes);
}
