/**
 * ASU iSearch module
 *
 * Provides panel configuration behavior
 *
 * @author Sebastian Nievas ( snievas@asu.edu )
 */

(function ($) {
  Drupal.behaviors.asu_isearch_dept_config = {
    attach: function (context, settings) {
      show_all_button = $('button.asu_isearch_type_select[value="All"]');

      $('.toggle-control').click(function(){
        toggleSelection($(this));
      });

      // preselect buttons
      $('.asu-isearch-dept-config input:checked').each(function(){
        var target = $('.asu-isearch-dept-config button.toggle-control[data-input-target="' + $(this).attr('name') + '"][value="' + $(this).val() + '"]');
        toggleSelection(target);
      });

      $('.form-type-isearch-dept-config button.asu_isearch_type_select').each(function(){
        $(this).click(function(e){
          var employee_type = $(this).val();

          // determine if we should clear all employee types when show all has been selected
          if (employee_type == 'All') {
            // clear all employee types, including show all
            $('.form-type-isearch-dept-config button.asu_isearch_type_select').each(function(){
              if ($(this) != show_all_button) {
                // remove active class
                var target = $(this).attr('data-input-target');
                $(this).removeClass('btn-primary')
                  .addClass('btn-default');

                // uncheck these employee types
                var input_field = $('input[name="'+ target + '"]');
                input_field.prop('checked', false);
              }
            });
          }

          // by default, we will unset show all when another has been selected.
          if (show_all_button.hasClass('btn-primary')) {
            toggleSelection(show_all_button);
          }

          // if no options have been selected, we will set 'show all' flag
          if ($('.form-item-employee-types input:checked').length == 0) {
            toggleSelection(show_all_button);
          }
        });
      });

    }
  }

  function toggleSelection(el) {
    var target = el.attr('data-input-target');
    var value = el.val();
    var input_field = $('input[name="'+ target + '"][value="' + value + '"]');
    var input_type = input_field.prop('type');

    switch (input_type) {
      case 'radio':
        $('button.toggle-control[data-input-target="' + target + '"]')
          .removeClass('btn-primary')
          .addClass('btn-default');

        el.removeClass('btn-default')
          .addClass('btn-primary');

        input_field.prop('checked', true);
        break;

      case 'checkbox':
        if (el.hasClass('btn-primary')) {
          el.removeClass('btn-primary')
            .addClass('btn-default');
          input_field.prop('checked', false);
        }
        else {
          el.addClass('btn-primary')
            .removeClass('btn-default');
          input_field.prop('checked', true);
        }
        break;
    }
  }
})(jQuery);

