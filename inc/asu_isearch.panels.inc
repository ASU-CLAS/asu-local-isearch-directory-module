<?php

/**
 * Implements hook_panels_pane_content_alter().
 */
function asu_isearch_panels_pane_content_alter($content, $pane, $args, $contexts) {
  if (is_object($content) && empty($content->content) || (isset($content->content[0]['#markup']) && strlen($content->content[0]['#markup']) === 0)) {
    $content->content = array();
    return;
  }

  if ($pane->subtype == 'node:field_isearch_rsrch_website') {
    $website = $content->content['#items'][0]['value'];
    $content->content[0]['#markup'] = l($website, $website, array('attributes' => array(
      'target' => '_blank',
    )));
  }

  // Get permission to display profile image
  if ($pane->subtype == 'node:field_isearch_photo_url') {
  	// check and see if we have uploaded a local image
  	// if so, display it vs whatever settings they currently have
  	if (isset($content->content['#object']->field_isearch_local_photo_url) && !empty($content->content['#object']->field_isearch_local_photo_url)) {
  		$content->content[0]['#item'] = (array) $content->content['#object']->field_isearch_local_photo_url[LANGUAGE_NONE][0];
  	}
  	else {
  		$photo_preference = $content->content['#object']->field_isearch_photo_preference[LANGUAGE_NONE][0]['value'];
      $photo_permission = $content->content['#object']->field_isearch_photo_permission[LANGUAGE_NONE][0]['value'];

      if ($photo_preference == 'none' || $photo_permission != 'public') {

        $display_default_photo = variable_get('isearch_display_default_photo', FALSE);
        $default_photo = asu_isearch_get_default_photo();

        if ($display_default_photo && $default_photo) {
          $content->content[0]['#item'] = (array) $default_photo;
        }
        else {
          $content->content['#access'] = FALSE;
        }
      }
  	}

  	$content->content[0]['#image_style'] = 'asu_isearch_profile';
  }

  // Add email icon to email address
  if ($pane->subtype == 'node:field_isearch_email') {
    $email = $content->content['#items'][0]['email'];
    $content->content[0]['#markup'] = l('<i class="fa fa-envelope"></i> ' . $email, 'mailto:'.$email, array('html' => TRUE));
  }

  // Add email icon to phone number
  if ($pane->subtype == 'node:field_isearch_phone') {
    $phone = $content->content['#items'][0]['value'];
    $link = preg_replace('/[^\d]/', '', $phone);
    $content->content[0]['#markup'] = l('<i class="fa fa-phone"></i> ' . $phone, 'tel:'.$link, array('html' => TRUE));
  }

  // Add file icon to CV url
  if ($pane->subtype == 'node:field_isearch_cv_url') {
    $cvurl = $content->content['#items'][0]['value'];
    $link = ASU_ISEARCH_DIRECTORY_URL . '/sites/default/files/' . $cvurl;
    $cvinfo = pathinfo($link);
    $content->content[0]['#markup'] = '<i class="fa fa-file"></i> ' . l($cvinfo['basename'], $link, array(
      'attributes' => array(
        'target' => '_blank',
      ),
    ));
  }

  // @TODO: There could possibly be a small bug here if there isn't a building but a room is present
  // Add pin icon to location
  if ($pane->subtype == 'node:field_isearch_campus_building') {
    $building = $content->content['#items'][0]['value'];
    $content->content[0]['#markup'] = '<i class="fa fa-map-marker"></i> ' . $building;
  }


  // social icons
  $social_fields_icons = array(
    'node:field_isearch_social_facebook' => 'fa-facebook-square',
    'node:field_isearch_social_twitter' => 'fa-twitter-square',
    'node:field_isearch_social_googleplus' => 'fa-google-plus-square',
    'node:field_isearch_social_linkedin' => 'fa-linkedin-square',
    'node:field_isearch_social_personalweb' => 'fa-globe',
  );

  if (array_key_exists($pane->subtype, $social_fields_icons)) {
    $value = $content->content[0]['#markup'];
    $content->content[0]['#markup'] = l('<i class="fa '.$social_fields_icons[$pane->subtype].'"></i>', $value, array(
      'attributes' => array(
        'target' => '_blank',
      ),
      'html' => TRUE,
    ));
  }


  if ($pane->panel == 'affiliations' && $pane->subtype == 'node:field_isearch_affiliations') {

    // unique quicktabs identifier
    $name = 'isearch_affiliations';

    // initial settings
    $settings = array(
      'ajax'          => 0,
      'hide_if_empty' => 1,
      'default_tab'   => 0,
      'title'         => 'My custom quicktab',
      'renderer'      => 'quicktabs',
      'style'         => 'isearch',
    );

    $tabs = array();
    $weight = 0;

    foreach ($content->content['#items'] as $key => $item_id) {
      $item = current($content->content[$key]['entity']['field_collection_item']);
      $title = $item['#entity']->field_isearch_affil_title[LANGUAGE_NONE][0]['value'];

      $tab_html = drupal_render($content->content[$key]);

      $tabs[] = array(
        'type'  => 'prerendered',
        'title' => $title,
        'weight' => $weight,
        'contents' => $tab_html,
      );
      $weight++;
    }

    $html = quicktabs_build_quicktabs($name, $settings, $tabs);

    $content->content = array(
      '#markup' => drupal_render($html),
    );
  }
}


/**
 * Implements hook_preprocess_panels_pane().
 */
function asu_isearch_preprocess_panels_pane(&$variables){

  if (isset($variables['content']['#bundle']) && $variables['content']['#bundle'] === 'isearch_directory') {

    // shortcut to content render array
    $content =& $variables['content'];

    // Setup variables here...
    $content['title'] = '';

    // fixes a bug with pagination when sort order field is exposed in view
    if (isset($_GET['sort_by']) && in_array(strtoupper($_GET['sort_by']), array('DESC', 'ASC'))) {
      unset($_GET['sort_by']);
    }

    // load the configuration saved for this pane
    $default_view_config = asu_isearch_get_default_view_config();
    $view_config = json_decode($content['field_isearch_config']['#object']->field_isearch_config[LANGUAGE_NONE][0]['value']);
    $view_config = (object) array_merge($default_view_config, (array) $view_config);

    if (!isset($view_config->view_display)) {
      $view_config->view_display = 'isearch_directory:directory_list';
    }
    list($view_name, $view_display_id) = explode(':', $view_config->view_display);

    // build out the view
    $view = views_get_view($view_name);
    $display_id = $view_display_id;

    $view->set_display($display_id);
    $view->init_handlers();

    $employee_types = 'all';
    $departments = 'all';
    $tenure_display = 'all';
    $expertise_areas = 'all';
    $local_tags = 'all';


    // filter content by:
    // affiliation title
    if (isset($view->filter['field_isearch_affil_title_value_1'])) {
      if (trim($view_config->filter_title) == '') {
        unset($view->filter['field_isearch_affil_title_value_1']);
      }
      else {
        $view->filter['field_isearch_affil_title_value_1']->value = $view_config->filter_title;
      }
    }

    // display managers - We will unset the value from the sort array if we don't want them at the top.
    if ($view_config->show_managers === 0) {
      unset($view->sort['field_isearch_affil_manager_value']);
    }

    // ensure tenure sort order is always descending
    if (isset($view->sort['field_isearch_affil_rank_tenure_value'])) {
      $view->sort['field_isearch_affil_rank_tenure_value']->options['order'] = 'DESC';
    }

    switch ($view_config->default_sort_by) {
      case 'field_isearch_firstname_value':
        if (isset($view->sort['field_isearch_lastname_value'])) {
          unset($view->sort['field_isearch_lastname_value']);
        }

      case 'field_isearch_lastname_value':
        if (isset($view->sort['field_isearch_affil_rank_weight_value'])) {
          unset($view->sort['field_isearch_affil_rank_weight_value']);
        }
        if (isset($view->sort['field_isearch_affil_rank_tenure_value'])) {
          unset($view->sort['field_isearch_affil_rank_tenure_value']);
        }
        break;

      case 'field_isearch_affil_rank_weight_value':
      default:
        if (isset($view->sort['field_isearch_affil_rank_weight_value'])) {
          $view->sort['field_isearch_affil_rank_weight_value']->options['order'] = 'ASC';
        }
        if (isset($view->sort['field_isearch_affil_rank_tenure_value'])) {
          unset($view->sort['field_isearch_affil_rank_tenure_value']);
        }
        break;
    }

    // should we include all subdepartments?
    if ($view_config->include_subdepts) {
      $departments = implode(',', $view_config->depts->tree_nids);
    }
    // include only the department root
    else {
      $departments = $view_config->depts->dept_nid;
    }

    // display only certain employee types?
    if (is_array($view_config->employee_types) && !empty($view_config->employee_types) && $view_config->employee_types[0] != 'All') {
      $employee_type_keys = array();
      foreach ($view_config->employee_types as $type) {
        $employee_type_keys[] = preg_replace('/[^a-zA-Z]/', '_', trim($type));
      }
      $employee_types = implode(',', $employee_type_keys);
    }

    // limit display to tenure type?
    if (!empty($view_config->tenure_display)) {
      $tenure_display = array();
      $view_config->tenure_display = (array) $view_config->tenure_display;

      if ($view_config->tenure_display['Tenure'] === 'Tenure') {
        $tenure_display[] = "1";
      }
      if ($view_config->tenure_display['Non-Tenure'] === 'Non-Tenure') {
        $tenure_display[] = "0";
      }
      if (empty($tenure_display)) {
        $tenure_display = array('all');
      }

      $tenure_display = implode(',', $tenure_display);
    }

    // display only certain employee types?
    if ($view_config->expertise_areas) {
      $expertise_areas = implode('+', (array) $view_config->expertise_areas);
    }

    // display only certain tags?
    if (!empty($view_config->local_tags)) {
      $local_tags = implode('+', (array) $view_config->local_tags);
    }

    // set other display options
    if (asu_isearch_is_isearch_view($view)) {
      $display_header    = TRUE;
      $display_photo     = TRUE;
      $display_name      = TRUE;
      $display_contact   = TRUE;
      $display_expertise = TRUE;

      if (isset($view_config->display_header) && (bool) $view_config->display_header == FALSE) {
        $display_header = FALSE;
      }

      if (isset($view_config->display_photo) && (bool) $view_config->display_photo == FALSE) {
        $view->field['field_isearch_local_photo_url']->options['exclude'] = TRUE;
        $display_photo = FALSE;
      }

      if (isset($view_config->display_name) && (bool) $view_config->display_name == FALSE) {
        $view->field['nothing_1']->options['exclude'] = TRUE;
        $display_name = FALSE;
      }

      if (isset($view_config->display_contact) && (bool) $view_config->display_contact == FALSE) {
        $view->field['nothing']->options['exclude'] = TRUE;
        $display_contact = FALSE;
      }

      if (isset($view_config->display_expertise) && (bool) $view_config->display_expertise == FALSE) {
        $view->field['field_isearch_expertise_areas']->options['exclude'] = TRUE;
        $display_expertise = FALSE;
      }

      $display_options = (object) array(
        'display_header' => $display_header,
        'display_photo' => $display_photo,
        'display_name' => $display_name,
        'display_contact' => $display_contact,
        'display_expertise' => $display_expertise,
      );

      $directory_header = theme_render_template(drupal_get_path('module', 'asu_isearch') . '/theme/asu_isearch_view_header.tpl.php', array(
        'display_options' => $display_options,
      ));

      $view->display_handler->handlers['header']['area_text_custom']->options['content'] = $directory_header;

    }

    if (isset($view_config->display_short_bio) && !$view_config->display_short_bio) {
      $view->display_handler->handlers['field']['field_isearch_short_bio']->options['alter']['alter_text'] = TRUE;
      $view->display_handler->handlers['field']['field_isearch_short_bio']->options['alter']['text'] = '&nbsp;';
    }

    if (isset($view_config->display_building) && !$view_config->display_building) {
      $view->display_handler->handlers['field']['field_isearch_campus_building']->options['alter']['alter_text'] = TRUE;
      $view->display_handler->handlers['field']['field_isearch_campus_building']->options['alter']['text'] = '&nbsp;';
    }

    if (isset($view_config->display_room) && !$view_config->display_room) {
      $view->display_handler->handlers['field']['field_isearch_campus_room']->options['alter']['alter_text'] = TRUE;
      $view->display_handler->handlers['field']['field_isearch_campus_room']->options['alter']['text'] = '&nbsp;';
    }

    // set the contextual filters arguments
    $view->set_arguments(array($employee_types, $departments, $tenure_display, $expertise_areas, $local_tags));

    // pagination options
    $view->set_items_per_page($view_config->pager_items_per_page);
    $view->set_offset($view_config->pager_offset);

    if ($view_config->pager_display != 'paged') {
      $pager = $view->display_handler->get_option('pager');
      $pager['type'] = 'none';

      if ($view_config->pager_display == 'fixed') {
        $pager['type'] = 'some';
      }

      $pager['options']['items_per_page'] = $view_config->pager_items_per_page;
      $view->display_handler->set_option('pager', $pager);
    }

    $view->view_config = $view_config;
    $view->pre_execute();

    $content['isearch_view'] = array(
      '#markup' => $view->render($display_id),
    );

    if (!$view_config->show_filter_titles
      && !$view_config->show_filter_expertise
      && !$view_config->show_filter_sort
      && !$view_config->show_filter_omni
    ) {
      $view_config->show_filters = FALSE;
    }

    // display filters? - Should we display or hide the filters
    if ($view_config->show_filters) {
      // build the exposed form fields
      $form_state = array(
        'view' => $view,
        'display' => $view->display_handler->display,
        'exposed_form_plugin' => $view->display_handler->get_plugin('exposed_form'),
        'method' => 'get',
        'rerender' => TRUE,
        'no_redirect' => TRUE,
      );

      $form = drupal_build_form('views_exposed_form', $form_state);

      // update search filters
      $view->set_items_per_page(0);
      $view->set_offset(0);

      // _asu_isearch_update_view_form_filters($view, $form);

      // change some values to correctly display the form
      $form['#action'] = '/' . drupal_get_path_alias(current_path());
      $form['#weight'] = -10;

      $form['field_isearch_affil_title_value']['#access'] = (bool) $view_config->show_filter_titles;
      $form['field_isearch_expertise_areas_tid']['#access'] = (bool) $view_config->show_filter_expertise;
      $form['sort_by']['#access'] = (bool) $view_config->show_filter_sort;
      $form['isearch_omni_search']['#access'] = (bool) $view_config->show_filter_omni;

      // set the default sort by
      $form['sort_by']['#value'] = $view_config->default_sort_by;

      // set some form defaults when submitted ...
      $params = drupal_get_query_parameters();
      if (isset($params['field_isearch_expertise_areas_tid'])) {
        $form['field_isearch_expertise_areas_tid']['#value'] = intval($params['field_isearch_expertise_areas_tid']);

        if (isset($params['sort_by'])) {
          $form['sort_by']['#default_value'] = check_url($params['sort_by']);
          $form['sort_by']['#value'] = check_url($params['sort_by']);
        }

        if (isset($params['sort_order'])) {
          // Forcing ascending sort order for now
          $form['sort_order']['#value'] =  'ASC';
        }
      }

      $content['isearch_view_exposed_form'] = $form;
    }

    // hide configuration
    unset($content['field_isearch_config']);
  }
}


function _asu_isearch_update_view_form_filters($view, &$form) {
  $filter_fields = array();
  $filter_field_labels = array('field_isearch_affil_title', 'field_isearch_expertise_areas');

  foreach ($filter_field_labels as $field_name) {
    $filter_fields[$field_name] = $view->field[$field_name];
  }

  $view->field = $filter_fields;
}


/**
 * Implements hook_preprocess_fieldable_panels_pane().
 */
function asu_isearch_preprocess_fieldable_panels_pane(&$variables){
  if ($variables['elements']['#bundle'] === 'isearch_directory'){
    $title = $variables['elements']['#fieldable_panels_pane']->title;
    $variables['elements']['title'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'h2',
      '#value' => $title
    );
  }
}



/**
 * Implementation of hook_form_FORM_ID_alter()
 */
function asu_isearch_form_fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_alter(&$form, &$form_state, $form_id) {
  if (isset($form['#entity']) && $form['#entity']->bundle == 'isearch_directory') {
    // add our custom submit handler
    $form['#submit'][] = 'asu_isearch_form_fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_submit';
    $form['#pre_render'][] = 'asu_isearch_form_fieldable_panels_panes_fieldable_panels_pane_content_type_group_pre_render';

    $form['isearch_custom'] = array(
      '#type' => 'div',
      '#prefix' => '<div class="asu-isearch-dept-config">',
      '#suffix' => '</div>',
    );

    $config = asu_isearch_get_default_view_config();

    // we are setting this up for the first time...
    if (!isset($form['#entity']->is_new) || !$form['#entity']->is_new) {
      // load the configuration from saved entity
      $config_defaults = $config;
      $config = (array) @json_decode($form['#entity']->field_isearch_config[LANGUAGE_NONE][0]['value']);
      $config = array_merge($config_defaults, $config);
    }

    $config = (object) $config;

    // Load the departments from our global configuration to use in our dept tree widget
    $feed_depts = variable_get('isearch_admin_feed_dept', null);

    if (!is_null($feed_depts)) {
      $feed_depts = json_decode($feed_depts);
    }
    else {
      $feed_depts = new stdclass();
      $feed_depts->dept_nid  = 'ASU';
      $feed_depts->dept_id   = 'ASU';
      $feed_depts->tree_nids = array();
    }

    // lets make sure we load the correct department tree nids into our widget
    if (isset($config->depts)) {
      // if we somehow lost the root nid in our configuration, we'll "remind" our tree of it
      if (!in_array($config->depts->dept_nid, $feed_depts->tree_nids)) {
        $feed_depts->tree_nids = $config->depts->tree_nids;
      }
    }
    else {
      $config->depts = $feed_depts;
    }

    $employee_types = asu_isearch_get_employee_types();
    $employee_types_options = array('All');
    foreach ($employee_types['nodes'] as $employee_type_node) {
      $employee_types_options[] = $employee_type_node['node']['name'];
    }

    $default_employee_types = array();
    foreach ($config->employee_types as $value) {
      $default_employee_types[] = $value;
    }

    $default_tenure_display = array();
    foreach ($config->tenure_display as $value) {
      $default_tenure_display[] = $value;
    }

    $form['isearch_custom']['dept_config'] = array(
      '#markup' => theme('asu_isearch_dept_config', array()),
      '#attached' => array(
        'css' => array(
          drupal_get_path('module', 'asu_isearch') . '/css/asu_isearch.admin.css',
        ),
      ),
    );

    $form['isearch_custom']['employee_types'] = array(
      '#type' => 'checkboxes',
      '#options' => drupal_map_assoc($employee_types_options),
      '#default_value' => $default_employee_types,
      '#title' => t('Select employee types to be shown'),
    );

    $form['isearch_custom']['tenure_display'] = array(
      '#type' => 'checkboxes',
      '#options' => drupal_map_assoc(array('Tenure', 'Non-Tenure')),
      '#default_value' => $default_tenure_display,
      '#title' => '',
    );

    $form['isearch_custom']['show_filters'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show Filters',
      '#default_value' => $config->show_filters,
    );

    $form['isearch_custom']['show_filter_omni'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show Name/Keyword Filter',
      '#default_value' => $config->show_filter_omni,
    );

    $form['isearch_custom']['show_filter_titles'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show Titles Filter',
      '#default_value' => $config->show_filter_titles,
    );

    $form['isearch_custom']['show_filter_expertise'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show Expertise Filter',
      '#default_value' => $config->show_filter_expertise,
    );

    $form['isearch_custom']['show_filter_sort'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show Sort By Filter',
      '#default_value' => $config->show_filter_sort,
    );

    $form['isearch_custom']['show_managers'] = array(
      '#type' => 'checkbox',
      '#title' => 'Show Managers at Top of Display',
      '#default_value' => $config->show_managers,
    );

    $form['isearch_custom']['include_subdepts'] = array(
      '#type' => 'checkbox',
      '#title' => 'Include Sub-department Results',
      '#default_value' => $config->include_subdepts,
    );

    $form['isearch_custom']['employee_type_exclude'] = array(
      '#type' => 'radios',
      '#options' => array(0 => 'Include', 1 => 'Exclude'),
      '#title' => 'Employee type visibility strategy',
      '#default_value' => $config->employee_type_exclude,
    );


    $form['isearch_custom']['filter_departments'] = array(
      '#type' => 'hidden',
      '#default_value' => json_encode($config->depts),
      '#attributes' => array('id' => 'filter_departments'),
    );

    $form['isearch_custom']['isearch_dept_tree'] = array(
      '#attached' => array(
        'css' => array(
          drupal_get_path('module', 'asu_isearch') . '/css/asu_isearch.dept_tree.css',
        ),
      ),
      '#markup' => theme('asu_isearch_dept_picker', array(
        'items' => array(
          array('directory_items' => $feed_depts->tree_nids),
        ),
        'settings' => array(
          'standalone_tree' => TRUE,
          'config_target' => '#filter_departments',
        ),
      )),
    );

    $expertise_vocabulary = taxonomy_vocabulary_machine_name_load('isearch_expertise');
    if ($expertise_terms = taxonomy_get_tree($expertise_vocabulary->vid, 0, NULL, TRUE)) {
      $expertise_areas = array();

      foreach ($expertise_terms as $term) {
        $expertise_areas[$term->tid] = $term->name;
      }

      if (!is_array($config->expertise_areas)) {
        $config->expertise_areas = (array) $config->expertise_areas;
      }

      if (!empty($expertise_areas)) {
        $form['isearch_expertise_areas'] = array(
          '#type' => 'select',
          '#options' => $expertise_areas,
          '#default_value' => $config->expertise_areas,
          '#multiple' => TRUE,
          '#title' => t('Filter profiles by these expertise areas'),
          '#weight' => 100,
        );
      }
    }

    $local_tags_vocabulary = taxonomy_vocabulary_machine_name_load('isearch_local_tags');
    if ($local_tags_terms = taxonomy_get_tree($local_tags_vocabulary->vid, 0, NULL, TRUE)) {
      $local_tags = array();

      foreach ($local_tags_terms as $term) {
        $local_tags_areas[$term->tid] = $term->name;
      }

      if (!is_array($config->local_tags)) {
        $config->local_tags = (array) $config->local_tags;
      }

      if (!empty($local_tags_areas)) {
        $form['isearch_local_tags'] = array(
          '#type' => 'select',
          '#options' => $local_tags_areas,
          '#default_value' => $config->local_tags,
          '#multiple' => TRUE,
          '#title' => t('Filter profiles by these local tags'),
          '#weight' => 100,
        );
      }
    }

    $form['isearch_advanced'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced'),
      '#collapsible' => FALSE,
      '#group' => 'settings_tabs',
      '#weight' => 200,
    );

    $available_views = asu_isearch_get_views();
    $form['isearch_advanced']['isearch_view_display'] = array(
      '#type' => 'select',
      '#title' => 'Select a view to display',
      '#options' => $available_views,
      '#default_value' => $config->view_display,
    );

    $form['isearch_advanced']['pager_display'] = array(
      '#type' => 'select',
      '#options' => array('paged' => 'Paged output', 'all' => 'Display all items', 'fixed' => 'Display a specified number of items'),
      '#title' => 'Display pagination',
      '#default_value' => $config->pager_display,
    );

    $form['isearch_advanced']['pager_items_per_page'] = array(
      '#type' => 'textfield',
      '#title' => 'Items to display',
      '#description' => t('The number of items to display. Enter 0 for no limit.'),
      '#default_value' => $config->pager_items_per_page,
    );

    $form['isearch_advanced']['pager_offset'] = array(
      '#type' => 'textfield',
      '#title' => 'Offset',
      '#description' => t('The number of items to skip. For example, if this field is 3, the first 3 items will be skipped and not displayed.'),
      '#default_value' => $config->pager_offset,
    );

    $form['isearch_advanced']['display_short_bio'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display short bio field?',
      '#default_value' => $config->display_short_bio,
    );

    $form['isearch_advanced']['display_header'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display header labels?',
      '#default_value' => $config->display_header,
    );

    $form['isearch_advanced']['display_photo'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display profile photo?',
      '#default_value' => $config->display_photo,
    );

    $form['isearch_advanced']['display_name'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display name column?',
      '#default_value' => $config->display_name,
    );

    $form['isearch_advanced']['display_contact'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display contact column?',
      '#default_value' => $config->display_contact,
    );

    $form['isearch_advanced']['display_expertise'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display expertise column?',
      '#default_value' => $config->display_expertise,
    );

    $form['isearch_advanced']['display_building'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display building?',
      '#default_value' => $config->display_building,
    );

    $form['isearch_advanced']['display_room'] = array(
      '#type' => 'checkbox',
      '#title' => 'Display room?',
      '#default_value' => $config->display_room,
    );

    $default_sort_by_options = array(
      'field_isearch_affil_rank_weight_value' => 'Rank',
      'field_isearch_lastname_value' => 'Last Name',
      'field_isearch_firstname_value' => 'First Name',
    );

    $form['isearch_advanced']['default_sort_by'] = array(
      '#type' => 'select',
      '#options' => $default_sort_by_options,
      '#title' => 'Default Sort Order',
      '#default_value' => $config->default_sort_by,
    );

    $form['isearch_advanced']['filter_title'] = array(
      '#type' => 'textfield',
      '#title' => 'Filter Title',
      '#description' => t('Limit results by the affiliation title. Regular expression allowed.'),
      '#default_value' => $config->filter_title,
    );

    // Hide isearch config field
    $form['field_isearch_config']['#access'] = FALSE;
  }
}



function asu_isearch_form_fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_submit($form, &$form_state) {
  // store values
  $values = $form_state['values'];

  $employee_types = array();
  foreach ($values['employee_types'] as $key => $value) {
    if ($value === 'All') {
      $employee_types = array('All');
      break;
    }
    if ($value !== 0) {
      $employee_types[] = $value;
    }
  }

  $config = array(
    'employee_types'        => $employee_types,
    'employee_type_exclude' => $values['employee_type_exclude'],
    'expertise_areas'       => $values['isearch_expertise_areas'],
    'local_tags'            => $values['isearch_local_tags'],
    'show_managers'         => $values['show_managers'],
    'show_filters'          => $values['show_filters'],
    'show_filter_omni'      => $values['show_filter_omni'],
    'show_filter_titles'    => $values['show_filter_titles'],
    'show_filter_expertise' => $values['show_filter_expertise'],
    'show_filter_sort'      => $values['show_filter_sort'],
    'tenure_display'        => $values['tenure_display'],
    'view_display'          => $values['isearch_view_display'],
    'include_subdepts'      => $values['include_subdepts'],
    'depts'                 => json_decode($values['filter_departments']),
    'pager_display'         => $values['pager_display'],
    'pager_items_per_page'  => $values['pager_items_per_page'],
    'pager_offset'          => $values['pager_offset'],
    'display_short_bio'     => $values['display_short_bio'],
    'display_header'        => $values['display_header'],
    'display_photo'         => $values['display_photo'],
    'display_name'          => $values['display_name'],
    'display_contact'       => $values['display_contact'],
    'display_expertise'     => $values['display_expertise'],
    'display_building'      => $values['display_building'],
    'display_room'          => $values['display_room'],
    'default_sort_by'       => $values['default_sort_by'],
    'filter_title'          => $values['filter_title'],
  );

  $form_state['entity']->field_isearch_config[LANGUAGE_NONE][0]['value'] = json_encode($config);
  entity_save('fieldable_panels_pane', $form_state['entity']);
}



function asu_isearch_form_fieldable_panels_panes_fieldable_panels_pane_content_type_group_pre_render(&$element) {
  $element['content_settings']['isearch_custom'] = $element['isearch_custom'];
  unset($element['isearch_custom']);
  return field_group_build_entity_groups($element, 'form');
}