$(function() {
	$("#tabs").tabs();
	$("#PDU").click(load_pdu_page);
	$("#RRD").click(load_rrd_page);
	$("#scheduler-button-ok").button();
	$("#scheduler-button-cancel").button();
	$("#scheduler-button-apply").button();
	$("#scheduler-button-ok").click(handle_scheduler_button_ok);
	$("#scheduler-button-cancel").click(handle_scheduler_button_cancel);
	$("#scheduler-button-apply").click(handle_scheduler_button_apply);
});
