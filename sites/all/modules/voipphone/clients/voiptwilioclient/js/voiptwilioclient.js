$(document).ready(function(){
var token;
if (Drupal.settings.voiptwilioclient_token.constructor.toString().indexOf("Array") == -1) {
  token = Drupal.settings.voiptwilioclient_token;
}
else {
  token = Drupal.settings.voiptwilioclient_token[0];
}

Twilio.Device.setup(token);

      Twilio.Device.ready(function (device) {
        $(".voiptwilio-log").text("Ready");
      });

      Twilio.Device.error(function (error) {
        $(".voiptwilio-log").text("Error: " + error.message);
      });

      Twilio.Device.connect(function (conn) {
        $(".voiptwilio-log").text("Successfully established call");
      });

      Twilio.Device.disconnect(function (conn) {
        $(".voiptwilio-log").text("Call ended");
      });

     
});      

function voiptwilioclient_call(number) {
  params =  { "PhoneNumber" : number};
  Twilio.Device.connect(params);
}

function voiptwilioclient_hangup() {
  Twilio.Device.disconnectAll();
}