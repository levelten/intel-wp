(function( $ ) {
	'use strict';

	var ths = {};

	/**
	 * All of the code for your admin-facing JavaScript source
	 * should reside in this file.
	 *
	 * Note: It has been assumed you will write jQuery code here, so the
	 * $ function reference has been prepared for usage within the scope
	 * of this function.
	 *
	 * This enables you to define handlers, for when the DOM is ready:
	 *
	 * $(function() {
	 *
	 * });
	 *
	 * When the window is loaded:
	 *
	 * $( window ).load(function() {
	 *
	 * });
	 *
	 * ...and/or other possibilities.
	 *
	 * Ideally, it is not considered best practise to attach more than a
	 * single DOM-ready or window-load handler for a particular page.
	 * Although scripts in the WordPress core, Plugins and Themes may be
	 * practising this, we should strive to set a better example in our own work.
	 */
	$(document).ready(function(){
		ths.form_init();
		/*
		var width = (jQuery(window).width() * 90)/100;
		var height =(jQuery(window).height() * 90)/100;
		$(".thickbox.modal-url").each(function(){
			jQuery(this).attr("href",jQuery(this).attr("href")+"?TB_iframe=true&width="+width+"&height="+height)
		})

		$('.ls-modal').on('click', function(e){
			e.preventDefault();
			$('#myModal').modal('show').find('.modal-body').load($(this).attr('href'));
		});
		*/
	});

	ths.form_init = function () {
		// transform field descriptions into tooltips
		var $description, $label;
		$('.form-item').each(function (index, value) {
			$description = $('.description', this);
			if ($description.length) {
				$label = $('.control-label', this);
				$label.after(' <a href="javascript:void(0)" data-toggle="tooltip" data-placement="bottom" title="' + $description.text() + '"><span class="glyphicon glyphicon-question-sign" aria-hidden="true"></span></a>');
				$description.hide();
			}
		});
	};

})( jQuery );
