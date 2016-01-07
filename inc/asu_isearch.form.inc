<?php

/**
 * Implements hook_form_alter()
 */
function asu_isearch_form_alter(&$form, &$form_state, $form_id) {
  // hide the profile tabs placeholder field that can be rendered on a node display
  if ($form_id == 'asu_isearch_profile_node_form') {
    $form['field_isearch_profile_tabs']['#access'] = FALSE;
    $form['field_isearch_hash']['#access'] = FALSE;
    $form['title'] = array(
      '#default_value' => 'Set on save...',
      '#disabled' => TRUE,
    );

    // these profile fields should not be editable at this time but...
    // provide a list of exceptional fields
    $field_exception_manifest = array(
      'field_isearch_local_tags',
      'field_isearch_local_photo_url',
      'field_isearch_affiliations',
      'field_isearch_affil_rank_weight',
    );

    // handle top level fields
    foreach ($form as $form_field => $value) {
      if (preg_match('/^field_isearch_/', $form_field) && !in_array($form_field, $field_exception_manifest)) {
        if (asu_isearch_devmode() === FALSE) {
          $form[$form_field]['#disabled'] = TRUE;
          $form[$form_field]['#access']   = FALSE;
        }
      }
    }

    // loop through any field collection fields
    $protected_field_collections = array(
      'field_isearch_affiliations',
    );

    foreach ($protected_field_collections as $field_collection) {

      if (asu_isearch_devmode() === FALSE) {
        $form[$field_collection][LANGUAGE_NONE]['add_more']['#disabled'] = TRUE;
      }

      foreach ($form[$field_collection][LANGUAGE_NONE] as $item_key => $item_value) {
        if (is_integer($item_key)) {
          foreach ($form[$field_collection][LANGUAGE_NONE][$item_key] as $field_key => $field_value) {
            if (asu_isearch_devmode() === FALSE) {
              $form[$field_collection][LANGUAGE_NONE][$item_key]['remove_button']['#disabled'] = TRUE;
              $form[$field_collection][LANGUAGE_NONE][$item_key]['field_isearch_affil_rank_tenure']['#access'] = FALSE;
            }

            if (preg_match('/^field_isearch_/', $field_key) && !in_array($field_key, $field_exception_manifest)) {
              if (asu_isearch_devmode() === FALSE) {
                $form[$field_collection][LANGUAGE_NONE][$item_key][$field_key]['#disabled'] = TRUE;
              }
            }
          }
        }
      }
    }
  }

  // restrict access to delete isearch fields
  if (asu_isearch_devmode() === FALSE && $form_id == 'field_ui_field_delete_form') {
    if (preg_match('/field_isearch_.*?delete$/', $form['#action'])) {
      drupal_access_denied();
      drupal_exit();
    }
  }

  // do not allow any isearch fields to be deleted by hiding the link (fields prefixed with 'field_isearch_')
  if (asu_isearch_devmode() === FALSE && $form_id == 'field_ui_field_overview_form') {
    $field_groups = array(
      'group_basic',
      'group_links',
      'group_expertise_area',
      'group_personal',
      'group_research',
      'group_teaching',
      'group_public_work',
      'group_media',
      'group_advanced',
    );

    foreach ($form['#groups'] as $field_key) {
      if (in_array($field_key, $field_groups)) {
        $form['fields'][$field_key]['delete'] = array();
        $form['fields'][$field_key]['edit'] = array();
      }
    }

    foreach ($form['#fields'] as $field_key) {
      if (preg_match('/^field_isearch_/', $field_key)) {
        $form['fields'][$field_key]['delete'] = array();
        $form['fields'][$field_key]['edit'] = array();
      }
    }
  }

  if ($form_id == 'views_exposed_form') {
    $view_display = $form_state['view']->current_display;
    if (in_array($view_display, array('isearch_directory_list', 'isearch_directory_grid'))) {
      asu_isearch_directory_list_form_alter($form, $form_state, $form_id);
    }
  }
}



function asu_isearch_directory_list_form_alter(&$form, &$form_state, $form_id) {

  $form['#attributes'] = array(
    'class' => array('isearch-directory-filters'),
  );

  // Rebuild rank title field into select field
  $form['field_isearch_affil_title_value']['#theme'] = 'select';
  $form['field_isearch_affil_title_value']['#type'] = 'select';
  $form['field_isearch_affil_title_value']['#multiple'] = FALSE;
  unset($form['field_isearch_affil_title_value']['#size']);
  $form['field_isearch_affil_title_value']['#options'] = array('' => t('Select Title')) + asu_isearch_get_rank_titles();

  if (isset($_GET['field_isearch_affil_title_value'])) {
    $form['field_isearch_affil_title_value']['#value'] = check_plain($_GET['field_isearch_affil_title_value']);
  }

  // reset label for expertise field
  $form['field_isearch_expertise_areas_tid']['#options']['All'] = t('Select Expertise');

  // create omni search field and hide fields that are unecessary
  $form['isearch_omni_search'] = array(
    '#type' => 'textfield',
    '#weight' => -100,
    '#attributes' => array(
      'placeholder' => t('Name or Keyword'),
    ),
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'asu_isearch') . '/js/asu_isearch_omni.js',
      ),
      'css' => array(
        drupal_get_path('module', 'asu_isearch') . '/css/asu_isearch_omni.css',
      ),
    ),
  );

  // Reset exposed form theme so we can set weights on fields
  $form['#theme'] = '';

  $form['sort_by']['#title'] = '';
  foreach ($form['sort_by']['#options'] as $key => $value) {
    $form['sort_by']['#options'][$key] = "Sort by {$value}";
  }

  // Hide sort order field
  $form['sort_order']['#type'] = 'hidden';

  // tag and hide fields so we can manipulate them with attached javascript
  $hidden_fields = array('omni_displayname', 'omni_firstname', 'omni_lastname', 'omni_expertise');
  foreach ($hidden_fields as $fieldname) {
    $form[$fieldname]['#type'] = 'hidden';
    $form[$fieldname]['#attributes']['class'] = array('omni-search-field');
  }

  // hide some filters if they do not have any options
  if (isset($form['field_isearch_expertise_areas_tid']['#options']) && count($form['field_isearch_expertise_areas_tid']['#options']) == 1) {
    $form['field_isearch_expertise_areas_tid']['#access'] = FALSE;
  }

}