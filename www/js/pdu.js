var pduReloadTimer = null;

function reload_outlet_status(event) {
  var json_data = JSON.parse(event);
  var rcode     = json_data.rcode;
  if (rcode) {
    /*
     * Error, unable to fetch outlet status
     */
     return;
  }
  var serial    = json_data.data.serial;
  var sserial   = serial.replace(/\:/g, '');
  var outlet    = json_data.data.outlet;
  var status    = json_data.data.status;
  var id        = sserial + outlet;

  var div_status = document.getElementById("pdu-div-status-" + id);
  if (div_status == null) {
    alert("Element not found");
    return;
  }

  if (status) {
    div_status.className   = "pdu-table-status-on";
  } else {
    div_status.className   = "pdu-table-status-off";
  }
}

function pdu_createTable(data) {
    var json_data = JSON.parse(data);
    var rcode     = json_data.rcode;
    if (rcode) {
      /*
       * Error, unable to fetch outlet status
       */
       return;
    }
    var pdus = json_data.data;

    $('#pdu-page').empty();
    pdus.forEach(function(serial) {
      var sserial = serial.replace(/\:/g, '');
      var table   = document.createElement('table');
      table.setAttribute("id", "pdu-table-" + sserial);
      table.setAttribute("class", "pdu-table-class");
      table.setAttribute("pdu-serial", serial);

      var row     = table.insertRow();
      var cell    = row.insertCell();
      cell.setAttribute("class", "pdu-table-header");
      cell.innerHTML = "<b>PDU-" + serial + "</b>";
      cell.colSpan = 4;

      for (var outlet = 1; outlet < 5; outlet++) {
        var id = sserial + outlet;
        /* Create new row for each outlet*/
        row   = table.insertRow();

        /* Create TD ID */
        cell  = row.insertCell();
        cell.setAttribute("id",    "pdu-table-id-" + id);
        cell.setAttribute("class", "pdu-table-id");

        /* Create TD Name */
        cell  = row.insertCell();
        cell.setAttribute("id",    "pdu-table-name-" + id);
        cell.setAttribute("class", "pdu-table-name");
        $(cell).click({serial: serial, outlet: outlet}, edit_scheduler);
        cell.innerHTML  = "Outlet-" + outlet;

        /* Create TD Scheduler */
        cell  = row.insertCell();
        cell.setAttribute("id",    "pdu-table-scheduler-" + id);
        cell.setAttribute("class", "pdu-table-scheduler");

        /* Create TD Status */
        cell  = row.insertCell();
        cell.setAttribute("id",    "pdu-table-status-" + id);
        cell.setAttribute("class", "pdu-table-status");

        /* Create DIV Status */
        var div_status = document.createElement('div');
        div_status.setAttribute("id",    "pdu-div-status-" + id);
        cell.appendChild(div_status);
        $(cell).click({id: id, serial: serial, outlet: outlet}, switch_outlet);
      }
      $('#pdu-page').append(table);
      pdu_getStatus(serial);
    });
    setTimer_reload_pdu_page();
}

function pdu_refresh_table(event) {
  var json_data = JSON.parse(event);
  var rcode     = json_data.rcode;
  if (rcode) {
    /*
     * Error, unable to fetch outlet status
     */
     return;
  }
  var serial    = json_data.data.serial;
  var outlets   = json_data.data.outlets;
  var sserial   = serial.replace(/\:/g, '');

  var outlet    = 1;
  outlets.forEach(function(s) {
    var id        = sserial + outlet;
    var plan      = s.scheduler;
    var f_id      = document.getElementById("pdu-table-id-" + id);
    var f_name    = document.getElementById("pdu-table-name-" + id);
    var f_sched   = document.getElementById("pdu-table-scheduler-" + id);
    var f_status  = document.getElementById("pdu-div-status-" + id);

    f_id.innerHTML    = outlet;
    f_name.innerHTML  = "Outlet-" + outlet;

    if(plan.length) {
      f_sched.innerHTML = "";
      plan.forEach(function(p){
        f_sched.innerHTML += "Date: " + p.date + " Time: " + p.time + " Status: " + p.status + "<br/>";
      });
    } else {
      f_sched.innerHTML = "Scheduler disabled";
    }

    if (s.status) {
      f_status.setAttribute("class", "pdu-table-status-on");
    } else {
      f_status.setAttribute("class", "pdu-table-status-off");
    }
    outlet++;
  });
  var pdu_table = document.getElementById("pdu-table-" + sserial);
  pdu_table.style.display = "table";
}

function pdu_getStatus(serial) {
  $('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=pdu&cmd=getStatus",
    data: { html_header: 0, serial: serial },
    success: pdu_refresh_table,
		complete: function(){
			$('#loader').hide();
    }
	});
}

function load_pdu_page() {
  clearTimeout(pduReloadTimer);
  $('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=pdu&cmd=scan",
    data: { html_header: 0 },
    success: pdu_createTable,
		complete: function(){
			$('#loader').hide();
    }
	});
}

function pdu_scan() {
  $('#loader').show();
  $.ajax({
    type: "POST",
    cache: false,
    async: false,
    url: "aquacc.php?app=pdu&cmd=status",
    success: function(html) {
      $('#pdu-page').html(html);
    },
    complete: function(){
      $('#loader').hide();
    }
  });
}

function setTimer_reload_pdu_page() {
		var interval = 10000;   //number of mili seconds between each call
    var tables = $.find("table[pdu-serial]");
    tables.forEach(function(table) {
      var serial = table.getAttribute("pdu-serial");
      $.ajax({
        type: "POST",
        url: "aquacc.php?app=pdu&cmd=getStatus",
        cache: false,
        data: { html_header: 0, serial: serial },
        success: function(event) {
          pdu_refresh_table(event);
        }
      });
    });
    pduReloadTimer = setTimeout(setTimer_reload_pdu_page, interval);
}

function switch_outlet(event) {
  var id          = event.data.id;
  var serial      = event.data.serial;
  var outlet      = event.data.outlet;
  var state       = event.data.state;
  var div_status  = document.getElementById("pdu-div-status-" + id);

  if (div_status.className == "pdu-table-status-on") {
    outlet_off(serial, outlet);
  } else {
    outlet_on(serial, outlet);
  }
}

function outlet_on(serial, outlet_no) {
  $('#loader').show();
		$.ajax({
			type: "POST",
			url: "aquacc.php?app=pdu&cmd=outlet_on",
			data: { html_header: 0, serial: serial, outlet_no: outlet_no },
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
			data: { html_header: 0, serial: serial, outlet_no: outlet_no },
			success: reload_outlet_status,
			complete: function(){
      	$('#loader').hide();
      }
		});
}

function parse_set_plannification_result(event) {
  var result  = JSON.parse(event);
  var serial  = result.data.serial;
  var outlet  = result.data.outlet;
  var plan    = result.data.scheduler;

  if (result.rcode) {
    alert("Error updating scheduler:\n" + result.command + "\nOutput:" + result.output);
    return;
  }

  var sserial = serial.replace(/\:/g, '');
  var id      = sserial + outlet;
  var cell    = document.getElementById("pdu-table-scheduler-" + id);
  if(plan.length) {
    cell.innerHTML = "";
    plan.forEach(function(p){
      cell.innerHTML += "Date: " + p.date + " Time: " + p.time + " Status: " + p.status + "<br/>";
    });
  } else {
    cell.innerHTML = "Scheduler disabled";
  }
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
		success: parse_set_plannification_result,
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
		success: parse_set_plannification_result,
		complete: function(){
			$('#loader').hide();
		}
	});
}

function get_plannification(serial, outlet_no, cb) {
    $('#loader').show();
		$.ajax({
			type: "POST",
			url: "aquacc.php?app=pdu&cmd=get_plannification",
			data: { html_header: 0, serial: serial, outlet_no: outlet_no },
			success: cb,
			complete: function(){
      	$('#loader').hide();
      }
		});
}

function edit_scheduler(event) {
    var serial = event.data.serial;
    var outlet_no = event.data.outlet;
    get_plannification(serial, outlet_no, show_plannification);
}

/*
 * data.scheduler contains following data structure
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
function show_plannification(event) {
  var json_data   = JSON.parse(event);
  var rcode       = json_data.rcode;
  if (rcode) {
    /*
     * Error, unable to fetch outlet status
     */
     return;
  }
  var data     = json_data.data;
	var plan     = data.scheduler;
	var modal    = document.getElementById('myModal');
	var span     = document.getElementsByClassName("close")[0];
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
	$("#pdu-schedule-serial").text(data.serial);
	$("#pdu-schedule-outlet").text(data.outlet);
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
