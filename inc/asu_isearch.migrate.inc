<?php
/**
 * migrate module implementations
 */


/**
 * Implements hook_migrate_api().
 */
function asu_isearch_migrate_api() {

  $disable_hooks = array(
    'entity_insert' => array(
      'metatag',
      'og',
      'registration',
    ),
    'entity_update' => array(
      'metatag',
      'og',
      'registration',
    ),
    'node_insert' => array(
      'commons_activity_streams',
      'commons_follow_group',
      'commons_follow_node',
      'commons_groups',
      'commons_notify',
      'commons_radioactivity_groups',
      'kwresearch',
      'xmlsitemap_node',
      'expire',
    ),
    'node_update' => array(
      'commons_activity_streams',
      'commons_follow_group',
      'commons_follow_node',
      'commons_groups',
      'commons_radioactivity_groups',
      'kwresearch',
      'xmlsitemap_node',
      'expire',
    ),
    'node_prepare' => array(
      'og_menu',
    ),
    'taxonomy_term_insert' => array(
      'metatag',
      'shs',
      'taxonomy_menu',
    ),
    'taxonomy_term_update' => array(
      'shs',
      'taxonomy_menu',
    ),
  );

  return array(
    'api' => 2,
    'groups' => array(
      'asu_isearch' => array(
        'title' => t('iSearch Migration'),
        'disable_hooks' => $disable_hooks,
      ),
    ),
    'migrations' => array(
      'MigrateIsearchProfile' => array(
        'class_name' => 'MigrateIsearchProfileMigration',
        'group_name' => 'asu_isearch',
      ),
    ),
  );
}

/**
 * Instantiates the migration object
 */
function _asu_isearch_get_migration() {
  try {
    $migration = Migration::getInstance('MigrateIsearchProfile');
  }
  catch (MigrateException $e) {
    MigrationBase::registerMigration('MigrateIsearchProfileMigration', 'MigrateIsearchProfile', array('group_name' => 'asu_isearch', 'default_uid' => 1));
    $migration = Migration::getInstance('MigrateIsearchProfile');
  }

  return $migration;
}


/**
 * Begins the iSearch profile migration
 */
function _asu_isearch_begin_migrate($force_update = FALSE) {
  $migrate_config = variable_get('isearch_admin_feed_dept', null);

  if (is_null($migrate_config)) {
    return FALSE;
  }

  $migrate_config = json_decode($migrate_config);

  $migration = _asu_isearch_get_migration();

  if ($migration->getStatus() != MigrationBase::STATUS_IDLE) {

    $last_import = variable_get('isearch_last_import', null);
    if (!is_null($last_import)) {
      $today = new DateTime();
      $last_import = new DateTime('@'.$last_import);

      // calculate the difference
      $import_diff = $last_import->diff($today);

      // if it has been more than 2 hours, reset migration
      // to allow import
      if ($import_diff->h > 1) {
        $migration->resetStatus();
      } else {
        // too soon, warn the user to wait a few minutes before trying again
        $message = 'An import is currently running. Please wait a few minutes for it to finish before importing again.';

        drupal_set_message($message, 'warning');
        return FALSE;
      }
    }
  }

<<<<<<< HEAD
  // feed endpoint
  $endpoint = ASU_ISEARCH_DIRECTORY_FEED_ENDPOINT;
  $source_urls = array();

  $import_subdepts = variable_get('isearch_import_subdepts', TRUE);
  if ($import_subdepts) {
    foreach ($migrate_config->tree_nids as $department_id) {
      $source_urls[] = $endpoint . $department_id;
    }
  }
  else {
    $source_urls[] = $endpoint . $migrate_config->dept_nid;
  }

  $migration->setMigrateSource($source_urls);

  $update = $migration->prepareUpdate();
  $result = $migration->processImport(array('force' => $force_update));
=======
  $source_urls = asu_isearch_get_migrate_source_urls();
  
  // determine how many items we attempt to pull at a time
  $limit = variable_get('isearch_import_limit', null);
  if (is_null($limit) || is_numeric($limit)) {
    $value = 50;

    if (is_numeric($limit)) {
      $value = $limit;
    }

    $limit = array(
      'value' => $value,
      'unit' => 'items',
    );
  }

  // instantiate the drupal queue for fetching department profiles
  $queue = DrupalQueue::get('isearch_fetch_department');

  foreach ($source_urls as $url) {
    $data = array(
      'endpoint' => $url,
      'force' => $force_update,
      'limit' => $limit,
      'queue' => TRUE,
    );

    $queue->createItem($data);
  }
>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d

  variable_set('isearch_last_import', time());
}


function _asu_isearch_begin_batch($force_update = FALSE) {
  $limit = variable_get('isearch_import_limit', null);

  if (is_null($limit) || is_numeric($limit)) {

    $value = 50;

    if (is_numeric($limit)) {
      $value = $limit;
    }

    $limit = array(
      'value' => $value,
      'unit' => 'items',
    );
  }

  $migration = _asu_isearch_get_migration();
  $migration->prepareUpdate();

  $operations[] = array('migrate_ui_batch', array('import', 'MigrateIsearchProfile', $limit, $force_update));

  $batch = array(
    'operations' => $operations,
    'title' => t('iSearch Profile processing'),
    'file' => drupal_get_path('module', 'migrate_ui') . '/migrate_ui.pages.inc',
    'init_message' => t('Starting iSearch profile migration process'),
    'progress_message' => t(''),
    'error_message' => t('An error occurred. Some or all of the migrate processing has failed.'),
    'finished' => 'migrate_ui_batch_finish',
  );
  batch_set($batch);
}
