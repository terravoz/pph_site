<?php
/**
 * @file phonerecorderfield.script.inc
 *  Scripts provided to voipcall nodes by phonerecorderfield module. 
 */ 

/**
 * Implementation of hook_voipcall_load_script()
 */
function phonerecorderfield_voipscript_load_script($script_name, $vars = NULL) {

  $script = NULL;
  switch ($script_name) {
    case 'phonerecorderfield_default': 
      $script = new VoipScript('phonerecorderfield_default');

      $script->addLabel('start');
      $prompt = t("Please record your message after the beep.  When done, press the pound key.");
      $timeout = 5;
      $end_key = '#';
      $max_length = 20;
      $format = variable_get('phonerecorderfield_format', 'mp3');
      $script->addRecord($prompt, $timeout, $end_key, $max_length, $format);

      $script->addSay('You said %recording_public_url');
    
      $script->addLabel('accept menu');
      $text = "To accept this recording, press 1.  To record it once again, press 2. To hangup, press the pound key.";
      $timeout = 5;
      $end_key = '';
      $num_digits = 1;
      $script->addGetInput($text, $num_digits, $end_key, $timeout);

      $script->addGotoIf('accept recording', "^%input_digits == '1'");

      $script->addGotoIf('start', "^%input_digits == '2'");

      $script->addGotoIf('end call', "^%input_digits == '#'");

      $script->addSay('Invalid input received. Please try again.');
      $script->addGoto('accept menu');

      $script->addLabel('accept recording');
      $script->addSay('About to start processing the recorded file.');
      // Set variables from $vars to pass to callback function.
      $script->addSet('uid', $vars['uid']); 
      $script->addSet('name', $vars['name']); 
      $script->addSet('number', $vars['number']); 
      // Process recording.
      $script->addSet('callback_result',
        '^_phonerecorderfield_record_callback(%call_id, %recording_public_url, %recording_duration,%recording_fid, %uid, %name, %number)');

      $script->addSay('The server returned: %callback_result');
      $script->addGoto('end call');

      $script->addLabel('no input received');
      $script->addSay("No input received. Please try again.");
      $script->addGoto('start');

      $script->addLabel('end call');
      $script->addSay('Bye bye.');
      $script->addHangup();

    break;
  }

  return $script;
}

/**
 * Callback for the voip script phonerecorderfield_default.
 * 
 * @see hook_voipscript_load_script()
 */
function _phonerecorderfield_record_callback($call_id, $recording_url, $recording_duration, $recording_fid, $uid, $name, $number) {
  $call=new VoipCall();
  $call_nid=$call->getCidFromCallId($call_id);
  // Note: Do not replace $call_id and $recording_url with '%s' here, it will cause mysql errors.
  $sql = "INSERT INTO {phonerecorderfield_recording} (call_nid, url, uid, fid, name, number, duration) " 
          ."VALUES ('$call_nid', '$recording_url', %d, %d, '%s', '%s', %d) ";
  $args = array($uid,$recording_fid, $name, $number, $recording_duration);
  $res=db_query($sql, $args);

  //Log with watchdog
  $type = 'phonerecorderfield';
  $message = t("New message recorded. Call_id: $call_id, recording_url: $recording_url, recording_duration: $recording_duration, name: $name, number: $number");
  watchdog($type, $message);
  if ($res) {
    return 'Success.';
  }
  else {
    return 'Failure';
  }
  
}

