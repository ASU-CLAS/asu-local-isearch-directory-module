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

/**
 * Implements hook_entity_presave()
 */
function asu_isearch_entity_presave($entity, $type) {
  if ($type == 'field_collection_item' && $entity->field_name == 'field_isearch_affiliations') {

    $affiliation = entity_metadata_wrapper($type, $entity);

    $affiliation_rank_weight = FALSE;

    $is_faculty_ranked = (!empty($entity->field_isearch_affil_rank[LANGUAGE_NONE]));
    if ($is_faculty_ranked) {
      $affiliation_rank_weight = $affiliation->field_isearch_affil_rank->field_isearch_rank_weight->value();
    }

    $has_local_weight = (!empty($entity->field_isearch_affil_local_weight[LANGUAGE_NONE]));
    if ($has_local_weight) {
      $affiliation_rank_weight = $affiliation->field_isearch_affil_local_weight->value();
    }

    // set the rank weight
    if ($affiliation_rank_weight === FALSE) {
      $entity->field_isearch_affil_rank_weight[LANGUAGE_NONE] = array();
    }
    else {
      $entity->field_isearch_affil_rank_weight[LANGUAGE_NONE][0]['value'] = $affiliation_rank_weight;
    }
  }
}
