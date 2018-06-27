Drupal.behaviors.phonerecorderfield = function (context) {
	$(".hangup-button").attr('disabled', 'disabled');
	$(".phonerecorderfield-hidden").hide();
	phonerecorderfield_attach_events();
}

function phonerecorderfield_attach_events(){
	//Automatically select radio when typing in textfield
	$(".phonerecorderfield-type").focus(function() { 
		$(this).prev().attr("checked","checked");
	});  
	
	//Automatically select radio when focusing on select list
	$(".phonerecorderfield-voipnumber").focus(function() { 
		$(this).prev().attr("checked","checked");
	});  
}

function phonerecorder(field_name,delta,node_type) { 
	//Disable the button
	$("#call-me-button-"+field_name+"-"+delta).attr('disabled', 'disabled');	
	
	if($("#call-me-button-"+field_name+"-"+delta).val()=='Call Me'){
		//Upload button
		var phone=$("#phonerecorderfield-"+field_name+"-"+delta+"-message input:radio:checked").val();
		if(phone=="voipnumber"){
			//Get the phone number from select list
			phone=$("#phonerecorderfield-"+field_name+"-"+delta+"-select :selected").val();
		}
		else if(phone=="type"){
			phone=$("#phonerecorderfield-"+field_name+"-"+delta+"-type-phone").val();
		}
		
		//Hide the phone number selection
	    $("#phonerecorderfield-"+field_name+"-"+delta+"-hidden").html($("#phonerecorderfield-"+field_name+"-"+delta+"-message").html());
		$("#phonerecorderfield-"+field_name+"-"+delta+"-message").html("Calling...");
		//Enable hangup button
		$("#hangup-button-"+field_name+"-"+delta).removeAttr("disabled");	
		$.ajax({
			type: "GET",
			url: Drupal.settings.basePath +"?q=phonerecorderfield/call",
			data: "phone=" + encodeURIComponent(phone) +"&field_name="+field_name+"&node_type="+node_type,
			dataType: 'json',
			success: function(data){
						phonerecorderResponse(data,field_name,delta,node_type);
					}
		});
	}
	else{
		//Remove button
		$("#edit-field-"+field_name+"-"+delta+"-value").val("");
		$("#edit-field-"+field_name+"-"+delta+"-fid").val("");
		$("#phonerecorderfield-"+field_name+"-"+delta+"-message").html($("#phonerecorderfield-"+field_name+"-"+delta+"-hidden").html());
		$("#phonerecorderfield-"+field_name+"-"+delta+"-hidden").html("");
		phonerecorderfield_attach_events();
		$("#call-me-button-"+field_name+"-"+delta).removeAttr("disabled");
		$("#call-me-button-"+field_name+"-"+delta).val("Call Me");
		$("#hangup-button-"+field_name+"-"+delta).show();
	}
} 

function phonerecorderfieldHangup(field_name,delta){
	
	var cid=$("#edit-field-"+field_name+"-"+delta+"-value").val();
	$.ajax({
			type: "GET",
			url: Drupal.settings.basePath +"?q=phonerecorderfield/hangup",
			data: "cid=" + cid,
			dataType: 'json',
			success: function(data){
						if(data.status){
							$("#phonerecorderfield-"+field_name+"-"+delta+"-message").html($("#phonerecorderfield-"+field_name+"-"+delta+"-hidden").html());
							$("#phonerecorderfield-"+field_name+"-"+delta+"-hidden").html("");
							phonerecorderfield_attach_events();
							$("#call-me-button-"+field_name+"-"+delta).val("Call Me");
							$("#edit-field-"+field_name+"-"+delta+"-fid").val("");
							$("#call-me-button-"+field_name+"-"+delta).removeAttr("disabled");
							$("#hangup-button-"+field_name+"-"+delta).attr('disabled', 'disabled');
						}	
					}
	});
}

function phonerecorderResponse(data,field_name,delta,node_type){
	$("#edit-field-"+field_name+"-"+delta+"-value").val(data.cid);
	// Check for recorded message after 5 sec.
	window.setTimeout(function() {
		phonerecorderCheck(data.cid,field_name,delta,node_type);
	}, 5000);

}

function phonerecorderCheck(cid,field_name,delta,node_type){
$.ajax({
		type: "GET",
		url: Drupal.settings.basePath +"?q=phonerecorderfield/get/recording",
		data: "cid=" + cid + "&field_name=" + field_name+"&delta="+delta+"&node_type="+node_type,
		dataType: 'json',
		success: function(data){
					if(data.status=="success"){
						//Call is sucessfully recorded
						$("#edit-field-"+field_name+"-"+delta+"-fid").val(data.fid);
						$("#phonerecorderfield-"+field_name+"-"+delta+"-message").html(data.file);
						$("#call-me-button-"+field_name+"-"+delta).val("Remove");
						$("#call-me-button-"+field_name+"-"+delta).removeAttr("disabled");
						$("#hangup-button-"+field_name+"-"+delta).attr('disabled', 'disabled');
						$("#hangup-button-"+field_name+"-"+delta).hide();
					}
					else if(data.status=="failed"){
						//Call is hanguped (busy, not answered, error, unavailable, ...)
						$("#phonerecorderfield-"+field_name+"-"+delta+"-message").html(data.message);
						$("#phonerecorderfield-"+field_name+"-"+delta+"-hidden").html("");
						phonerecorderfield_attach_events();
						$("#call-me-button-"+field_name+"-"+delta).removeAttr("disabled");	
						$("#hangup-button-"+field_name+"-"+delta).attr('disabled', 'disabled');					
					}
					else{
					//Call is not finished yet, check again after 2 sec.
						window.setTimeout(function() {
							phonerecorderCheck(cid,field_name,delta,node_type);
						}, 2000);
					}
				}
	});
}
