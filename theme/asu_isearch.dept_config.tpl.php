<?php
/**
 * @file
 * Default output for an ASU Spotlight object.
 */
?>

<div class="form-type-isearch-dept-config">
  <div class="row asu_isearch_toggle_options">
    <h4>Display Options</h4>
    <div class="col-md-4 btn-group-vertical asu_isearch_admin_options" role="group">
      <button type="button" id="asu_isearch_show_managers" class="btn-group btn btn-default toggle-control" value="1"
              data-input-target="show_managers">
        Show Managers at Top of Display
      </button>
    </div>
    <div class="col-md-4 btn-group-vertical asu_isearch_admin_options" role="group">
      <button type="button"
              class="btn-group btn btn-default asu_isearch_tenure_select toggle-control"
              value="Tenure"
              data-input-target="tenure_display[Tenure]">
        Show Tenure
      </button>
    </div>
    <div class="col-md-4 btn-group-vertical asu_isearch_admin_options" role="group">
      <button type="button"
              class="btn-group btn btn-default asu_isearch_tenure_select toggle-control"
              value="Non-Tenure"
              data-input-target="tenure_display[Non-Tenure]">
        Show Non-Tenure
      </button>
    </div>
    <div class="col-md-4 btn-group-vertical asu_isearch_admin_options" role="group">
      <button type="button" id="asu_isearch_show_filters" class="btn-group btn btn-default toggle-control" value="1"
              data-input-target="show_filters">
        Show Filters
      </button>
    </div>
  </div>

  <div class="row asu_isearch_employee_type_select">
    <h4>Select employee types to
      <div class="btn-group asu_isearch_admin_exclude" role="group">
        <button type="button" class="btn-group btn btn-primary include toggle-control" value="0" data-input-target="employee_type_exclude">Show</button>
        <button type="button" class="btn-group btn btn-default exclude toggle-control" value="1" data-input-target="employee_type_exclude">Hide</button>
      </div>
    </h4>

    <?php for ($count = 0; $count < 3; $count++): ?>

      <?php foreach ($settings['employee_types_col_' . $count] as $type): ?>
        <div class="col-md-4 btn-group-vertical">
          <button type="button"
                  class="btn-group btn btn-default toggle-control asu_isearch_type_select"
                  value="<?php print $type['node']['name']; ?>"
                  data-input-target="employee_types[<?php print $type['node']['name']; ?>]">
            <?php print $type['node']['name']; ?>
          </button>
        </div>
      <?php endforeach; ?>

    <?php endfor; ?>
  </div>

  <div class="row">
    <a type="button" role="link" class="btn dept-mgr-btn"
       href="https://isearch.asu.edu/asu_dept_mgr" target="_blank">Go To ASU
      Directory Admin Tool</a>
  </div>

  <div class="asu_isearch_toggle_subdirectory">
    <h4>Showing this department +
      <div class="btn-group asu_isearch_admin_depts" role="group">
        <button type="button" id="asu_isearch_show_subs" class="btn btn-default toggle-control" value="1" data-input-target="include_subdepts">Display Sub-Departments?
        </button>
      </div>
    </h4>
  </div>
</div>