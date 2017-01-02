$(function() {
	$("#tabs").tabs();
	$("#DSU").click(load_dsu_page);
	$("#PDU").click(load_pdu_page);
	$("#RRD").click(load_rrd_page);
	$("#scheduler-button-disable").button();
	$("#scheduler-button-ok").button();
	$("#scheduler-button-cancel").button();
	$("#scheduler-button-apply").button();
	$("#scheduler-button-disable").click(handle_scheduler_button_disable);
	$("#scheduler-button-ok").click(handle_scheduler_button_ok);
	$("#scheduler-button-cancel").click(handle_scheduler_button_cancel);
	$("#scheduler-button-apply").click(handle_scheduler_button_apply);
});
