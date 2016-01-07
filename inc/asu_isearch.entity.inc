<?php

/**
 * Implements hook_entity_view()
 */
function asu_isearch_entity_view($entity, $type, $view_mode, $langcode) {
  if ($type == 'node' && $entity->type == 'asu_isearch_profile') {
    // Suppress title
    drupal_set_title(FALSE);
  }
}
