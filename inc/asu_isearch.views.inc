<?php

/**
 * Implements hook_views_pre_render()
 */
function asu_isearch_views_pre_render(&$view) {

  if ($view->name == 'isearch_affiliations') {

    $display_affiliations = (bool) variable_get('isearch_display_affiliations', 0);
    if (FALSE === $display_affiliations) {
      $view->field['field_isearch_affil_title']->options['exclude'] = TRUE;
    }
  }

  if ($view->name == 'isearch_directory') {
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
        $args[1] = 1;
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
