google.load('visualization', '1', {packages: ['corechart']});
google.load('visualization', '1', {packages: ['table']});

(function ($) {
  $( document ).ready(function() {
    init();
    console.log( "ready!" );
  });

  function init() {
    //google.load('visualization', '1', {packages: ['corechart']});
    //google.load('visualization', '1', {packages: ['table']});
    $('#apply-report-filter').click( function() {
      reportFilterSubmit();
    });

    if ($('#intel-report-container').length > 0) {
      var data = {
        return_type: 'json'
      };
      var url = ('https:' == document.location.protocol) ? 'https://' : 'http://';
      url += Drupal.settings.intel.config.cmsHostpath + $('#intel-report-container').attr("data-q"); //?callback=?";
      if ($('#intel-report-container').attr("data-refresh")) {
        data.refresh = 1;
      }
      if ($('#intel-report-container').attr("data-dates")) {
        data.dates = $('#intel-report-container').attr("data-dates");
      }
      jQuery.ajax();
      jQuery.getJSON(url, data, function (data) {
        $("#intel-report-container").replaceWith(data.report);
        l10iCharts.init();
      });
    }
  }

  intelReport = {
    attach: function (context) {
      $('#apply-report-filter').click( function() {
        reportFilterSubmit();
      });

      if ($('#intel-report-container').length > 0) {
        var data = {
          return_type: 'json'
        };
        var url = ('https:' == document.location.protocol) ? 'https://' : 'http://';
      url += Drupal.settings.intel.config.cmsHostpath + $('#intel-report-container').attr("data-q"); //?callback=?";
        if ($('#intel-report-container').attr("data-refresh")) {
          data.refresh = 1;
        }
        if ($('#intel-report-container').attr("data-dates")) {
          data.dates = $('#intel-report-container').attr("data-dates");
        }
        jQuery.ajax();
        jQuery.getJSON(url, data, function(data) {
          $("#intel-report-container").replaceWith(data.report);
          l10iCharts.init();
        });
      }
    }
  };

  function reportFilterSubmit() {
    var loc = location.href;
    var a = loc.split("?");
    var loc = a[0];
    var query = '';
    var v = $('#page-path').val();
    if ((v != undefined) && (v != '')) {
      query += ((query != '') ? '&' : '') + 'page=' + $('#page-mode').val() + ':' + v; //encodeURIComponent(v);
    }
    v = $('#event-filter').val();
    if ((v != undefined) && (v != '')) {
      query += ((query != '') ? '&' : '') + 'event=' + v;
    }
    v = $('#referrer-type').val();
    if ((v != undefined) && (v != '')) {
      query += ((query != '') ? '&' : '') + 'referrer=' + v + ':' + $('#referrer-value').val();
    }
    v = $('#location-type').val();
    if ((v != undefined) && (v != '')) {
      query += ((query != '') ? '&' : '') + 'location=' + v + ':' + $('#location-value').val();
    }
    v = $('#visitor-type').val();
    if ((v != undefined) && (v != '')) {
      query += ((query != '') ? '&' : '') + 'visitor=' + v + ':' + $('#visitor-value').val();
    }
    v = $('#visitor-attr-type').val();
    if ((v != undefined) && (v != '')) {
      query += ((query != '') ? '&' : '') + 'visitor-attr=' + v + ':' + $('#visitor-attr-value').val();
    }
    if (query != '') {
      loc = loc + '?' + query;
    }
    window.location.href = loc;
  }
})(jQuery);