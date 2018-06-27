<?php
// $Id:

/**
 * @file
 * phonerecorderfield widget hooks and callbacks.
 */

/**
 * Implementation of CCK's hook_widget_settings($op = 'form').
 * Add per field options
 */
function phonerecorderfield_widget_settings_form($widget) {
  $form = module_invoke('filefield', 'widget_settings', 'form', $widget);
  $form['file_extensions'] ['#default_value']= is_string($widget['file_extensions']) ? $widget['file_extensions'] :'mp3 wav';
  
  $form['phonerecorderfield'] = array(
    '#type' => 'fieldset',
    '#title' => t('Phone Recorder Settings'),
  );
  
  if (module_exists('voipnumber')) {
        //This options are available only if voipnumber is enabled
        $options['default']=t('Default number (from user profile)');
        $options['voipnumber']=t('Number returned by Voipnumber field');
    }
    else {
        $description = t('Enable !voipnumber module to get more options.', array('!voipnumber' => l('voipnumber', 'http://drupal.org/project/voipnumber')));
    }
    
    $options['type']=t('Allow user to type in the number');
    
  $form['number'] = array(
      '#type' => 'checkboxes',
      '#prefix' => $prefix,
      '#title' => t('Number to be called'),
      '#options' => $options,
      '#default_value' => $widget['number'],
      '#description' => $description,
  );
    
  /* Build list of voip scripts */
  $form['script'] = array(
      '#type' => 'select',
      '#title' => t('Default script to be used'),
      '#options' => VoipScript::getScriptNames(),
      '#default_value' => $widget['script'],
  );
  $form['recordingformat'] = array(
      '#type' => 'select',
      '#title' => t('Default audio format'),
      '#options' => array('mp3'=>'mp3', 'wav'=>'wav'),
      '#default_value' =>$widget['recordingformat'],
      '#description' => t('Default audio format to be used for the recordings.'),
  );
  $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Default text'),
      '#default_value' => $widget['text'],
      '#description' => t('Default text to be displayed on Phone Recorder Field.'),
  );
  
  //$form=array_merge($form['phonerecorderfield'], _phonerecorderfield_settings_form($widget));
  return $form;
}

/**
 * Implementation of CCK's hook_widget_settings($op = 'save').
 */
function phonerecorderfield_widget_settings_save($widget) {
  $filefield_settings = module_invoke('filefield', 'widget_settings', 'save', $widget);
  return array_merge($filefield_settings, array('number', 'script', 'phonerecorderfield_text', 'recordingformat','text'));
}

/**
 * Element #value_callback function.
 */
function phonerecorderfield_widget_value($element, $edit = FALSE) {
  $field = filefield_widget_value($element, $edit);
  
  return $field;
}

/**
 * FormAPI theme function. Theme the output.
 */
function theme_phonerecorderfield_widget($element) {
  
  $z=theme('form_element', $element, $element['#children']);
  
  return $z;
}

/**
 * Process an individual element.
 *
 * Build the form element. When creating a form using FAPI #process,
 * note that $element['#value'] is already set.
 *
 * The $fields array is in $form['#field_info'][$element['#field_name']].
 */
function phonerecorderfield_widget_process($element, $edit, $form_state, $form) {
    $field = $form['#field_info'][$element['#field_name']];
    $field_key = $element['#columns'][0];
    $fid=$element['#value'][$field_key];
    $delta=$element['#delta'];
    
    $element['phonerecorder'] = array(
        '#value' => phonerecorderfield_button($element, $fid, $form['#node']),
        '#title' => "Phone Recorder"
     );
     
    // this would be the hidden field the Phone Recorder field would insert the recorder call id 
    $element[$field_key] = array(
         '#title' => t($field['widget']['label']),
         '#type' => 'hidden',
         '#required' => $field['required'],
         '#default_value' => isset($fid) ? $fid : NULL,
    );
    
    $element['value'] = array(
         '#title' => t($field['widget']['label']),
         '#type' => 'hidden',
         '#default_value' => $element['value'],
    );
 
    return $element;
}

function phonerecorderfield_button($element, $fid, $node) {
    // The widget is being presented, so apply the JavaScript.
    drupal_add_js(drupal_get_path('module', 'phonerecorderfield') . '/js/phonerecorderfield.js');
    
    $delta=$element['#delta'];
    $field_name=str_replace('field_', '', $element['#field_name']);
    $field_name=str_replace('_', '-', $field_name);
    $number_selection=theme('phonerecorderfield_phone_numbers', $field_name, $delta, $node->type);
    
    if (!empty($element['#value']['filepath'])) {
        //field with value
        $button_op='Remove';
        //$call_me=phonerecorderfield_file_load_by_fid($fid);
        $phonerecorder_file=file_create_url($element['#value']['filepath']);
        $output .= '<div class="phonerecorderfield-hidden" id="phonerecorderfield-' . $field_name . '-' . $delta . '-hidden">' . $number_selection . '</div>';
        $output .= '<div id="phonerecorderfield-' . $field_name . '-' . $delta . '-message">' . audiofield_get_player($phonerecorder_file, 'mp3') . '</div>';
        //Hide hangup button
        $hangup_style="style='display:none;'";
    }
    else{
        //empty field
        $button_op='Call Me';
        $output .= '<div class="phonerecorderfield-hidden" id="phonerecorderfield-' . $field_name . '-' . $delta . '-hidden"></div>';
        $output .= '<div id="phonerecorderfield-' . $field_name . '-' . $delta . '-message">' . $number_selection . '</div>';
    }

    //Call/Remove button
    $output .= '<input id="call-me-button-' . $field_name . '-' . $delta . '"type="button" value="' . $button_op . '" onclick="phonerecorder(\'' . $field_name . '\',' . $delta . ',\'' . $node->type .'\')" />';
    
    //Hangup button
    $output .= '<input type="hidden" value="" id="edit-field-'.$field_name.'-'.$delta.'-value"><input class="hangup-button" ' . $hangup_style .' id="hangup-button-'. $field_name .'-'. $delta .'"type="button" value="'. t('Hangup') .'" onclick="phonerecorderfieldHangup(\''. $field_name .'\','. $delta .')" />';
    
    return $output;
}
