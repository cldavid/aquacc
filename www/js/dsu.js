function dsu_handle_button_edit(event) {
  event.preventDefault();
  $('#dsuModal').css('display', 'block');

  $(".close").click(function () {
    $('#dsuModal').css('display', 'none');
  });

  var motorid   = $('#dsu-show-motor').text();
  var starttime = $('#dsu-show-starttime').text();
  var endtime   = $('#dsu-show-endtime').text();
  var duration  = $('#dsu-show-duration').text();
  var frequency = $('#dsu-show-frequency').text();

  $('#dsu-edit-motor').val(motorid);
  $('#dsu-edit-motor').prop("disabled", true);

//  $('#dsu-edit-starttime').text(starttime);
  $("#dsu-edit-starttime").timepicker({
    'scrollDefault': 'now',
    'timeFormat': 'H:i:s',
    'step': 5
  });
//  $('#dsu-edit-endtime').text(endtime);
  //$('#dsu-edit-duration').selectmenu();
  $('#dsu-edit-startdate').datepicker();
  $('#dsu-edit-frequency').selectmenu().selectmenu("menuWidget").addClass("dsu-edit-frequency-button-overflow");
  $('#dsu-edit-duration').spinner({
    min: 0,
    max: 12,
    start: 1,
    numberFormat: "s",
    spin: function( event, ui ) {
      if ( ui.value > 10 ) {
        $( this ).spinner( "value", 0 );
        return false;
      } else if ( ui.value < 0 ) {
        $( this ).spinner( "value", 10 );
        return false;
      }
    }
  });
//  $('#dsu-edit-duration').text(duration);
//  $('#dsu-edit-frequency').text(frequency);
}

function dsu_handle_button_ok(event) {
  event.preventDefault();
  $('#dsuModal').css('display', 'none');
}

function dsu_handle_button_cancel(event) {
  event.preventDefault();
  $('#dsuModal').css('display', 'none');
}

function dsu_handle_button_disable(event) {
  event.preventDefault();
  $('#dsuModal').css('display', 'none');
}

function dsu_show_pump_info(data) {
  var json_data = JSON.parse(data);
  var rcode     = json_data.rcode;
  var data      = json_data.data;

  if (rcode) {
    /* Error */
    return;
  }

  var starttime = new Date(data.starttime * 1000);
  var endtime   = new Date(data.endtime * 1000);
  var start_str = getDateString(starttime) + '-'  + getTimeString(starttime);
  var end_str   = getDateString(endtime) + '-'    + getTimeString(endtime);
  var every     = displayEvery(data.frequency);

  $('#dsu-show-pump').css('display', 'inline-block');
  $('#dsu-show-motor').text(data.motorid);
  $('#dsu-show-starttime').text(starttime.toString());
  $('#dsu-show-endtime').text(endtime.toString());
  $('#dsu-show-duration').text(data.duration);
  $('#dsu-show-frequency').text(every);
}

function dsu_get_pump_info(event) {
  var motor_id  = event.data.motor_id;
  var callback  = event.data.callback;
  $('#loader').show();
  $.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=dsu&cmd=getMotorInfo",
    data: { html_header: 0, motor_id: motor_id },
		success: callback,
		complete: function(){
			$('#loader').hide();
    }
	});
}

function load_dsu_page() {
  clearTimeout(pduReloadTimer);
  $('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=dsu&cmd=show-dsu",
    data: { html_header: 0 },
		success: function(html) {
			$('#dsu-page').html(html);
      $("#dsu-tabs").tabs();
      $("#dsu-tabs-pump1").click({motor_id: 0, callback: dsu_show_pump_info}, dsu_get_pump_info);
      $("#dsu-tabs-pump2").click({motor_id: 1, callback: dsu_show_pump_info}, dsu_get_pump_info);
      $("#dsu-tabs-pump3").click({motor_id: 2, callback: dsu_show_pump_info}, dsu_get_pump_info);
      $("#dsu-button-edit").button();
      $("#dsu-button-edit").click(dsu_handle_button_edit);
      $("#dsu-button-ok").button();
      $("#dsu-button-ok").click(dsu_handle_button_ok);
      $("#dsu-button-cancel").button();
      $("#dsu-button-cancel").click(dsu_handle_button_cancel);
      $("#dsu-button-disable").button();
      $("#dsu-button-disable").click(dsu_handle_button_disable);
		},
		complete: function(){
			$('#loader').hide();
    }
	});
}

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

function set_motor_start(theForm) {
	var epoch = humanToTime(theForm);
	var hiddenField = document.createElement("input");
	hiddenField.setAttribute("type", "hidden");
	hiddenField.setAttribute("name", "motor_start");
	hiddenField.setAttribute("value", epoch);

	theForm.appendChild(hiddenField);
}

function displayEvery(seconds) {
  var every;

	switch(parseInt(seconds)) {
		case 1:
			every = "second";
			break;

		case 60:
			every = "min";
			break;

		case 3600:
			every = "hour";
			break;

		case 7200:
			every = "two hours";
			break;

		case 14400:
			every = "four hours";
			break;

		case 28800:
			every = "eight hours";
			break;

		case 43200:
			every = "twelve hours";
			break;

		case 86400:
			every = "day";
			break;

		case 172800:
			every = "two days";
			break;

		case 604800:
			every = "week";
			break;

		case 1209600:
			every = "two weeks";
			break;

		default:
			every=seconds;
	}
	return every;
}
