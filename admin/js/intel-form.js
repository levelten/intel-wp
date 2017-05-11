(function( $ ) {
	'use strict';

	$( window ).load(function() {
	  init();
	});

	function init() {
		var id, $this;
		$(".bootstrap-wrapper .fieldset-panel").each(function(index) {
			$this = $(this);

			id = $this.attr('id');
			// strip off 'fieldset-panel-'
			var i = id.substr(15);

			if ($this.hasClass('in')) {
				$('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-bottom');
			}
			else {
				$('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-right');
			}

			$('#fieldset-panel-' + i).first().on('shown.bs.collapse', function(event) {
				// prevents embedded fieldset events from bubbling up.
				event.stopPropagation();
				$('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-bottom').removeClass('glyphicon-triangle-right');
			});
			$('#fieldset-panel-' + i).first().on('hidden.bs.collapse', function(event) {
				// prevents embedded fieldset events from bubbling up.
				event.stopPropagation();
				$('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-right').removeClass('glyphicon-triangle-bottom');
			});
		});

		// transform field descriptions into tooltips
		var $description, $label;
		$('.bootstrap-wrapper .form-item').each(function (index, value) {
			$description = $('.description', this);
			if ($description.length) {
				$label = $('.control-label', this);
				$label.after(' <a href="javascript:void(0)" data-toggle="tooltip" data-placement="bottom" title="' + $description.text() + '"><span class="glyphicon glyphicon-question-sign" aria-hidden="true"></span></a>');
				$description.hide();
			}
		});

		/*
		 $('.fieldset-panel').first().on('shown.bs.collapse', function(event) {
			 console.log(this);
			 var $this = $(this);
			 var id = $this.attr('id');
			 var i = id.substr(15);
		   $('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-bottom').removeClass('glyphicon-triangle-right');
		 });
		$('.fieldset-panel').first().on('hidden.bs.collapse', function(event) {
			var $this = $(this);
			var id = $this.attr('id');
			var i = id.substr(15);
		  $('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-right').removeClass('glyphicon-triangle-bottom');
		});
		*/
	}

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

})( jQuery );
