<?php

// $Id$

/**
 * @file
 * Implementation of the VoipServer-related classes
 */


/**
 * Class definitions
 */
        
abstract class VoipServer {

  private $id;  // internal server id
  private $name; // human-readable name of the server
  private $config_form; // (optional) name of form function for server configuration options
  private $config = NULL; // current configuration

  /*
   * Constructors
   */

  function __construct($id, $name, $config_form=NULL) {
    $this->id = $id;
    $this->name = $name;
    $this->config_form = $config_form;
  }

  public static function getServer($server_id) {
    $rc = NULL;
    $server_array = self::getServers();
    foreach($server_array as $server) {
      if($server->id == $server_id) {
        $rc = $server;
        break;
      }
    }
    return $rc;
  }

  public static function getDefaultServer() {
    $default_server_id = variable_get('voip_default_server', 'log');
    $default_server = self::getServer($default_server_id);
    return($default_server);
  }

  /*
   * Public methods
   */

  function getId() {
    return $this->id;
  }

  function getName() {
    return $this->name;
  }


  function getDefaultVoice(){
    $voice_id = variable_get('voipserver_'.$this->id.'_voice_id', '');
    if (!$voice_id) {
      // set $voice_id with the first voice in the list of voices provided by this server
      $voices = array_values(VoipVoice::getVoices(NULL, NULL, NULL, $this->id));
      $voice_id = $voices[0]->getVoiceId();
      $this->setDefaultVoice($voice_id);
    }
    $voipvoice_voice = VoipVoice::getVoice($voice_id);
    return $voipvoice_voice;
  }
  
  function setDefaultVoice($voice_id) {
    variable_set('voipserver_'.$this->id.'_voice_id', $voice_id);
  }
  
/**
   * This method defines which voice is to be used whenever the id is 'man'
   *
   * @param $voice_id
   *   String representing voice id
   *
   * @return
   *   VoipVoice instance associated with the 'man' id
   */
  function setDefaultManVoice($voice_id = NULL) {
    variable_set('voipserver_'.$this->id.'_man_voice_id', $voice_id);
  }

  /**
   * Defines which voice is to be used whenever the id is 'woman'
   *
   * @param $voice_id
   *   String representing voice id
   *
   * @return
   *   VoipVoice instance associated with the 'woman' id
   */
  function setDefaultWomanVoice($voice_id = NULL) {
    variable_set('voipserver_'.$this->id.'_woman_voice_id', $voice_id);
  }
    
  /**
   * Returns the VoipVoice instance associated with the 'man' id
   */
  function getDefaultManVoice() {
    $voice_id = variable_get('voipserver_'.$this->id.'_man_voice_id', '');
    if (!$voice_id) {
      // set $voice_id with the first voice in the list of voices provided by this server
      $voices = array_values(VoipVoice::getVoices(NULL, "man", NULL, $this->id));
      $voice_id = $voices[0]->getVoiceId();
      $this->setDefaultManVoice($voice_id);
    }
    $voipvoice_voice = VoipVoice::getVoice($voice_id);
    return $voipvoice_voice;
  }
    
  /**
   * Returns the VoipVoice instance associated with the 'woman' id
   */
   function getDefaultWomanVoice() {
    $voice_id = variable_get('voipserver_'.$this->id.'_woman_voice_id', '');
    if (!$voice_id) {
      // set $voice_id with the first voice in the list of voices provided by this server
      $voices = array_values(VoipVoice::getVoices(NULL, "woman", NULL, $this->id));
      $voice_id = $voices[0]->getVoiceId();
      $this->setDefaultWomanVoice($voice_id);
    }
    $voipvoice_voice = VoipVoice::getVoice($voice_id);
    return $voipvoice_voice;
  }

  function getConfigForm() {
    return $this->config_form;
  }

  function getConfig() {
    return $this->config;
  }

  public static function getServers() {
    $server_array = module_invoke_all('voipserver_info');
    foreach ($server_array as $server) {
      // retrieve server-specific configuration
      $server->config = variable_get('voip_'. $server->id .'_settings', '');
    }
    return $server_array;
  }


  /*
   * API methods
   */

  /**
   * Make a phone call
   * 
   * @param 
   * $voipcall, VoipCall instance with information about the call
   * 
   * @param 
   * $server_config, configuration of the voip server to be used for the call
   * 
   * @return
   * boolean, TRUE if success, FALSE in case of processing error
   */
  abstract function dial($voipcall, $server_config=NULL);
  
  /**
   * Send a text message
   * 
   * @param 
   * $text, body of the text message being sent
   *
   * @param 
   * $voipcall, VoipCall instance with the origin, destination, the script, and
   * addditional information about the way the message should be sent
   * 
   * @param 
   * $reply, whether the messages is to be sent as a reply to an already
   * existent VoipCall instance
   * 
   * @param 
   * $server_config, configuration of the VoIP server to be used
   * 
   * @return
   * boolean, TRUE if success, FALSE in case of processing error
   */
  abstract function send_text($text, $voipcall, $reply=FALSE, $server_config=NULL);

  /**
   * Hangup an existing call
   * 
   * @param 
   * $voipcall, VoipCall instance with information about the call to be hangup
   * 
   * @param 
   * $server_config, voip server to be used
   * 
   * @return
   * boolean, TRUE if success, FALSE in case of processing error
   */
  abstract function hangup($voipcall, $server_config=NULL);
  
  /**
   * Common entry point function for text message handling.
   * 
   * @param 
   * $text_content, with the text body
   * 
   * @param 
   * $origin, text origin
   *
   * @param 
   * $destination, text destination
   *
   * @param 
   * $network, text network
   *
   * @param 
   * $server_name, server name
   * 
   * @return
   * boolean, TRUE if success, FALSE in case of processing error
   */
  abstract function text_handler($text_content, $origin ,$destination, $time, $network, $server_name);


  /**
   * Test the connection with the voip server
   * 
   * @param 
   * $server_config, configuration of the voip server to be tested
   * 
   * @return
   * boolean, TRUE if success, FALSE in case of failure
   */
  abstract function ping($server_config=NULL);

}
