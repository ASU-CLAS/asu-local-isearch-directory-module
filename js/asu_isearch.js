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

            if (settings.chosen) {
              // taken from chosen module
              filters.find('select')
                .not('#field-ui-field-overview-form select, #field-ui-display-overview-form select, .wysiwyg, .draggable select[name$="[weight]"], .draggable select[name$="[position]"], .chosen-disable, .chosen-processed')
                .filter(function() {
                  // Filter out select widgets that do not meet the minimum number of
                  // options.
                  var minOptions = $(this).attr('multiple') ? settings.chosen.minimum_multiple : settings.chosen.minimum_single;
                  if (!minOptions) {
                    // Zero value means no minimum.
                    return true;
                  }
                  else {
                    return $(this).find('option').length >= minOptions;
                  }
                })
                .once('chosen', function() {
                  if (typeof $(this).chosen === 'function') {
                    var chosen_options = Drupal.settings.chosen.options;
                    chosen_options.width = '100%';
                    $(this).chosen(chosen_options);
                  }
                });
            }
            
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

        /* ===========================================================
        * Patch
        * -- issue: pagination/filters persist through each tab.
        * When a not active tab is clicked and a filter has been selected, reset the filters
        *
        * - tthai1@asu.edu
        */

        // get each tabs content, add click listener
        $('.ui-tabs-nav .ui-corner-top:not(.ui-tabs-selected) a').each(function(){
          this.onclick=function(){
            if(!$(this).hasClass('ui-tabs-selected')){
              // trigger the reset if a non-active tab is clicked
              tabReset();
            }
          }
        });

        // reset logic
        function tabReset() {
          // reset iff a filter has been changed. If no filters are changed, do not call reset
          // when a filter is used, the url will have a query ?
          $('.ui-tabs-panel .asu-isearch-directory-pane').each(function() {
            // get the url
            var currentUrl = $(location).attr('href');

            // if a query is present, trigger the reset
            if(currentUrl.toLowerCase().indexOf("\?") >= 0){
              $('#views-exposed-form-isearch-directory-isearch-directory-list input#edit-reset').each(function(index){
                // get all the values of the filters

                // only the clicked tab will have height
                if($(this).height()) {
                  // triggers the reset submit
                  $(this).trigger('click');
                }
              });
            }
          });
        }
        /* ----------------------------------------
        * -- end of patch
        */
      });
    }
  }
})(jQuery);
