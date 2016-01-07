<?php
/**
 * @file
 * Default output for an ASU Spotlight object.
 */
?>
<?php

?>
<div class="field-type-asu-isearch-dept-picker">
  <label><?php print isset($label) ? $label : '' ?></label>
    <?php print theme('asu_isearch_dept_tree', array(
      'items' => $items,
      'settings' => $settings,
    )); ?>
  </div>
</div>