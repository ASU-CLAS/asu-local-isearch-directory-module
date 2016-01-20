<?php

/**
 * Implements hook_views_pre_render()
 */
function asu_isearch_views_pre_render(&$view) {

  if ($view->name == 'isearch_affiliations') {
    // If we're not displaying all affiliations, hide the title so it isn't redundant.
    $display_affiliations = (bool) variable_get('isearch_display_affiliations', 0);
    if (FALSE === $display_affiliations) {
      $view->field['field_isearch_affil_title']->options['exclude'] = TRUE;
    }
  }

  if ($view->name == 'isearch_directory') {
    $display_options = array(
      'display_header' => $view->view_config->display_header,
      'display_photo' => $view->view_config->display_photo,
      'display_name' => $view->view_config->display_name,
      'display_contact' => $view->view_config->display_contact,
      'display_expertise' => $view->view_config->display_expertise,
    );
    $display_options = (object) $display_options;
    $view_column_widths = asu_isearch_get_view_column_widths($display_options);

    $column_names = array_keys($view_column_widths);
    foreach ($view->field as $field_name => $data) {
      if ($view->field[$field_name]->options['exclude'] == 0) {
        foreach ($column_names as $column_name) {
          if (strstr($view->field[$field_name]->options['element_wrapper_class'], 'user-' . $column_name)) {
            $view->field[$field_name]->options['element_wrapper_class'] .= ' col-md-' . $view_column_widths[$column_name];
          }
        }
      }
    }


    foreach ($view->result as $key => $result) {
      $entity = $result->_field_data['nid']['entity'];
      $wrapper = entity_metadata_wrapper('node', $entity);

      // display correct photo if any
      $photo_permission = strtolower($wrapper->field_isearch_photo_permission->value());
      if ($photo_permission != 'public' || empty($entity->field_isearch_photo_url)) {

        $display_default_photo = variable_get('isearch_display_default_photo', FALSE);
        $default_photo = asu_isearch_get_default_photo();

        if ($display_default_photo && $default_photo) {
          $view->result[$key]->field_field_isearch_photo_url[0]['rendered']['#item'] = (array) $default_photo;
          $view->result[$key]->field_field_isearch_photo_url[0]['raw'] = (array) $default_photo;
        }
        else {
          $view->result[$key]->field_field_isearch_photo_url = array();
        }
      }
    }
  }
}

/**
 * Implements hook_views_pre_view()
 */
function asu_isearch_views_pre_view(&$view, &$display_id, &$args) {
  if ($view->name == 'isearch_affiliations') {
    if ($node = menu_get_object()) {
      if (isset($node->type) && $node->type == 'asu_isearch_profile') {
        $args[0] = $node->nid;
      }

      $display_affiliations = (bool) variable_get('isearch_display_affiliations', 0);
      if (FALSE === $display_affiliations) {

        $local_primary_affiliation = (!empty($node->field_isearch_primary_aff))
          ? $node->field_isearch_primary_aff[LANGUAGE_NONE][0]['target_id']
          : null;

        if (!empty($node->field_isearch_local_primary_aff)) {
          $local_primary_affiliation = $node->field_isearch_local_primary_aff[LANGUAGE_NONE][0]['target_id'];
        }

        $args[1] = $local_primary_affiliation;
        $view->style_options['tab_field'] = 'affiliation_tab_title';
      }
    }
  }
}



/**
 * Returns an array of available views
 */
function asu_isearch_get_views() {
  $views = array();
  $loaded_views = views_get_all_views();

  foreach ($loaded_views as $view_name => $view) {
    // loop through display
    foreach ($view->display as $display_name => $display) {

      if ($display_name == 'default') {
        continue;
      }

      $id = $view_name . ':' . $display_name;
      $views[$id] = strip_tags(format_string(':view (:display)', array(':view' => $view->human_name, ':display' => $display_name)));
    }
  }

  return $views;
}


/**
 * Implements hook_views_query_alter()
 */
function asu_isearch_views_query_alter(&$view, &$query) {
  if ($view->name == 'isearch_directory') {
    if ($view->view_config->employee_type_exclude == 1) {
      $dept_id_field = 'field_collection_item_field_data_field_isearch_affiliations__field_data_field_isearch_affil_empl_key.field_isearch_affil_empl_key_value';
      $found_employee_type_filter = FALSE;

      foreach ($query->where as $item_index => $item) {
        if (!empty($item['conditions'])) {
          foreach ($item['conditions'] as $condition_index => $condition) {
            if (!is_object($condition['field']) && $condition['field'] == $dept_id_field) {
              $found_employee_type_filter = TRUE;
              if (is_array($query->where[$item_index]['conditions'][$condition_index]['value'])) {
                $operator = 'NOT IN';
              }
              else {
                $operator = '!=';
              }
              $query->where[$item_index]['conditions'][$condition_index]['operator'] = $operator;
            }
          }
        }
      }

      if (!$found_employee_type_filter) {
        $query->where[0]['conditions'][] = array(
          'field' => $dept_id_field,
          'operator' => '=',
          'value' => 'unknown',
        );
      }
    }
  }
}
