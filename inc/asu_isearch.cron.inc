<?php
/**
 * cron implementations
 */


/**
 * Implements hook_cron().
 */
function asu_isearch_cron() {

  // Cache dept tree and employee types data
  _asu_isearch_cache_dept_data();

  $last_import = variable_get('isearch_last_import', null);
  _asu_isearch_begin_migrate();
}


/**
 * Implements hook_cron_queue_info().
 */
function asu_isearch_cron_queue_info() {
  $queues['isearch_fetch_profile_photo'] = array(
    'worker callback' => 'asu_isearch_fetch_profile_photo',
    'time' => 120,
  );

  return $queues;
}


/**
 * Cron queue worker callback for fetching profile photos
 */
function asu_isearch_fetch_profile_photo($data) {
  $photo_url = $data['photo_url'];
  $destination = $data['destination'];
  $entity = current(entity_load('node', array($data['entity'])));

  // ensure the file is truly accessible
  $response = drupal_http_request($photo_url);
  if ($response->code == 200) {

    if (!file_exists($destination) || filesize($destination) != $response->headers['content-length']) {
      $file = system_retrieve_file($photo_url, $destination, TRUE, $replace = FILE_EXISTS_REPLACE);

      // Load the node so we can set
      $entity->field_isearch_photo_url[LANGUAGE_NONE][0]['fid'] = $file->fid;
      node_save($entity);
    }
  }
}


/**
 * Caches the isearch department tree and employee types
 */
function _asu_isearch_cache_dept_data() {
  //set timeout of request to 60 seconds
  $options = array(
    'timeout' => 60,
  );

  //Refresh and cache the Tree
  $tree = NULL;

  $tree_endpoint = ASU_ISEARCH_DIRECTORY_TREE_ENDPOINT;
  $request = drupal_http_request($tree_endpoint, $options);

  if ($request->code == 200) {
    $tree = $request->data;
  }
  // Create cache, set to hold cache for number of hours specified
  if ($tree != NULL) {
    $expire_date = time() + 3600 * ASU_ISEARCH_DIRECTORY_FORM_CACHE_DURATION;
    cache_set('asu_directory_tree', $tree, 'cache', $expire_date);
  }

  //Refresh and cache the employee types
  $employee_types = NULL;

  $endpoint = ASU_ISEARCH_EMPLOYEE_TYPES_ENDPOINT;
  $request = drupal_http_request($endpoint, $options);

  if ($request->code == 200) {
    $types = $request->data;
    $employee_types = drupal_json_decode($types);
  }
  // Create cache, set to hold cache for number of hours specified
  if ($employee_types != NULL) {

    $expire_date = time() + 3600 * ASU_ISEARCH_DIRECTORY_FORM_CACHE_DURATION;
    cache_set('asu_employee_types', $employee_types, 'cache', $expire_date);
  }
}
