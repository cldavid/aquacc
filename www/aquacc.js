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
