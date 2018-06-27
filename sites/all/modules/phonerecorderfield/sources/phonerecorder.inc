<?php
// $Id: phonerecorder.inc,v 1.2 2010/08/09 15:06:50 quicksketch Exp $

/**
 * @file
 * A FileField extension to allow uploading audio files by recording them through Voip call.
 */

/**
 * Implementation of hook_filefield_source_info().
 */
function filefield_source_phonerecorder_info() {
  $source['phonerecorder'] = array(
    'name' => t('Phone Recorder'),
    'label' => t('Phone Recorder'),
    'description' => t('Record audio by having system call you.'),
    'process' => 'filefield_source_phonerecorder_process',
    'weight' => 3,
  );
  return $source;
}

/**
 * Implementation of hook_theme().
 */
function filefield_source_phonerecorder_theme() {
  return array(
    'filefield_source_phonerecorder_element' => array(
      'arguments' => array('element' => NULL),
      'file' => 'sources/phonerecorder.inc',
    ),
  );
}

/**
 * Implementation of hook_filefield_source_settings().
 * This hook is not invoked due to a bug in filefield_sources: http://drupal.org/node/939386
 */
function filefield_source_phonerecorder_settings($op, $field) {
  $return = array();
  if ($op == 'form') {
    $return['source_phonerecorder'] = array(
      '#type' => 'fieldset',
      '#title' => t('Phone Recorder Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('File attach allows for selecting a file from a directory on the server, commonly used in combination with FTP.') . ' <strong>' . t('This file source will ignore file size checking when used.') . '</strong>',
      '#element_validate' => array('_filefield_source_attach_file_path_validate'),
      '#weight' => 3,
    );
    $return=array_merge($return['source_phonerecorder'], _phonerecorderfield_settings_form($form_state, $field));
  }
  if ($op == 'save') {
    $return = array('phonerecorderfield_number', 'phonerecorderfield_script', 'phonerecorderfield_text');
  }

  return $return;
}

 /**
 * Implementation of hook_form_alter().
 * Temporary solution until hook_filefield_source_settings() is fixed.
 */
function phonerecorderfield_form_alter(&$form, $form_state, $form_id) {
    if ($form_id=='content_field_edit_form') {
        if (isset($form['widget']['filefield_sources'])) {
            $form['widget']['filefield_sources']['source_phonerecorder'] = array(
              '#type' => 'fieldset',
              '#title' => t('Phone Recorder Settings'),
              '#collapsible' => TRUE,
              '#collapsed' => TRUE,
              '#weight' => 3,
            );
            
            $field_settings=content_fields($form['#field']['field_name'], $form['#field']['type_name']);
            $form['widget']['filefield_sources']['source_phonerecorder'] = array_merge($form['widget']['filefield_sources']['source_phonerecorder'], 
                                                                                       _phonerecorderfield_settings_form($form_state, $field_settings['widget']));
        }
    }
}

/**
 * Implementation of hook_widget_settings_alter().
 * Temporary solution until hook_filefield_source_settings() is fixed.
 */
function phonerecorderfield_widget_settings_alter(&$settings, $op, $widget) {
  // Only support modules that implement hook_insert_widgets().
  $widget_type = isset($widget['widget_type']) ? $widget['widget_type'] : $widget['type'];
  if (!in_array($widget_type, module_invoke_all('filefield_sources_widgets'))) {
    return;
  }
  if ($op == 'save') {
    $settings = array_merge($settings, array('phonerecorderfield_number', 'phonerecorderfield_script', 'phonerecorderfield_text'));
  }
}

/**
 * A #process callback to extend the filefield_widget element type.
 */
function filefield_source_phonerecorder_process($element, $edit, &$form_state, $form) {
  $field = content_fields($element['#field_name'], $element['#type_name']);

  $element['filefield_phonerecorder'] = array(
    '#theme' => 'filefield_source_phonerecorder_element',
    '#weight' => 100.5,
    '#access' => empty($element['fid']['#value']),
  );
  $description = t('Record audio by having system call you.');

  $element['filefield_phonerecorder']['#description'] = $description;
  $element['filefield_phonerecorder']['phonerecorderbutton'] = array(
        '#value' => phonerecorderfield_button($element, $field['widget'], $form['#node'])
      );

  return $element;
}

/**
 * Theme the output of the autocomplete field.
 */
function theme_filefield_source_phonerecorder_element($element) {
  $output = $element['phonerecorder_message']['#value'];
  $output .= $element['phonerecorderbutton']['#value'];
  $element['#type'] = 'item';
  return '<div class="filefield-source filefield-source-phonerecorder clear-block">' . theme('form_element', $element, $output) . '</div>';
}