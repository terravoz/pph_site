<?php
/**@file
 */
 /* Load audio file from external source. */
function theme_phonerecorderfield_formatter_phonerecorderfield_external_url($element) {
  //Don't render player if there is no file.
  if (!$element['#item']['fid']) {
    return '';
  }
  $call_me=db_fetch_array(db_query("SELECT * FROM {phonerecorderfield_recording} WHERE fid=%d", $element['#item']['fid']));

  /* Render audio player */
  return audiofield_get_player($call_me['url'], 'mp3');
}