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
