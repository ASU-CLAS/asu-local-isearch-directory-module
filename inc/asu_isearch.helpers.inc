<?php

/**
 * Helper function to get the header column widths
 * used on the iSearch directory list view.
 */
function asu_isearch_get_view_column_widths(stdClass $display_options) {
  $columns = array(
    'photo'     => 2,
    'name'      => 4,
    'contact'   => 3,
    'expertise' => 3,
  );

  if (!$display_options->display_photo) {
    $columns['name'] += $columns['photo'];
    $columns['photo'] = 0;
  }

  if (!$display_options->display_expertise) {
    $columns['name'] += ($columns['expertise'] - 1);
    $columns['contact'] += 1;
    $columns['expertise'] = 0;
  }

  if (!$display_options->display_name) {
    $columns['contact'] += $columns['name'];
    $columns['name'] = 0;
  }

  if (!$display_options->display_contact) {
    $columns['name'] += ($columns['contact'] - 1);
    $columns['expertise'] += 1;
  }

  return $columns;
}

/**
 * Helper function to get the default view configuration
 * used on the iSearch directory list view.
 *
 * This will help avoid undefined warnings when new options added
 */
function asu_isearch_get_default_view_config() {
  $depts = new stdClass();
  $depts->items = array();
  $depts->options = new stdClass();

  return array(
    'employee_types'        => array('All'),
    'employee_type_exclude' => 0,
    'tenure_display'        => 0,
    'show_managers'         => 0,
    'show_filters'          => 1,
    'include_subdepts'      => 0,
    'view_display'          => 'isearch_directory:isearch_directory_list',
    'pager_display'         => 'paged',
    'pager_items_per_page'  => 10,
    'pager_offset'          => 0,
    'expertise_areas'       => array(),
    'local_tags'            => array(),
    'display_short_bio'     => TRUE,
    'display_header'        => TRUE,
    'display_photo'         => TRUE,
    'display_name'          => TRUE,
    'display_contact'       => TRUE,
    'display_expertise'     => TRUE,
    'display_building'      => TRUE,
    'display_room'          => TRUE,
    'default_sort_by'       => 'field_isearch_affil_rank_weight_value',
    'filter_title'          => '',
    'show_filter_titles'    => TRUE,
    'show_filter_omni'      => TRUE,
    'show_filter_expertise' => TRUE,
    'show_filter_sort'      => TRUE,
    'show_az_index'         => FALSE,
    'link_building_codes'   => variable_get('isearch_link_buildings', FALSE),
    'depts'                 => $depts,
  );
}


/**
 * Helper function to determine if the passed view is an isearch configured view
 *
 * This will help clarify code and add checks to keep undefines from happening
 */
function asu_isearch_is_isearch_view($view) {
  return $view->current_display == 'isearch_a_z' || (isset($view->display_handler->view->display['default']->display_options['css_class']) && $view->display_handler->view->display['default']->display_options['css_class'] == 'isearch-directory-list');
}


/**
 * Helper function to check for valid building codes
 */
function asu_isearch_fetch_asu_building_code($code) {
  $building_code = check_plain(trim($code));

  if (in_array(strtoupper($building_code), array('FULTON CENTER', 'FULTON'))) {
    $building_code = 'FULTN';
  }

  if ($has_building_code = preg_match('/^[0-9A-Z]{2,5}$/', strtoupper($building_code))) {

    return $building_code;
  }

  return FALSE;
}


/**
 * Helper function to load react library locally if module isn't present
 */
function asu_isearch_load_js() {
  if (!module_exists('react')) {
    drupal_add_js(drupal_get_path('module', 'asu_isearch') . '/vendor/react/build/react.js');
    drupal_add_js(drupal_get_path('module', 'asu_isearch') . '/vendor/react/build/react-dom.js');
  }
}

/**
 * Helper function for backwards compatibility when retrieving tenure display setting
 */
function asu_isearch_get_tenure_config($setting) {
  if (!is_object($setting)) {
    return $setting;
  }

  $tenure_display = 'Both';

  $tenure_display_values = array();
  foreach ($setting as $key => $value) {
    if ($value == 1) {
      $tenure_display_values[] = $key;
    }
  }

  if (count($tenure_display_values) == 1) {
    $tenure_display = $tenure_display_values[0];
  }

  return $tenure_display;
}


/**
 * Returns the 
 */
function asu_isearch_get_dept_config() {
  $default_feed_dept = array(
    'items' => array(),
    'options' => new stdClass(),
  );

  return variable_get('isearch_admin_feed_dept', json_encode($default_feed_dept));
}

/**
 * Retrieves an array of source urls
 */
function asu_isearch_get_migrate_source_urls($from_cache = FALSE) {

  $source_urls = array();

  // Check and see if we have configured our migration
  $migrate_config = variable_get('isearch_admin_feed_dept', null);

  if (!is_null($migrate_config)) {
    $migrate_config = json_decode($migrate_config);

    foreach ($migrate_config->items as $migrate_item) {
      // determine which endpoint to use
      $dept_id = $migrate_item->dept_id;

      if ($migrate_config->options->$dept_id->subdepts) {
        foreach ($migrate_item->tree_nids as $department_nid) {

          if ($from_cache && file_exists(ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT . $department_nid)) {
            $endpoint = ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT;
          }
          else {
            $endpoint = ASU_ISEARCH_DIRECTORY_FEED_ENDPOINT;
          }

          $source_urls[$department_nid] = $endpoint . $department_nid;
        }
      }
      else {
        // determine which endpoint to use
        if ($from_cache && file_exists(ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT . $migrate_item->dept_nid)) {
          $endpoint = ASU_ISEARCH_DIRECTORY_FEED_CACHE_ENDPOINT;
        }
        else {
          $endpoint = ASU_ISEARCH_DIRECTORY_FEED_ENDPOINT;
        }

        $source_urls[$migrate_item->dept_nid] = $endpoint . $migrate_item->dept_nid;
      }
    }
  }

  return $source_urls;
}


/**
 * Helper function to test for devmode
 */
function asu_isearch_devmode() {
  return (bool) variable_get('asu_isearch_devmode', FALSE);
}

/**
 * Returns the node (and all children) where (node.tid == @param tid)
 * @param {Object} data
 *  Nested JSON object with department data
 * @param {integer} dept_id
 *  ID of the department that should be the root of the hierarchy
 */
function asu_isearch_dept_find_root($data, $dept_id, $lookup = 'dept_nid') {
  $success = array();

  for ($i = 0; $i < count($data); $i++) {
    if (count($success) == 0 && $data[$i] != NULL) {
      if ($data[$i]->$lookup == $dept_id) {
        return $data[$i];
      }
      else if (isset($data[$i]->children)) {
        $success = asu_isearch_dept_find_root($data[$i]->children, $dept_id, $lookup);
      }
    }
    else {
      break;
    }
  }
  return $success;
}

/**
 * Saves the ids of all departments under currently selected tree on #people's data object
 * @param {object}
 *  Nested JSON object with department data
 */
function asu_isearch_dept_get_tree_data($tree, $tree_ids = array()) {

  $newItem = new stdClass();
  $newItem->name = $tree->name;
  $newItem->dept_nid = $tree->dept_nid;

  $tree_ids[] = $newItem;

  for ($i = 0; $i < count($tree->children); $i++) {
    $tree_ids = asu_isearch_dept_get_tree_data($tree->children[$i], $tree_ids);
  }

  return $tree_ids;
}


