(function( $ ) {
	'use strict';

	$( window ).load(function() {
	  init();
	});

	function init() {
		var id, $this;
		$(".fieldset-panel").each(function(index) {
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

			$('#fieldset-panel-' + i).on('shown.bs.collapse', function(event) {
				console.log(i);
				$('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-bottom').removeClass('glyphicon-triangle-right');
			});
			$('#fieldset-panel-' + i).on('hidden.bs.collapse', function(event) {
				console.log(i);
				$('.collapsible-fieldset-icon-' + i).addClass('glyphicon-triangle-right').removeClass('glyphicon-triangle-bottom');
			});

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
