function dsu_handle_button_edit(event) {
  event.preventDefault();
  var currDate = new Date();

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

  $("#dsu-edit-starttime").timepicker({
    'scrollDefault': 'now',
    'timeFormat': 'H:i:s',
    'step': 5
  });
  $("#dsu-edit-starttime").timepicker('setTime', currDate);

  $('#dsu-edit-startdate').datepicker({
    minDate: 0,
    maxDate: "+1M +10D"
  });
  $("#dsu-edit-startdate").datepicker('setDate', "+1d");

  $('#dsu-edit-frequency').selectmenu().selectmenu("menuWidget").addClass("dsu-edit-frequency-button-overflow");
  $('#dsu-edit-duration').spinner({
    min: -1,
    max: 11,
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
}

function dsu_handle_button_dose(event) {
  event.preventDefault();
  $('#dsuDriveModal').css('display', 'block');
  $(".close").click(function () {
    $('#dsuDriveModal').css('display', 'none');
  });

  var motorid   = $('#dsu-show-motor').text();
  $('#dsu-drive-motor').val(motorid);
  $('#dsu-drive-motor').prop("disabled", true);
  $('#dsu-drive-volume').spinner({
    min: -1,
    max: 11,
    start: 1,
    numberFormat: "s",
    spin: function(event, ui) {
      if (ui.value > 10) {
        $(this).spinner("value", 0);
        return false;
      } else if (ui.value < 0) {
        $(this).spinner("value", 10);
        return false;
      }
    }
  });
}

function dsu_handle_button_ok(event) {
  event.preventDefault();
  var motor_id  = $('#dsu-edit-motor').val();
  var startdate = $('#dsu-edit-startdate').datepicker("getDate");
  var starttime = $('#dsu-edit-starttime').timepicker('getTime', startdate);
  var motor_for   = $('#dsu-edit-duration').val();
  var motor_every = $('#dsu-edit-frequency').val();
  var motor_start = parseInt(starttime.getTime() / 1000);

  dsu_set_pump_info(motor_id, motor_start, motor_for, motor_every);
  $('#dsuModal').css('display', 'none');
}

function dsu_handle_button_cancel(event) {
  event.preventDefault();

  $('#dsuModal').css('display', 'none');
}

function dsu_handle_button_disable(event) {
  event.preventDefault();
  var motor_id  = $('#dsu-edit-motor').val();

  dsu_disable_motor(motor_id);
  $('#dsuModal').css('display', 'none');
}

function dsu_handle_drive_button_ok(event) {
  event.preventDefault();

  $('#dsuDriveModal').css('display', 'none');
}

function dsu_handle_drive_button_dose(event) {
  event.preventDefault();

  var motor_id  = parseInt($('#dsu-drive-motor').val());
  var motor_vol = parseInt($('#dsu-drive-volume').val());

  dsu_drive_pump(motor_id, motor_vol);

  $('#dsuDriveModal').css('display', 'none');
}

function dsu_drive_pump(motor_id, motor_volume) {
  $('#loader').show();
  $.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=dsu&cmd=driveMotor",
    data: {
      html_header: 0,
      motor_id: motor_id,
      motor_volume: motor_volume,
    },
		success: function () {
    },
		complete: function(){
			$('#loader').hide();
    }
	});
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

function dsu_disable_motor(motor_id) {
  $('#loader').show();
  $.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=dsu&cmd=disableMotor",
    data: {
      html_header: 0,
      motor_id: motor_id,
    },
		success: function () {
      $("#dsu-tabs-pump1").trigger("click",{motor_id: motor_id, callback: dsu_show_pump_info});
    },
		complete: function(){
			$('#loader').hide();
    }
	});
}

function dsu_set_pump_info(motor_id, motor_start, motor_for, motor_every) {
  $('#loader').show();
  $.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=dsu&cmd=setMotorInfo",
    data: {
      html_header: 0,
      motor_id: motor_id,
      motor_start: motor_start,
      motor_for: motor_for,
      motor_every: motor_every
    },
		success: function () {
      $("#dsu-tabs-pump1").trigger("click",{motor_id: motor_id, callback: dsu_show_pump_info});
    },
		complete: function(){
			$('#loader').hide();
    }
	});
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
  clearTimeout(rrdReloadTimer);
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
      $("#dsu-tabs-pump1").trigger("click",{motor_id: 0, callback: dsu_show_pump_info});

      $("#dsu-button-edit").button();
      $("#dsu-button-edit").click(dsu_handle_button_edit);
      $("#dsu-button-dose").button();
      $("#dsu-button-dose").click(dsu_handle_button_dose);

      $("#dsu-button-ok").button();
      $("#dsu-button-ok").click(dsu_handle_button_ok);
      $("#dsu-button-cancel").button();
      $("#dsu-button-cancel").click(dsu_handle_button_cancel);
      $("#dsu-button-disable").button();
      $("#dsu-button-disable").click(dsu_handle_button_disable);

      $("#dsu-drive-button-ok").button();
      $("#dsu-drive-button-ok").click(dsu_handle_drive_button_ok);
      $("#dsu-drive-button-dose").button();
      $("#dsu-drive-button-dose").click(dsu_handle_drive_button_dose);
		},
		complete: function(){
			$('#loader').hide();
    }
	});
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

		case 21600:
			every = "six hours";
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
