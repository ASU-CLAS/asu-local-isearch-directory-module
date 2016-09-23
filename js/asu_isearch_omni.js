/**
 * ASU iSearch module
 *
 * Custom behavior for iSearch admin
 *
 * @author Sebastian Nievas ( snievas@asu.edu )
 */

(function ($) {
  Drupal.behaviors.asu_isearch_omni = {
    attach: function (context, settings) {
      $('.isearch-directory-filters').each(function(){
        var omnisearch = $(this).find('.form-item-isearch-omni-search');

        if (omnisearch.length) {
          omnisearch.find('input').change(function(){
            terms = $(this).val();
            omnisearch.siblings('.omni-search-field').val(terms);
          });
        }
      });
    }
  }
})(jQuery);