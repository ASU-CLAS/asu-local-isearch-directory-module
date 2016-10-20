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

            $(document).ready(function () {
                var profile_photo_pane = $(".node-asu-isearch-profile .profile-header .isearch-profile-photo");

                if (profile_photo_pane.length && $.trim(profile_photo_pane.html()) == '') {
                    profile_photo_pane.hide();
                }

                // fix pagination/exposed filters when isearch found within tabs
                /*
                $('.ui-tabs-panel .asu-isearch-directory-pane').each(function () {

                    var tab = $(this).closest('.ui-tabs-panel');
                    var tab_hash = '#' + tab.attr('id');

                    // detect pagination
                    var pager = $(this).find('.item-list .pager');
                    if (pager.length) {
                        pager.find('li a').each(function () {
                            var href = $(this).attr('href');
                            $(this).attr('href', href + tab_hash);
                        });
                    }

                    // detect exposed filter
                    /*
                    var filters = $(this).find('.isearch-directory-filters');
                    if (filters.length) {
                        var action = filters.attr('action');
                        filters.attr('action', action + tab_hash);

                        if (settings.chosen) {
                            // taken from chosen module
                            filters.find('select')
                                .not('#field-ui-field-overview-form select, #field-ui-display-overview-form select, .wysiwyg, .draggable select[name$="[weight]"], .draggable select[name$="[position]"], .chosen-disable, .chosen-processed')
                                .filter(function () {
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
                                .once('chosen', function () {
                                    if (typeof $(this).chosen === 'function') {
                                        var chosen_options = Drupal.settings.chosen.options;
                                        chosen_options.width = '100%';
                                        $(this).chosen(chosen_options);
                                    }
                                });
                        }

                    }
                });*/

                var isearch_index = $('.isearch-a-z-index');

                $('.isearch-a-z-index').each(function () {
                    if ($(this).parents('div.ui-tabs-panel').length) {
                        var tab_id = $(this).parents('div.ui-tabs-panel').attr('id');

                        $(this).find('li a').each(function () {
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
