function load_rrd_page() {
	clearTimeout(pduReloadTimer);
	$('#loader').show();
	$.ajax({
		type: "POST",
		cache: false,
		url: "aquacc.php?app=rrd&cmd=show-rrd",
		success: function(html) {
			$('#rrd-page').html(html);
		},
		complete: function(){
						$('#loader').hide();
					}
	});
}
