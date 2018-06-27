(function( $ ){
	
	$.fn.callme = function( options ) {
	
		var thisPhone = this, dispatcher = $({}), callMeOptions = {}, call; 
		var settings = {
			token : "",
			dialpad: true,
			//togglemic: true,
			slideopen: true,
			buttontext: "loading...",
			buttontextready: "Call me",
			numbertodial: "app:9991457008",
    	};
    	
    	if(options) {
	    	//lowercase all options passed in
	    	$.each(options, function(k,v){
	    		callMeOptions[k.toLowerCase()] = v;
	    	});
    	}
    	    	
    	return this.each(function(){
    		
    		if ( options ) { 
        		$.extend( settings, callMeOptions );
      		}
      		
      		var phone = buildPhone( settings );
      		
      		$(this).append(phone);
      		
      		// event handlers
      		phone.find(".phono-digit").bind({
      			mouseenter: function(){
      				$(this).addClass("ui-state-hover");
      			},
      			mouseleave: function(){
      				$(this).removeClass("ui-state-hover");
      			},
      			mousedown: function(){
      				$(this).addClass("ui-state-active");
      			},
      			mouseup: function(){
      				$(this).removeClass("ui-state-active");
      			},
      			click: function(e){
      				if( call )
                        call.sendDigits($(this).attr("title"));
      				e.preventDefault();
      			}
      		});
      		
      		dispatcher.bind({
      			// twilio client is ready, bind events to the call button
      			twilioReady: function(){
      				var btn = phone.find("a.phono-phone-button");
      				btn.bind({
      					click: function(e){
                          phoneBtn = phone.find(".phono-phone-button");
    		              if(phoneBtn.text() == "Hang Up") {
                            hangUpCall(settings, phone)
                          }
                          else {
                            makeCall(settings, phone);
                          }
      						e.preventDefault();
      					}
      				});
      				
      				btn.text(settings.buttontextready);
      			}
      		});
            
          
            Twilio.Device.setup(settings.token);

            Twilio.Device.ready(function (device) {
              dispatcher.trigger("twilioReady");
            });
      
            Twilio.Device.error(function (error) {
              phoneBtn = phone.find(".phono-phone-button");
              phoneBtn.text("Error: " + error.message);
            });
            
            
    	});
    	
    	function buildPhone( settings ){
    		var phoneHldr = $( "<div/>")
    			.addClass("phono-hldr ui-widget ui-corner-all");
    			
    		var phoneContent = $("<div/>")
    			.addClass("ui-widget-content ui-corner-all")
    			.css("padding","2px")
    			.appendTo(phoneHldr);
    			
    		var statusLink = $("<a>")
    			.attr("href","#")
    			.addClass("phono-phone-button ui-widget-header ui-corner-all")
    			.css({
    				"text-align":"center",
    				"display":"block",
    				"padding":"8px 10px",
    				"text-decoration":"none"
    			})
    			.html(settings.buttontext)
    			.appendTo(phoneContent);  
    		
    		if( settings.dialpad ){
    			var digitTblHldr = $("<div/>")
    				.addClass("phono-digit-hldr")
    				.appendTo(phoneContent);
    				
    			if(settings.slideopen)
    				digitTblHldr.css("display","none");
    			
    			var digitTbl = $( "<table/>" )
    				.addClass( "phono-digits-tbl" )
    				.css({
    					"width": "100%"
    				})
    				.appendTo( digitTblHldr );
    				
    			var digits = [
    				{key:"1", value:"&nbsp;"},
    				{key:"2", value:"ABC"},
    				{key:"3", value:"DEF"},
    				{key:"4", value:"GHI"},
    				{key:"5", value:"JKL"},
    				{key:"6", value:"MNO"},
    				{key:"7", value:"PQRS"},
    				{key:"8", value:"TUV"},
    				{key:"9", value:"WXYZ"},
    				{key:"*", value:"&nbsp;"},
    				{key:"0", value:"+"},
    				{key:"#", value:"&nbsp;"}
    			];
    			
    			var tblRows = $("<tr><td></td><td></td><td></td></tr><tr><td></td><td></td><td></td></tr><tr><td></td><td></td><td></td></tr><tr><td></td><td></td><td></td></tr>")
    				.appendTo(digitTbl);
    			
    			$.each(tblRows.find("td"), function( i, el ) { 
    				var digitObj = digits[i];
    				$(el).css({
    						"padding":"1px",
    						"vertical-align":"top",
    						"width":"33%"
    					});
    				
    				var digit = $( "<a/> ")
    				    .addClass( "phono-digit ui-state-default ui-corner-all" )
    				    .attr("title", digitObj.key)
    				    .attr("href","#")
    				    .css({
    				    	"padding":"3px",
    				    	"display":"block",
    				    	"text-align":"center",
    				    	"text-decoration":"none",
    				    	"font-size":"90%"
    				    })
    				    .html( digitObj.key )
    				    .appendTo( el );
    				    
    				var digitText = $("<span>")
    					.css({
    						"display":"block",
    						"font-size":"60%"
    					})
    					.html(digitObj.value)
    					.appendTo(digit);
    			});	
    		}
    		return phoneHldr;
    	}
    	
    	function makeCall(settings, phone){
    		phoneBtn = phone.find(".phono-phone-button");
    		phoneBtn.text("Hang Up");
    		
    		if( settings.dialpad && settings.slideopen )
    			phone.find(".phono-digit-hldr").slideDown();
    		params =  { "PhoneNumber" : settings.numbertodial};
            call = Twilio.Device.connect(params);	
    	}
    	
    	function hangUpCall(settings, phone){
       
    		Twilio.Device.disconnectAll();
    		phone.find(".phono-phone-button").text(settings.buttontextready);
    		
    		if( settings.dialpad && settings.slideopen )
    			phone.find(".phono-digit-hldr").slideUp();
    	}
	};
})( jQuery );