var intel_admin_config_intel_event_edit = (function( $ ) {
	'use strict';


	var ths = {};

	$(document).ready(function(){
		ths.handleMode();
		$('#edit-mode').on('change', function (e) {
			ths.handleMode();
		});
	});

	ths.handleMode = function handleMode () {
		var mode = $('#edit-mode option:selected').val();
		console.log(mode);
		if (mode == 'goal') {
			$('div.form-item-ga-id').show();
		}
		else {
			$('div.form-item-ga-id').hide();
		}

	};

})( jQuery );
