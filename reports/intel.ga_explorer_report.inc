<?php
/**
 * @file
 * utility to help fiddle with Google Analytics API requests
 * 
 * @author Tom McCracken <tomm@getlevelten.com>
 */

function intel_ga_explorer_report() {
  $request = get_option('intel_ga_explorer_request', '');
intel_d($request);
  $options = array('' => Intel_Df::t('- None -'));
  $presets = intel_ga_explorer_presets();
  foreach ($presets AS $name => $arr) {
    $options[$name] = $arr['title'];
  }

  $form['markup00'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="panel panel-default"><h3 class="panel-heading">' . Intel_Df::t('Request form') . '</h3><div class="panel-body">',
  );

  $form['preset'] = array(
    '#type' => 'select',
    '#title' => Intel_Df::t('Preset'),
    '#options' => $options,
    '#default_value' => (isset($request['preset_name'])) ? $request['preset_name']: '',
  );
  if (!empty($request['preset_name'])) {
    $items = array();
    $preset = $presets[$request['preset_name']];
    unset($preset['title']);
    foreach ($preset AS $key => $value) {
      $item = $key . ': ';
      if (is_array($value)) {
        $item .= implode(',', $value);
      }
      else {
        $item .= $value;
      }
      $items[] = $item;
    }
    $form['preset_data'] = array(
      '#type' => 'item',
      '#title' => Intel_Df::t('Preset'),
      '#markup' => implode("<br />", $items),
    );
  }


  $form['markup1'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="row"><div class="col-md-7">',
  );
  $form['dimensions'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('dimensions'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#default_value' => (isset($request['custom']['dimensions'])) ? implode(',', $request['custom']['dimensions']) : '',
    '#maxlength' => 200,
   );
  $form['metrics'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('metrics'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#default_value' => (isset($request['custom']['metrics'])) ? implode(',', $request['custom']['metrics']) : '',
    '#maxlength' => 200,
  );
  $form['segment'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('segment'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#default_value' => (isset($request['custom']['segment'])) ? $request['custom']['segment'] : '',
    '#rows' => 2,
  );
  $form['filters'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('filters'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#default_value' => (isset($request['custom']['filters'])) ? $request['custom']['filters'] : '',
    '#rows' => 2,
  );
  $form['sort_metric'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('sort'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#default_value' => (isset($request['custom']['sort_metric'])) ? $request['custom']['sort_metric'] : '',
  );
  $form['markup2'] = array(
    '#type' => 'markup',
    '#markup' => '</div><div class="col-md-5">',
  );
  $form['start_date'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('start date'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#size' => 16,
    '#default_value' => (isset($request['custom']['start_date'])) ? Date("Y-m-d", $request['custom']['start_date']) : Date("Y-m-d", strtotime("-7 days")),
  );
  $form['end_date'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('end date'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#size' => 16,
    '#default_value' => (isset($request['custom']['end_date'])) ? Date("Y-m-d", $request['custom']['end_date']) : Date("Y-m-d"),
  );
  $form['max_results'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('max results'),
    //'#description' => Intel_Df::t('Enter a value to assign to the event when the CTA is clicked. Must be an whole number.'),
    '#size' => 16,
    '#default_value' => (isset($request['custom']['max_results'])) ? $request['custom']['max_results'] : 50,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  $form['markup3'] = array(
    '#type' => 'markup',
    '#markup' => '</div></div>',
  );
  $form['markup01'] = array(
    '#type' => 'markup',
    '#markup' => '</div></div>',
  );
  if (!empty($request)) {
    //$req = drupal_array_merge_deep($request['preset'], $request['custom']);
    $def = array();

    $req = $request['preset'] + $request['custom'];
    $req['dimensions'] = !empty($request['preset']['dimensions']) ? $request['preset']['dimensions'] : array();
    if (!empty($request['custom']['dimensions'])) {
      $req['dimensions'] = array_merge($req['dimensions'], $request['custom']['dimensions']);
    }
    $req['metrics'] = !empty($request['preset']['metrics']) ? $request['preset']['metrics'] : array();
    if (!empty($request['custom']['metrics'])) {
      $req['metrics'] = array_merge($req['metrics'], $request['custom']['metrics']);
    }

    if (intel_is_debug()) {
      intel_d($req);//
    }

    if (!empty($req['metrics']) || !empty($req['dimensions'])) {
      $items = array(
        'dimensions: ' . implode(',', $req['dimensions']),
        'metrics: ' . implode(',', $req['metrics']),
        'segment: ' . $req['segment'],
        'filters: ' . $req['filters'],
        'sort: ' . $req['sort_metric'],
        'start_date: ' . $req['start_date'],
        'end_date: ' . $req['end_date'],
        'max_results: ' . $req['max_results'],
      );
      $form['report_req'] = array(
        '#type' => 'markup',
        '#markup' => '<br><div class="panel panel-default"><h3 class="panel-heading">Request</h3><div class="panel-body">' . implode("<br>\n", $items) . '</div></div>',
      );

      $options = array('refresh' => TRUE);
      //$data = google_analytics_reports_api_report_data($req, $options);
      $data = intel_ga_api_data($req, $options);

      if (intel_is_debug()) {
        intel_d($data);//
      }

      $header = array();
      $rows = array();
      $data_rows = intel_get_ga_feed_rows($data);
      $cols = array();
      intel_d($data_rows);//
      if (!empty($data_rows) && is_array($data_rows)) {
        foreach ($data_rows AS $r) {
          $row = array();
          if (!count($header)) {
            foreach ($r AS $k => $e) {
              $header[] = $k;
            }
          }
          foreach ($r AS $k => $e) {
            $row[] = $e;
          }
          $rows[] = $row;
        }
      }
      $row = array();
      $data_totals = intel_get_ga_feed_totals($data);
      foreach ($header as $r) {
        $row[] = isset($data_totals[$r]) ? "<strong>" . $data_totals[$r] . "</strong>" : '';
      }

      $rows[] = $row;
      $vars = array(
        'header' => $header,
        'rows' => $rows,
        'empty' => Intel_Df::t('No results found.'),
      );
      $form['report'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="panel panel-default"><h3 class="panel-heading">Results</h3><div>' . Intel_Df::theme('table', $vars) . '</div></div>',
      );
    }
  }

  return $form;
}

function intel_ga_explorer_report_submit($form, &$form_state) {
  $values = $form_state['values'];

  $presets = intel_ga_explorer_presets();
  $request = array(
    'preset' => array(),
    'custom' => array(),
    'preset_name' => '',
  );
  if (!empty($values['preset'])) {
    $request['preset_name'] = $values['preset'];
    $request['preset'] = $presets[$values['preset']];
    unset($request['preset']['title']);
  }

  $request['custom'] = array(
    'dimensions' => array(),
    'metrics' => array(),
    'segment' => '',
    'filters' => '',
    'sort_metric' => '',
    'start_date' => '',
    'end_date' => '',
    'max_results' => '',
  );
  $d = $values['dimensions'];
  $d = str_replace("'", '', $d);
  $d = explode(',', $d);
  foreach ($d AS $i => $e) {
    if (!empty($e)) {
      $request['custom']['dimensions'][] = trim($e);
    }
  }
  $d = $values['metrics'];
  $d = str_replace("'", '', $d);
  $d = explode(',', $d);
  foreach ($d AS $i => $e) {
    if (!empty($e)) {
      $request['custom']['metrics'][] = trim($e);
    }
  }
  $request['custom']['segment'] = str_replace("'", '', $values['segment']);
  $request['custom']['filters'] = str_replace("'", '', $values['filters']);
  $request['custom']['sort_metric'] = str_replace("'", '', $values['sort_metric']);
  $request['custom']['start_date'] = strtotime(str_replace("'", '', $values['start_date']));
  $request['custom']['end_date'] = strtotime(str_replace("'", '', $values['end_date']));
  $request['custom']['max_results'] = str_replace("'", '', $values['max_results']);
intel_d($request);
  update_option('intel_ga_explorer_request', $request);
}

function intel_ga_explorer_presets() {
  $presets = array();

  // works for date
  $presets['pageviews'] = array(
    'title' => Intel_Df::t('Pageviews (no dimensions)'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:pageviewsPerSession', 'ga:timeOnPage', 'ga:exits', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:pageValue'),
  );

  // note goalValues lost when pagePath dimension used, calc goalValueAll using pageValue * uniquePageviews
  // using filter: ga:pagePath=@/ used to correct this, but no longer works
  $presets['pageviews_content_list'] = array(
    'title' => Intel_Df::t('Pageviews > content list'),
    'dimensions' => array('ga:pagePath'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:exits', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:pageValue'),
  );

  // goalValues not reported when pagePath filter used
  $presets['pageviews_content_filter'] = array(
    'title' => Intel_Df::t('Pageviews > content filter'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:exits', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:pageValue'),
    //'filters' => 'ga:pagePath==/blog/ian-whitcomb/brand-publisher-style-guides'
  );

  $presets['pageviews_content_list_pa'] = array(
    'title' => Intel_Df::t('Pageviews > content list > pa filter'),
    'dimensions' => array('ga:pagePath'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:exits', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:pageValue'),
    //'filters' => 'ga:customVarValue1=@&a=6&'
  );

  // goalValue provides in seg completed goals numbers, ? on any goal
  // estimate pageValueAll
  $presets['pageviews_pa_list'] = array(
    'title' => Intel_Df::t('Pageviews > pa  list'),
    'dimensions' => array('ga:customVarValue1'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:exits', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:pageValue'),
    //'filters' => 'ga:customVarValue1=@&a='
  );

  $presets['pageviews_pa_list2'] = array(
    'title' => Intel_Df::t('Pageviews > pa  list2'),
    'dimensions' => array('ga:customVarValue1', 'ga:goalCompletionLocation'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:pageviewsPerSession', 'ga:timeOnPage', 'ga:exits', 'ga:pageValue', 'ga:goalValueAll', 'ga:goalValuePerSession'),
    //'filters' => 'ga:customVarValue1=@&a=6&'
  );

  // - cannot sum pageValue for totals
  $presets['pageviews_pa_list'] = array(
    'title' => Intel_Df::t('Pageviews > pa  list'),
    'dimensions' => array('ga:customVarValue1'),
    'metrics' => array('ga:pageviews', 'ga:uniquePageviews', 'ga:pageviewsPerSession', 'ga:timeOnPage', 'ga:exits', 'ga:pageValue', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
    //'filters' => 'ga:customVarValue1=@&a=6&'
  );


  //
  $presets['entrances'] = array(
    'title' => Intel_Df::t('Entrances (no dimensions)'),
    'metrics' => array('ga:entrances', 'ga:newUsers', 'ga:pageviewsPerSession', 'ga:timeOnSite', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
    //'metrics' => array('ga:entrances', 'ga:pageviewsPerSession', 'ga:timeOnSite', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
  );




  $presets['entrances_works'] = array(
    'title' => Intel_Df::t('Entrances (landingPagePath)'),
    'dimensions' => array('ga:landingPagePath'),
    'metrics' => array('ga:entrances', 'ga:newVisits', 'ga:pageviewsPerSession', 'ga:timeOnSite', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
  );


  $presets['pageviews_pageattr_list'] = array(
    'title' => Intel_Df::t('Pageviews (Page Attr list)'),
    'dimensions' => array('ga:customVarValue1'),
    'metrics' => array('ga:pageviews', 'ga:pageviewsPerSession', 'ga:timeOnPage', 'ga:exits', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
    //'segment' => 'dynamic::ga:customVarValue1=@&a=6&',
    //'filters' => 'ga:customVarValue1=@&og=53&',
  );

  // ?goalValueAll = ga:entrances x ga:pageValue
  // filter: ga:customVarValue1=@&og=53&;entrances>0
  /*
  $presets['entrances_pageattr_list'] = array(
    'title' => Intel_Df::t('Entrances (Page Attr list)'),
    'dimensions' => array('ga:landingPagePath', 'ga:customVarValue1'),
    'metrics' => array('ga:entrances', 'ga:newVisits', 'ga:pageviewsPerSession', 'ga:timeOnSite', 'ga:bounces', 'ga:pageValue', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
    //'filters' => 'ga:entrances>0',
    'filters' => 'ga:customVarValue1=@&og=53&;ga:entrances>0',
  );
  */



  // this works, but as soon as you add ga:customVarValue1 it breaks. Adding ga:customVarValue1 makes pageviews revert to hit mode
  $presets['entrances_pa_list'] = array(
    'title' => Intel_Df::t('Entrances  > pa  list'),
    'dimensions' => array('ga:landingPagePath'),
    //'dimensions' => array('ga:landingPagePath', 'ga:customVarValue1'),
    'metrics' => array('ga:entrances', 'ga:newVisits', 'ga:pageviewsPerSession', 'ga:timeOnSite', 'ga:bounces', 'ga:pageValue', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
    //'segment' => 'sessions::sequence::^ga:customVarValue1=@&a=',
    //'filters' => 'ga:customVarValue1=@&a=6&;ga:entrances>0',
  );



  // ?goalValueAll = ga:entrances x ga:pageValue
  // filter: ga:customVarValue1=@&og=53&;entrances>0
  $presets['entrances_scorecard_date'] = array(
    'title' => Intel_Df::t('Entrances Scorecard Date'),
    'dimensions' => array('ga:date'),
    'metrics' => array('ga:entrances', 'ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
  );


  // ?goalValueAll = ga:entrances x ga:pageValue
  // filter: ga:customVarValue1=@&og=53&;entrances>0
  $presets['entrances_scorecard_date_pageattr'] = array(
    'title' => Intel_Df::t('Entrances Scorecard Date (filter: Page Attr)'),
    'dimensions' => array('ga:date'),
    'metrics' => array('ga:entrances', 'ga:newVisits', 'ga:pageviewsPerSession', 'ga:timeOnPage', 'ga:bounces', 'ga:pageValue', 'ga:goalValueAll', 'ga:goalCompletionsAll'),
    //'filters' => 'ga:entrances>0',
    'filters' => 'ga:customVarValue1=@&og=53&;ga:entrances>0',
  );


  $presets['clickstream'] = array(
    'title' => Intel_Df::t('Clickstream'),
    'dimensions' => array('ga:customVarValue5', 'ga:sessionCount', 'ga:customVarValue4'),
    'metrics' => array('ga:entrances', 'ga:pageviews', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:uniquePageviews', 'ga:pageValue'),
    'sort_metric' => 'ga:customVarValue5,ga:customVarValue4',
  );

  return $presets;
}

