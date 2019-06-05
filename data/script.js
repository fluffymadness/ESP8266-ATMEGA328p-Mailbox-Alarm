//Global
function getHostname() {
	$.ajax({
		url: "getHostname",
		type: 'GET',
		success: function(res) {
			$(document).prop("title", res);
			$("#menutitle").html(res);
		}
	});
}
