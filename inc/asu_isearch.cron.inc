<?php
/**
 * cron implementations
 */


/**
 * Implements hook_cron().
 */
function asu_isearch_cron() {

  // Cache data from configuration
  _asu_isearch_cache_dept_feed();

  // Cache dept tree and employee types data
  _asu_isearch_cache_dept_tree_data();

  // Begin profile migrations
<<<<<<< HEAD
  _asu_isearch_begin_migrate();
=======
  _asu_isearch_begin_migrate(FALSE);
>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
}


/**
 * Implements hook_cron_queue_info().
 */
function asu_isearch_cron_queue_info() {
  $queues['isearch_fetch_profile_photo'] = array(
    'worker callback' => 'asu_isearch_fetch_profile_photo',
    'time' => 120,
  );

<<<<<<< HEAD
=======
  $queues['isearch_fetch_department'] = array(
    'worker callback' => 'asu_isearch_fetch_departments',
    'time' => 120,
  );

>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
  return $queues;
}


/**
<<<<<<< HEAD
=======
 * Cron queue worker callback for fetching department profiles
 */
function asu_isearch_fetch_departments($data) {
  // feed endpoint
  $source_urls = array($data['endpoint']);

  $migration = _asu_isearch_get_migration();
  $migration->setMigrateSource($source_urls);

  if (isset($data['queue']) && $data['queue']) {

    $batch_limit = 50;
    if (isset($data['limit']['unit']) && $data['limit']['unit'] == 'items') {
      $batch_limit = intval($data['limit']['value']);
    }

    $source_count = $migration->sourceCount();
    $batch_size = ceil($source_count / $batch_limit);

    $queue = DrupalQueue::get('isearch_fetch_department');
    // avoid an inifinite loop by resetting queue to false
    unset($data['queue']);

    for ($i = 1; $i <= $batch_size; $i++) {
      $data['limit']['value'] = $i * $batch_limit;
      $queue->createItem($data);
    }
  }
  else {
    // begin the migration
    $update = $migration->prepareUpdate();
    $result = $migration->processImport($data);
  }
}



/**
>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
 * Cron queue worker callback for fetching profile photos
 */
function asu_isearch_fetch_profile_photo($data) {
  $photo_url = $data['photo_url'];
  $destination = $data['destination'];

  $entity = current(entity_load('node', array($data['entity'])));

  if (!$entity) {
    return;
  }

  // ensure the file is truly accessible
  $response = drupal_http_request($photo_url);
  if ($response->code == 200) {

<<<<<<< HEAD
    if (!file_exists($destination) || filesize($destination) != $response->headers['content-length']) {
      $file = system_retrieve_file($photo_url, $destination, TRUE, $replace = FILE_EXISTS_REPLACE);

=======
    $file = FALSE;
    $photo_exists = file_exists($destination);

    // does the file already exist on the server?
    // if yes, associate it

    if ($photo_exists) {
      $file = current(entity_load('file', array(), array('uri' => $destination.'')));
    }

    if (!$photo_exists || filesize($destination) != $response->headers['content-length']) {
      $file = system_retrieve_file($photo_url, $destination, TRUE, $replace = FILE_EXISTS_REPLACE);
    }
    
    if ($file) {
>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
      // Load the node so we can set
      $entity->field_isearch_photo_url[LANGUAGE_NONE][0]['fid'] = $file->fid;
      node_save($entity);
    }
  }
}


function _asu_isearch_cache_dept_feed() {

  if (!file_exists(ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT)) {
    mkdir(ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT, 0644, TRUE);
  }

  // set timeout of request to 60 seconds
  $options = array(
    'timeout' => 60,
  );

  // retrieve list of configured isearch dept urls for import
  $source_urls = asu_isearch_get_migrate_source_urls();

  foreach ($source_urls as $url) {
    $data = NULL;

    $request = drupal_http_request($url, $options);

    if ($request->code == 200) {
      $data = $request->data;
    }

    if (!is_null($data)) {
      $info = pathinfo($url);
      $dept_nid = $info['filename'];

      file_save_data($data, ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT . $dept_nid, FILE_EXISTS_REPLACE);
    }
  }
}


/**
 * Caches the isearch department tree and employee types
 */
function _asu_isearch_cache_dept_tree_data() {
  // set timeout of request to 60 seconds
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
