<?php
/**
 * @file
 * Theming functions for the asu_isearch module.
 *
 * @author Sebastian Nievas (snievas@asu.edu)
 *
 */

/**
 *Implements hook_theme()
 */
function asu_isearch_theme() {
  return array(
    // iSearch Directory Panel Pane
    'fieldable_panels_pane__isearch_directory' => array(
      'template' => 'theme/fieldable-panels-pane--isearch-directory',
      'render element' => 'elements',
      'file' => 'inc/asu_isearch.theme.inc',
      'type' => 'module'
    ),
    // ASU Spotlight container
    'asu_isearch_dept_picker' => array(
      'variables' => array('items' => array(), 'settings' => array(), 'label' => 'Select Department'),
      'template' => 'theme/asu_isearch.dept_picker',
      'file' => 'inc/asu_isearch.theme.inc',
    ),
    // iSearch Department Tree
    'asu_isearch_dept_tree' => array(
      'variables' => array(
        'item' => array(),
        'settings' => array(),
        'caption' => ''
      ),
      'file' => 'inc/asu_isearch.theme.inc',
    ),
    // iSearch Config Form Field
    'asu_isearch_dept_config' => array(
      'variables' => array('items' => array(), 'settings' => array()),
      'template' => 'theme/asu_isearch.dept_config',
      'file' => 'inc/asu_isearch.theme.inc',
    ),
  );
}

/**
 * Default theme implementation for iSearch department tree
 */
function theme_asu_isearch_dept_tree(&$vars) {
  $standalone    = FALSE;
  $whole_tree    = FALSE;
  $config_target = FALSE;

  if (isset($vars['settings']['standalone_tree'])) {
    $standalone = $vars['settings']['standalone_tree'];
  }

  if (isset($vars['settings']['whole_tree'])) {
    $whole_tree = $vars['settings']['whole_tree'];
  }

  if (isset($vars['settings']['config_target'])) {
    $config_target = $vars['settings']['config_target'];
  }

  $tree = asu_isearch_get_dept_tree();

  // jqTree (http://mbraak.github.com/jqTree)
  libraries_load('jqtree');
  libraries_load('fontawesome');

  // Custom
  drupal_add_css(drupal_get_path('module', 'asu_isearch') . '/css/asu_isearch.dept_tree.css');
  drupal_add_js(drupal_get_path('module', 'asu_isearch') . '/js/asu_isearch.dept_tree.js');

  $top_level_ids = array(
    "top_level_psid" => ASU_ISEARCH_DIRECTORY_TOP_LEVEL_PS_ID,
    "top_level_nid" => ASU_ISEARCH_DIRECTORY_TOP_LEVEL_DEPT_NID
  );

  //Pass our items and configs to the JS
  //will need to pass extra configs if the tree is being used without the people display (standalone)
  if ($standalone) {
    if (isset($vars['items'][0]['directory_items'])) {
      $nids = $vars['items'][0]['directory_items'];
    }
    else {
      $nids = array(ASU_ISEARCH_DIRECTORY_TOP_LEVEL_DEPT_NID);
    }

    $settings = array(
      'tree' => $tree,
      'top_level_ids' => $top_level_ids,
      'standalone'    => $standalone,
      'dept_nids'     => $nids,
    );
  }
  else {
    $settings = array(
      'tree' => $tree,
      'whole_tree' => $whole_tree,
      'top_level_ids' => $top_level_ids
    );
  }

  $settings['config_target'] = $config_target;

  drupal_add_js(array('asu_isearch_dept_tree' => $settings), 'setting');

  $html = '<div class="asu-directory-tree-hierarchy"><div id="treediv"></div></div>';

  return $html;

}

/**
 * Template preprocess handler for 'asu_isearch_form_field' template.
 */
function template_process_asu_isearch_dept_config(&$vars) {

  //this generates the employee type display, by first grabbing the types from the endpoint
  //and then grouping them into 3 columns

  $employee_types = asu_isearch_get_employee_types();
  $employee_types = $employee_types['nodes'];

  //add the "show all" option to the front of the array
  $show_all = array('node' => array('name' => 'All'));
  array_unshift($employee_types, $show_all);

  //calculate how many types we'll put in each column
  $num_per_col = count($employee_types) / 3 + 1;

  $count = 1;
  $col = 0;

  $vars['settings']['employee_types_col_' . $col] = array();

  foreach ($employee_types as $type) {

    if (($count % $num_per_col == 0) && ($count < count($employee_types))) {
      $col++;
      $vars['settings']['employee_types_col_' . $col] = array();
    }

    array_push($vars['settings']['employee_types_col_' . $col], $type);

    $count++;
  }

  drupal_add_js(drupal_get_path('module', 'asu_isearch') . '/js/asu_isearch.dept_config.js');
}

/**
 * Implements hook_libraries_info
 *
 * Adds the JQtree library
 * Documentation for the library can be found here http://mbraak.github.io/jqTree/
 *
 * @return mixed
 *
 */
function asu_isearch_libraries_info() {
  // Expected to be extracted into 'sites/all/libraries/jqtree'.
  $libraries['jqtree'] = array(
    'name' => 'jQTree Library',
    'vendor url' => 'http://mbraak.github.io/jqTree/',
    'download url' => 'https://github.com/mbraak/jqTree/archive/0.22.0.zip',
    'version callback' => 'asu_isearch_short_circuit_version',
    'files' => array(
      'js' => array('tree.jquery.js'),
      'css' => array('jqtree.css'),
    ),
  );

  return $libraries;
}


/**
 * Short-circuit the version argument.
 */
function asu_isearch_short_circuit_version() {
  return TRUE;
}
