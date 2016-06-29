<?php

/**
 * IsearchProfileDirectory
 *
 * This class creates and renders a directory from a set of
 * configuration options as an effort to refactor the current
 * working code.
 */
class IsearchProfileDirectory {
  private $instanceId = null;
  private $directoryListView;
  private $directoryGridView;
  private $exposedFilters;
  private $contextualFilters;
  private $sortingOptions;
  private $view = null;
  private $viewConfig = null;

  private static $currentInstance = 1;

<<<<<<< HEAD
  public function __construct($view_config) {
    $this->viewConfig = $view_config;
=======
  public function __construct() {
>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
    $this->view = views_get_view('isearch_directory', TRUE);

    $this->instanceId = self::$currentInstance++;
    $this->view->isearch_instance_id = $this->instanceId;
  }

<<<<<<< HEAD
=======
  public function setViewConfig($view_config) {
    $this->viewConfig = $view_config;
  }

>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
  public function render() {

  }

  public function renderIndex() {
    $display_id = 'isearch_a_z';
    $this->view->set_display($display_id);
    $this->view->init_display(TRUE);
    $this->view->init_handlers(TRUE);

    $this->view->display_handler->handlers['argument']['field_isearch_lastname_value']->options['summary']['format'] = 'unformatted_summary';
    $this->view->display_handler->handlers['argument']['field_isearch_lastname_value']->options['summary_options']['count'] = 0;
    $this->view->display_handler->handlers['argument']['field_isearch_lastname_value']->options['summary_options']['inline'] = 1;
    $this->view->display_handler->handlers['argument']['field_isearch_lastname_value']->options['default_action'] = 'summary';

    $contextual_filters = $this->contextualFilters;
    // reset to null to show the entire index
    $contextual_filters[0] = NULL;
    $this->view->set_arguments($contextual_filters);

    if (isset($this->view->filter['field_isearch_affil_title_value_1'])) {
<<<<<<< HEAD
=======
      if ($this->viewConfig->filter_title == '') {
        $this->viewConfig->filter_title = '.*';
      }
>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
      $this->view->filter['field_isearch_affil_title_value_1']->value = $this->viewConfig->filter_title;
    }

    $this->view->pre_execute();
    $this->view->execute();

    $items = array();
<<<<<<< HEAD
=======

>>>>>>> a316bb350e7ecb392b1b384664ed7b42cc7b2d4d
    foreach ($this->view->result as $key => $result) {
      $index_letter = $result->field_isearch_lastname_value_truncated;
      $items[] = t('<a href="@url">@letter</a>', array(
        '@url'    => url(current_path(), array(
          'query' => array(
            'isrch_index' => $index_letter,
            'isrch_id'    => $this->instanceId,
          ),
        )),
        '@letter' => $index_letter,
      ));
    }

    return theme_item_list(array(
      'items'      => $items,
      'title'      => '',
      'type'       => 'ul',
      'attributes' => array(
        'class' => 'isearch-a-z-index',
      ),
    ));
  }

  public function getInstanceId() {
    return $this->instanceId;
  }

  public function getTitleOptions() {
    $options = array();

    $display_id = 'isearch_directory_titles';

    if (isset($this->view->display[$display_id])) {

      $contextual_filters = $this->contextualFilters;
      $contextual_filters[0] = 'all';

      $view = views_get_view('isearch_directory', TRUE);
      $view->set_display($display_id);
      $view->init_handlers();
      $view->set_arguments($contextual_filters);
      $view->execute();

      foreach ($view->result as $item) {
        if (isset($item->field_field_isearch_affil_title)) {
          $title = $item->field_field_isearch_affil_title[0]['raw']['value'];
          $options[$title] = $title;
        }
      }
    }

    return $options;
  }

  public function setIndexView() {

  }

  public function setDirectoryListView() {

  }

  public function setDirectoryGridView() {

  }

  public function setContextualFilters($filters) {
    $this->contextualFilters = $filters;
  }

  public function setExposedFilters($filters) {
    $this->exposedFilters = $filters;
  }

  public function setSortingOptions() {

  }
}
