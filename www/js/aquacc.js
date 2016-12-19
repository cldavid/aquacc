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
			$('#div-sis-pm').html(html);
		},
		complete: function(){
        		$('#loader').hide();
      		}
	});
}

setTimer_example();
function setTimer_example() {
	$(document).ready(function () {
		var interval = 10000;   //number of mili seconds between each call
		var refresh = function() {
			$.ajax({
				url: "aquacc.php?app=pdu&cmd=status",
				cache: false,
				success: function(html) {
					$('#div-sis-pm').html(html);
					setTimeout(function() {
						refresh();
					}, interval);
				}
			});
		};
			refresh();
	});
}
