/**
 * ASU iSearch module
 *
 * Custom behavior for iSearch admin
 *
 * @author Sebastian Nievas ( snievas@asu.edu )
 */

(function ($) {
  Drupal.behaviors.asu_isearch = {
    attach: function (context, settings) {
      $(document).ready(function() {
        var profile_photo_pane = $(".node-asu-isearch-profile .profile-header .isearch-profile-photo");

        if (profile_photo_pane.length && $.trim(profile_photo_pane.html()) == '') {
          profile_photo_pane.hide();
        }

        // fix pagination/exposed filters when isearch found within tabs
        $('.ui-tabs-panel .asu-isearch-directory-pane').each(function() {

          var tab = $(this).closest('.ui-tabs-panel');
          var tab_hash = '#' + tab.attr('id');

          // detect pagination
          var pager = $(this).find('.item-list .pager');
          if (pager.length) {
            pager.find('li a').each(function(){
              var href = $(this).attr('href');
              $(this).attr('href', href + tab_hash);
            });
          }

          // detect exposed filters
          var filters = $(this).find('.isearch-directory-filters');
          if (filters.length) {
            var action = filters.attr('action');
            filters.attr('action', action + tab_hash);

            filters.find('select').each(function(){
              if (typeof $(this).chosen === 'function') {
                $(this).chosen({ width: '100%' });
              }
            });
          }
        });

        var isearch_index = $('.isearch-a-z-index');

        $('.isearch-a-z-index').each(function() {
          if ($(this).parents('div.ui-tabs-panel').length) {
            var tab_id = $(this).parents('div.ui-tabs-panel').attr('id');

            $(this).find('li a').each(function(){
              var href = $(this).attr('href');
              if (href.search('#') == -1) {
                $(this).attr('href', href + '#' + tab_id);
              }
            });
          }
        });
      });
    }
  }
})(jQuery);
