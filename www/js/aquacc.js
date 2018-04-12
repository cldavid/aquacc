$.ajaxSetup({async: false});
loadScript("js/dsu.js");
loadScript("js/pdu.js");
loadScript("js/rrd.js");
loadScript("js/rrd/rrd.js");
loadScript("js/helpers.js");
loadScript("js/init.js");
$.ajaxSetup({async: true});

function loadScript(scriptName) {
	$.getScript(scriptName, page_loaded).fail(function(jqxhr, settings, exception ) {
		console.log("Error loading page " + scriptName + "(" + exception + ")");
		loadScript(scriptName);
	});
}

function page_loaded(data, textStatus, jqxhr) {
	console.log("Load was performed (" + textStatus + ")");
}
