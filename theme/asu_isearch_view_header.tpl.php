<?php
/**
 * @file
 * Custom header output for ASU iSearch directory list header
 */
?>
<?php if ($render_header): ?>
<div class="views-row views-row-1 views-row-odd views-row-first row">
    <?php if ($display_photo): ?>
    <div class="views-field views-field-field-isearch-photo-url col-md-2 user-photo"></div>
    <?php endif; ?>
    <?php if ($display_name): ?>
    <div class="views-field views-field-nothing-1 col-md-4 user-name">
      <span class="field-content"><div class="name">Name</div></span>
    </div>
    <?php endif; ?>
    <?php if ($display_contact): ?>
    <div class="views-field views-field-nothing col-md-3 user-contact">
      <span class="field-content"><div class="email">Contact</div></span>
    </div>
    <?php endif; ?>
    <?php if ($display_expertise): ?>
    <div class="views-field views-field-field-isearch-expertise-areas col-md-3 user-expertise">
      <span class="field-content"><div class="expertise">Expertise</div></span>
    </div>
    <?php endif; ?>
</div>
<?php endif; ?>