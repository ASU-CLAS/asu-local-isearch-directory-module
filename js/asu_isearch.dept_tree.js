/**
 * ASU Directory module
 * Department Hierarchy Block JavaScript
 *
 * Provides AJAX behavior, pagination, modal, and sort for Department block
 *
 * @author Colton Testamarck (colt@asu.edu)
 * @author Robert Jenkins ( rjenkins@eaglecrk.com )
 * @author Michael Samuelson ( mlsamuel@asu.edu / mlsamuelson@gmail.com )
 * @author Sebastian Nievas ( snievas@asu.edu )
 */

(function ($) {
  Drupal.behaviors.asu_isearch_dept_tree = {
    attach: function (context, settings) {
      if (settings.hasOwnProperty('asu_isearch_dept_tree')) {

        settings = settings.asu_isearch_dept_tree;

        var $title = jQuery('#edit-title');
        var top_nid = null;
        var department = 'ASU';
        var whole_tree = false;
        var top_level_ids = settings.top_level_ids;
        var dept_nids = [top_level_ids.top_level_nid];
        var standalone = false;
        var config_target = false;

        if (settings.hasOwnProperty('dept_nids')) {
          dept_nids = settings.dept_nids;
        }

        if (settings.hasOwnProperty('standalone')) {
          standalone = settings.standalone;
        }

        if (settings.hasOwnProperty('config_target')) {
          config_target = settings.config_target;
        }

        if (settings.hasOwnProperty('field_configs') && settings.field_configs.hasOwnProperty('dept_id')) {
          department = settings.field_configs.dept_id;
        } else {
          department = settings.top_level_psid;
        }

        //Show the entire tree if whole_tree is set to true
        if (settings.hasOwnProperty('whole_tree')) {
          whole_tree = settings.whole_tree;
        }

        //if we want the whole tree, use the top level (ASU),
        //else, the stored root department will be used
        if (whole_tree) {
          top_nid = top_level_ids.top_level_nid;
        } else {
          top_nid = dept_nids[0];
        }

        var $people = $('#people');
        var tree = [];

        if (( top_nid != null) && ( tree = JSON.parse(settings.tree))) {

          // Set the root of the tree to the point defined by id -> set by asu_directory.module
          temp = [];
          temp.push(asu_isearch_dept_find_root(tree, top_nid));
          tree = temp;

          $people.data.tree_nids = asu_isearch_dept_get_tree_ids(asu_isearch_dept_find_root(tree, dept_nids[0]));
        }

        if (tree != null && tree.length > 0) {

          // Build Department Hierarchy tree list for display in block
          $('#treediv').tree({
            closedIcon: $('<span class="fa fa-plus-circle"></span>'),
            openedIcon: $('<span class="fa fa-minus-circle"></span>'),
            data: tree,
            // First level open
            autoOpen: 0,
            selectable: true,
            // Assign dept_id attribute to each tree <li>
            onCreateLi: function (node, $li) {
              $li.attr('dept_nid', node.dept_nid);
              $li.attr('dept_id', node.dept_id);

              if (!node.hasChildren()) {
                $li.find('.jqtree-element').prepend('<span class="jqtree-folder-icon fa fa-bookmark"></span>');
              }
            }
          });

          if (config_target !== false && $(config_target).val() !== undefined) {
            department = 'ASU';
            $config = JSON.parse($(config_target).val());
            $depts = $('#isearch_departments');

            $depts.data.dept_id = $config.dept_id;
            $depts.data.dept_nid = $config.dept_nid;
            $depts.data.tree_nids = $config.tree_nids;

            department = $depts.data.dept_id;

            $('#treediv').bind('tree.click', function (event) {

              if ($depts.data.dept_nid == event.node.dept_nid) {
                return false;
              }

              $depts.data.dept_nid = event.node.dept_nid;
              deptnid = event.node.dept_nid;

              // set and save to Dept. ID, so that we can save and recreate tree
              $depts.data.dept_id = event.node.dept_id;
              $depts.data.tree_nids = asu_isearch_dept_get_tree_ids(asu_isearch_dept_find_root(tree, deptnid));

              $(config_target).val(JSON.stringify({
                'dept_id': $depts.data.dept_id,
                'dept_nid': $depts.data.dept_nid,
                'tree_nids': $depts.data.tree_nids
              }));
            });

            // If previous department was saved, open tree to that dept.
            if (department != '') {
              var $tree = $('#treediv');
              var node = $tree.tree('getNodeById', department);
              $tree.tree('selectNode', node, true);
            }
          }
        }
        else {
          $('#block-asu-dept-custom-info-asu-dept-custom-info-dept-hrc').find('.content').html('Error: Failed to load department data. If this error persists, please contact a site administrator.');
        }
      }
    }
  }
})(jQuery);

/**
 * Returns the node (and all children) where (node.tid == @param tid)
 * @param {Object} data
 *  Nested JSON object with department data
 * @param {integer} dept_id
 *  ID of the department that should be the root of the hierarchy
 */
function asu_isearch_dept_find_root(data, dept_id) {
  var success = null;
  for (var i = 0; i < data.length; i++) {
    if (success == null) {
      if (data[i].dept_nid == dept_id) {
        return data[i];
      }
      else if (data[i].hasOwnProperty('children')) {
        success = asu_isearch_dept_find_root(data[i].children, dept_id);
      }
    }
    else {
      break;
    }
  }
  return success;
}

/**
 * Saves the ids of all departments under currently selected tree on #people's data object
 * @param {object}
 *  Nested JSON object with department data
 */
function asu_isearch_dept_get_tree_ids(tree, tree_ids) {

  if (arguments.length == 1) {
    tree_ids = [];
  }

  tree_ids.push(tree.dept_nid);

  for (var i = 0; i < tree.children.length; i++) {
    asu_isearch_dept_get_tree_ids(tree.children[i], tree_ids);
  }

  return tree_ids;

}
