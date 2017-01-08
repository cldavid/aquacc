function stripLeadingZeroes(input) {
	if((input.length > 1) && (input.substr(0,1) == "0"))
		return input.substr(1);
	else
		return input;
}

function getDateString(d) {
	var mday	= d.getDate();
	var month = d.getMonth()+1;

	if(mday.toString().length == 1) mday = '0' + mday;
	if(month.toString().length == 1) month = '0' + month;

	var curDate =  d.getFullYear() + '-' + month  + '-' + mday;
	return curDate;
}

function getTimeString(d) {
	var h	= d.getHours();
	var m = d.getMinutes();

	if(h.toString().length == 1) h = '0' + h;
	if(m.toString().length == 1) m = '0' + m;

	var curTime =  h + ':' + m;
	return curTime;
}

function cMinutes2TimeString(minutes) {
	var h = Math.floor(minutes / 60);
	var m = minutes - (h * 60);
	if(h.toString().length == 1) h = '0' + h;
	if(m.toString().length == 1) m = '0' + m;
	return(h +':' + m);
}

function cTimeString2Minutes(timeString) {
	var t = timeString.split(":");
	var h = t[0];
	var m = t[1];
	m = parseInt(m, 10) + (parseInt(h, 10) * 60);
	return m;
}
