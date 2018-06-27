<?php
// $Id$
/**
 * @file
 * Provide views data and handlers for click2call_user.module
 */

function click2call_user_views_data() {
  $data['users']['click2call_user'] = array(
    'group' => t('Click to Call'),
    'title' => t('Click to Call User'),
    'field' => array(
      'title' => t('Click to Call User'),
      'help' => t('Displays Click to Call button.'),
      'handler' => 'views_handler_field_click2call_user',
    ),
  );
  return $data;
}


/**
 * Implementation of hook_views_handlers().
 */
function click2call_user_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'click2call_user') . '/views',
    ),
    'handlers' => array(
      // field handlers
      'views_handler_field_click2call_user' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}



