<?php

/**
 * IsearchProfileDirectory
 *
 * This class creates and renders a directory from a set of
 * configuration options as an effort to refactor the current
 * working code.
 */
class IsearchProfileDirectory {
  private $indexView;
  private $directoryListView;
  private $directoryGridView;
  private $contextualFilters;
  private $sortingOptions;


  public function __construct() {

  }

  public function render() {

  }

  public function renderIndex() {

  }

  public function getTitleOptions() {
    $options = array();

    $view = views_get_view('isearch_directory');
    $display_id = 'isearch_directory_titles';

    if (isset($view->display[$display_id])) {
      $view->set_display($display_id);
      $view->init_handlers();
      $view->set_arguments($this->contextualFilters);
      $view->execute();

      foreach ($view->result as $item) {
        $title = $item->field_field_isearch_affil_title[0]['raw']['value'];
        $options[$title] = $title;
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

  public function setSortingOptions() {

  }
}
