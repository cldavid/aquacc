$.getScript( "js/dsu.js", page_loaded);
$.getScript( "js/pdu.js", page_loaded);
$.getScript( "js/rrd.js", page_loaded);
$.getScript( "js/helpers.js", page_loaded);
$.getScript( "js/init.js", page_loaded);

function page_loaded(data, textStatus, jqxhr) {
	console.log("Load was performed (" + textStatus + ")");
}
