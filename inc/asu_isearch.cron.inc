<?php
/**
 * cron implementations
 */


/**
 * Implements hook_cron().
 */
function asu_isearch_cron() {

  // Cache data from configuration
  //CACHE THE FEED AND BUILD THE SOURCE
  _asu_isearch_cache_dept_feed(TRUE);

  // Cache dept tree and employee types data
  _asu_isearch_cache_dept_tree_data();

  // Begin profile migrations
  _asu_isearch_begin_migrate(FALSE);

  // Cleanup the profiles which are no longer associated with
  // unit depts in iSearch.  Only run once per day.
  $last_clean = variable_get('last_profile_cleanup', NULL);

  if ($last_clean != date('ymd', time())) {
    _asu_isearch_cleanup();
    variable_set('last_profile_cleanup', date('ymd', time()));
  }
}

/**
 * Implements hook_cron_queue_info().
 */
function asu_isearch_cron_queue_info() {
  $queues['isearch_fetch_profile_photo'] = array(
    'worker callback' => 'asu_isearch_fetch_profile_photo',
    'time' => 120,
  );

  $queues['isearch_fetch_department'] = array(
    'worker callback' => 'asu_isearch_fetch_departments',
    'time' => 120,
  );

  $queues['isearch_clean_profiles'] = array(
    'worker callback' => 'asu_isearch_cleanup_profile',
    'time' => 120,
  );

  return $queues;
}


/**
 * Cron queue worker callback for fetching department profiles
 */
function asu_isearch_fetch_departments($data) {
  // feed endpoint
  $source_urls = array($data['endpoint']);

  $migration = _asu_isearch_get_migration();


  $migration->setMigrateSource($source_urls);
  // begin the migration
  $update = $migration->prepareUpdate();
  $result = $migration->processImport($data);
}



/**
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
      // Load the node so we can set
      $entity->field_isearch_photo_url[LANGUAGE_NONE][0]['fid'] = $file->fid;
      node_save($entity);
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

/**
 * Iterate over all imported iSearch profiles,
 * cleaning up the profiles which are no longer associated with the dept
 * in iSearch
 *
 */
function _asu_isearch_cleanup() {
  $type = "asu_isearch_profile";
  $nodes = node_load_multiple(array(), array('type' => $type));
  $queue = DrupalQueue::get("isearch_clean_profiles");

  foreach($nodes as $key => $profile) {

    //we only cleanup profiles which have been imported from iSearch
    if ($profile->field_isearch_eid['und'][0]['value'] != -1) {
      $queue->createItem($profile);
    }
  }
}

/**
 * @param $profile
 *
 * Worker callback for cleaning up dept profiles
 */
function asu_isearch_cleanup_profile($profile) {

  $sources = asu_isearch_get_migrate_source_urls(TRUE);
  $asurite = $profile->field_isearch_asurite_id['und'][0]['value'];
  $pfound = FALSE;

  // load each affiliation, and check the profile against the listings from
  // isearch
  foreach($profile->field_isearch_affiliations['und'] as $item) {
    $affil = entity_load('field_collection_item', array($item['value']));

    foreach ($affil as $af) {
      $feed = array();
      $nid = $af->field_isearch_affil_dept_id['und'][0]['value'];

      $locache = ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT.$nid;
      $idir = ASU_ISEARCH_DIRECTORY_FEED_ENDPOINT.$nid;

      if (in_array($locache, $sources)) {

        $feed = drupal_json_decode(file_get_contents($locache));

      } else if (in_array($idir, $sources)) {

        $request = drupal_http_request($idir);

        if ($request->code == 200) {
          $feed = drupal_json_decode($request->data);

        // if there was a bad request, then set the $pfound var to true, so that
        // we don't delete an entire dept
        } else {
          $pfound = TRUE;
        }
      }

      //check the asurite value of the profile agains the asurites in the feed,
      // and set the $pfound flag to true if found
      if (!empty($feed)) {
        foreach($feed as $fitem) {
          if ($fitem['asuriteId'] == $asurite) {
            $pfound = TRUE;
            break;
          }
        }
      }
    }
  }

  //if the profile wasn't found, then unpublish
  if ($pfound == FALSE) {
    $title = $profile->title;
    watchdog('asu_isearch',"Profile'[@id]'unpublishedviacroncleanup",array('@id'=>$title));
    $profile->status = 0;
    node_save($profile);

  //if node was unpublished, but was found on iSearch, republish the node
  } else if ($pfound == TRUE && $profile->status == 0) {
    $profile->status = 1;
    $title = $profile->title;
    watchdog('asu_isearch',"Profile'[@id]'REpublishedviacroncleanup",array('@id'=>$title));
    node_save($profile);
  }
}