<?php
// $Id$
/**
 * @file
 * Contains the click2call_user views field handler.
 */

/**
 * Provides a configurable link to the new message form for a specific user.
 */
class views_handler_field_click2call_user extends views_handler_field {

  /**
   * Add uid as a additional field.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['uid'] = 'uid';
  }

  /**
   * Define our additional configuration setting.
   */
  function option_definition() {
    $options = parent::option_definition();
    //$options['text'] = array('default' => '', 'translatable' => TRUE);
   // $options['subject'] = array('default' => '', 'translatable' => TRUE);
    //$options['return'] = array('default' => TRUE, 'translatable' => FALSE);
    return $options;
  }

  /**
   * Define the configuration form for our textfield.
   */
  function options_form(&$form, &$form_state) {
     $form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => isset($this->options['label']) ? $this->options['label'] : '',
      '#description' => t('The label for this field that will be displayed to end users if the style requires it.'),
    );
  }

  /**
   * Renders our field, displays a link if the user is allowed to.
   */
  function render($values) {
    if (isset($values->uid)) {
      $uid = $values->uid;
    }
    elseif (isset($values->users_uid)) {
      $uid = $values->users_uid;
    }
    else {
      return '';
    }
    
    $account=user_load($uid);
    if (!user_access('include click2call in user profile', $account)) {
      return '';
    }
    $data = _click2call_user_user_view($account);
    return $data;
  }

  /**
   * Only display the column for users with the appropriate permission.
   */
  function access() {
    return user_access('access click2call in user profiles');
  }

  /**
   * Just do some basic checks, don't add "click2call_user" as field.
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }
}
