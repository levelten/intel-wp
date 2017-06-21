<?php
/**
 * @file
 * Administration of visitors
 * 
 * @author Tom McCracken <tomm@getlevelten.com>
 */


function intel_util() {
  $output = '';

  $l_options = array(
    'attributes' => array(
      'class' => array(
        'btn btn-info'
      )
    ),
    'query' => array(
      'destination' => Intel_Df::current_path(),
    ),
  );
  $output .= Intel_Df::l(Intel_Df::t('Clear Google Analytics API cache'), 'admin/util/clear_gapi_cache', $l_options);
  return $output;
}

/**
 * Testing function
 */
function intel_util_temp() {
  include_once INTEL_DIR . 'includes/intel.ga.inc';



  $ga_goals = intel_ga_goal_load(null, array('refresh' => 1));

  intel_d($ga_goals);

  return '';
  $goal = array(
    'ga_id' => '8',
    'title' => 'Contact2',
  );
  intel_goal_save($goal);



  return '';
}

/**
 * Form Test function
 */
function intel_util_temp_form($form, &$form_state) {

  $settings = get_option('intel_test_form', array());

  /*
  $form['enable_all'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Fieldset'),
    '#description' => Intel_Df::t('Fieldset description'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  */

intel_d($settings);//

  $page_match = Intel_Df::drupal_match_path($settings['uri'], $settings['enable_pages']);

  intel_d($page_match);//

  $desc = Intel_Df::t('Input your LevelTen Intelligence API key. You can get one at !link',
    array(
      '!link' => Intel_Df::l(Intel_Df::t('api.getlevelten.com'), 'http://api.getlevelten.com/site', array('attributes' => array('target' => '_blank'))),
    )
  );
  $form['uri'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('URI to match'),
    '#default_value' => !empty($settings['uri']) ? $settings['uri'] : '',
    '#description' => $desc,
  );

  $options = array(
    1 => 'Yes',
    0 => 'No',
  );
  $form['enable_all'] = array(
    '#type' => 'radios',
    '#title' => Intel_Df::t('Enabled'),
    '#default_value' => !empty($settings['enable_all']) ? $settings['enable_all'] : '',
    '#options' => $options,
  );

  $form['enable_pages'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('URIs'),
    '#default_value' => !empty($settings['enable_pages']) ? $settings['enable_pages'] : '',
    '#html' => 1,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => Intel_Df::t('Submit'),
    '#prefix' => '<br><br>',
  );

  return $form;
}

/**
 * Form test validate
 */
function intel_util_temp_form_validate($form, &$form_state) {

}

/**
 * Form test validate
 */
function intel_util_temp_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  $defaults = update_option('intel_test_form', $values);
}

function intel_util_environment() {
  if (!intel_is_test() || !intel_is_debug()) {
    return '';
  }
  $info = array(
    '_summary' => array(),
  );
  $info = intel_environment_info($info);
  $json_options = 0x0000;
  $print = '';
  if (!empty($_GET['print'])) {
    $print = $_GET['print'];


  }

  // create summary elements
  $info['_summary']['PHP Version'] = isset($info['php']['Core']['PHP Version']) ? $info['php']['Core']['PHP Version'] : '(not set)';
  $info['_summary']['WP Version'] = isset($info['wp']['wp_version']) ? $info['wp']['wp_version'] : '(not set)';
  $info['_summary']['Intel Version'] = isset($info['intel']['INTEL_VER']) ? $info['intel']['INTEL_VER'] : '(not set)';

  $json = json_encode($info, $json_options);
  $output = "<script>var intel_info_env = $json;</script>";

  if ($print == 'pretty') {
    $json_options = $json_options | JSON_PRETTY_PRINT;
    $json = json_encode($info, $json_options);
  }

  if ($print) {
    $output .= $json;
  }
  else {
    $output .= '<textarea cols="120" rows="20">' . $json . '</textarea>';
  }

  if ($print == 'pretty') {
    //$output = str_replace("\n", "\n<br>", $output);
  }

  return $output;
}

function intel_environment_info($info = array()) {
  $info['php'] = intel_parse_phpinfo();
  $info['wp'] = array();
  $info['intel'] = array();

  $consts = array(
    'HOME_URL',
    'SITE_URL',
    'WPLANG',
  );
  foreach ($consts as $k) {
    $info['wp'][$k] = defined($k) ? constant($k) : '(not set)';
  }

  $globals = array(
    'wp_version',
    'wp_db_version',
  );
  foreach ($globals as $k) {
    $info['wp'][$k] = $GLOBALS[$k];
  }

  $info['wp']['get_plugins'] = get_plugins();
  $info['wp']['wp_get_theme'] = wp_get_theme();


  $consts = array(
    'INTEL_VER',
    'INTEL_DIR',
    'INTEL_URL',
  );
  foreach ($consts as $k) {
    $info['intel'][$k] = defined($k) ? constant($k) : '(not set)';
  }

  return $info;
}

function intel_parse_phpinfo() {
  ob_start(); phpinfo(); $s = ob_get_contents(); ob_end_clean();
  $s = strip_tags($s, '<h2><th><td>');
  $s = preg_replace('/<th[^>]*>([^<]+)<\/th>/', '<info>\1</info>', $s);
  $s = preg_replace('/<td[^>]*>([^<]+)<\/td>/', '<info>\1</info>', $s);
  $t = preg_split('/(<h2[^>]*>[^<]+<\/h2>)/', $s, -1, PREG_SPLIT_DELIM_CAPTURE);
  $r = array(); $count = count($t);
  $p1 = '<info>([^<]+)<\/info>';
  $p2 = '/'.$p1.'\s*'.$p1.'\s*'.$p1.'/';
  $p3 = '/'.$p1.'\s*'.$p1.'/';
  for ($i = 1; $i < $count; $i++) {
    if (preg_match('/<h2[^>]*>([^<]+)<\/h2>/', $t[$i], $matchs)) {
      $name = trim($matchs[1]);
      $vals = explode("\n", $t[$i + 1]);
      foreach ($vals AS $val) {
        if (preg_match($p2, $val, $matchs)) { // 3cols
          $r[$name][trim($matchs[1])] = array(trim($matchs[2]), trim($matchs[3]));
        } elseif (preg_match($p3, $val, $matchs)) { // 2cols
          $r[$name][trim($matchs[1])] = trim($matchs[2]);
        }
      }
    }
  }
  return $r;

}

function intel_util_env_settings_form($form, &$form_state) {

}

function intel_util_clear_gapi_cache() {
  global $wpdb;


  $ga_data_src = intel_ga_data_source();

  if ($ga_data_src == 'gadwp') {
    $sql = "
        SELECT option_name
        FROM {$wpdb->prefix}options
        WHERE option_name LIKE %s
      ";
    $data = array(
      'gadwp_cache_intel_%',
    );

    $results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

    foreach ($results as $row) {
      delete_option($row->option_name);
    }
    delete_option('gadwp_cache_gapi_errors');
    delete_option('gadwp_cache_last_error');
    Intel_Df::drupal_set_message(Intel_Df::t('gawdp_cache cleared'));
  }

  if (!empty($_GET['destination'])) {
    Intel_Df::drupal_goto($_GET['destination']);
    exit;
  }

  return 'OK';
}

function intel_util_test() {
  return Intel_Df::t('Test Utilities');
}

function intel_util_test_webform_page() {
  include_once ( INTEL_DIR . 'includes/class-intel-form.php' );

  $form = Intel_Form::drupal_get_form('intel_util_test_webform_form');
  return $form;
}

/**
 * Form Test function
 */
function intel_util_test_webform_form($form, &$form_state) {

  $account = wp_get_current_user();

  $form['data.givenName'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Given name'),
    '#default_value' => !empty($account->user_firstname) ? $account->user_firstname : Intel_Df::t('Tommy'),
    //'#description' => Intel_Df::t('Input family name.'),
  );

  $form['data.familyName'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Family name'),
    '#default_value' => !empty($account->user_lastname) ? $account->user_lastname : Intel_Df::t('Tester'),
    //'#default_value' => !empty($defaults['test']) ? $defaults['test'] : '',
    //'#description' => Intel_Df::t('Input family name.'),
  );

  $form['data.email'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Email'),
    '#default_value' => !empty($account->user_email) ? $account->user_email : '',
    //'#default_value' => !empty($defaults['test']) ? $defaults['test'] : '',
    //'#description' => Intel_Df::t('Input family name.'),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Advanced'),
    '#description' => Intel_Df::t('Configure the Google Analytics event/goal to trigger upon submission.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $submission_goals = intel_get_event_goal_info('submission');

  $options = array();
  $options[''] = esc_html__( '-- None --', 'wpcf7_intel' );
  $options['form_submission-'] = esc_html__( 'Event: Form submission', 'wpcf7_intel' );
  $options['form_submission'] = esc_html__( 'Valued Event: Form submission!', 'wpcf7_intel' );

  foreach ($submission_goals AS $key => $goal) {
    $options[$key] = esc_html__( 'Goal: ', 'intel') . $goal['goal_title'];
  }
  // Set #tree to group all cf7_intel data in POST
  $form['advanced']['tracking_event_name'] = array(
    '#type' => 'select',
    '#title' => Intel_Df::t('Tracking event'),
    '#options' => $options,
    '#default_value' => !empty($settings['tracking_event_name']) ? $settings['tracking_event_name'] : '',
    //'#description' => $desc,
    //'#size' => 32,
  );

  $form['advanced']['tracking_event_value'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Tracking value'),
    '#default_value' => !empty($settings['tracking_event_value']) ? $settings['tracking_event_value'] : '',
    //'#description' => $desc,
    '#size' => 8,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => Intel_Df::t('Submit'),
    '#prefix' => '<br>',
  );

  return $form;
}

function intel_util_test_webform_form_validate($form, &$form_state) {
  $_SESSION['intel_weform_test']['time0'] = microtime (TRUE);
}

function intel_util_test_webform_form_submit($form, &$form_state) {
  $msg = Intel_Df::t('Form submitted');
  $values = $form_state['values'];
  intel_d($values);//

  // get initialied var structure
  $vars = intel_form_submission_vars_default();

  // create pointer aliases
  $submission = &$vars['submission'];
  $track = &$vars['track'];

  // set visitor properties from webform values
  $vp_info = intel()->visitor_property_info();
  foreach ($values as $k => $v) {
    if (!empty($vp_info[$k])) {
      $vars['visitor_properties'][$k] = $v;
    }
  }
  //$vars['visitor_properties']

  // set type of submission, e.g. gravityform, cf7, webform
  $submission->type = 'intel_form';
  // if form type allows multiple form, set id of form that was submitted
  $submission->fid = $values['form_id'];
  // if form submision creates a submission record, set it here
  $submission->fsid = 0;
  //$submission->submission_uri = "/wp-admin/admin.php?page=gf_entries&view=entry&id={$submission->fid}&lid={$submission->fsid}";
  // set title of form
  $submission->form_title = !empty($form_state['form_title']) ? $form_state['form_title'] : ucwords(str_replace('_', ' ', $values['form_id']));

  if (!empty($values['tracking_event_name'])) {
    $track['name'] = $values['tracking_event_name'];
    if (substr($track['name'], -1) == '-') {
      $track['name'] = substr($track['name'], 0, -1);
      $track['valued_event'] = 0;
    }
    if (!empty($values['tracking_event_value'])) {
      $track['value'] = $values['tracking_event_value'];
    }
  }

  $time1 = microtime(TRUE);
  $msg .= " (t1d=" .  number_format($time1 - $_SESSION['intel_weform_test']['time0'], 3) . 'sec';

  intel_d($vars);//

  // process submission data
  intel_process_form_submission($vars);

  $msg .= ", t2d=" .  number_format(microtime(TRUE) - $time1, 3) . "sec)";

  $msg .= "\n" . intel()->tracker->get_pushes_script();
  intel_d($msg);//
  Intel_Df::drupal_set_message($msg, 'status');

  unset($_SESSION['intel_weform_test']);
}

function intel_util_test_form_page() {
  include_once ( INTEL_DIR . 'includes/class-intel-form.php' );

  $form = Intel_Form::drupal_get_form('intel_util_test_form');
  return $form;
}

/**
 * Form Test function
 */
function intel_util_test_form($form, &$form_state) {

  $defaults = get_option('intel_test_form', array());

  intel_d($defaults); //

  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $type = 'checkbox';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
  );

  $type = 'checkboxes';
  $options = Intel_Df::drupal_map_assoc(array(Intel_Df::t('Yes'), Intel_Df::t('No')));
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
    '#options' => $options,
  );

  $type = 'date';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
  );

  $type = 'file';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
  );

  $type = 'password';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    //'#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
  );

  $type = 'password_confirm';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    //'#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
  );

  $type = 'radio';
  $options = array(
    0 => Intel_Df::t('No'),
    1 => Intel_Df::t('Yes'),
  );
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : 0,
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    //'#field_prefix' => 'field_prefix',
    //'#field_suffix' => 'field_suffix',
    '#options' => $options,
  );

  $type = 'radios';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : 0,
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    //'#field_prefix' => 'field_prefix',
    //'#field_suffix' => 'field_suffix',
    '#options' => $options,
  );

  $type = 'select';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : array(),
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
    '#options' => $options,
  );

  $type = 'textfield';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
  );

  $type = 'textarea';
  $form['fs'][$type] = array(
    '#type' => $type,
    '#title' => Intel_Df::t('title'),
    '#description' => Intel_Df::t('description'),
    '#default_value' => !empty($defaults[$type]) ? $defaults[$type] : '',
    '#prefix' => '<div class="card"><h4 class="card-header">' . $type . '</h4><div class="card-block">',
    '#suffix' => '</div></div>',
    '#field_prefix' => 'field_prefix',
    '#field_suffix' => 'field_suffix',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => Intel_Df::t('Submit'),
    '#prefix' => '<br><br>',
  );

  return $form;
}

/**
 * Form test validate
 */
function intel_util_test_form_validate($form, &$form_state) {

}

/**
 * Form test validate
 */
function intel_util_test_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  intel_d($values); //
  update_option('intel_test_form', $values);
}

function intel_util_test_url_parsing() {
  $urls = array(
    'http://www.alpha.com',
    '//www.beta.com',
    'http://www.alpha.com/blog/alpha',
    '//www.beta.com/blog/beta',
    'http://www.alpha.com/blog/alpha?id=4',
    '//www.beta.com/blog/beta?id=4&view=full',
    'admin/config/intel/settings',
    'mailto:tom@example.com',
    'tel:+1-214-555-1212',
    'urn::post:1',
    ':post:1',
    //'urn::gravityform:1',
    //':gravityform:1',
    'urn:isbn:0451450523',
    'news:comp.unix',
  );

  $urls = apply_filters('intel_test_url_parsing_alter', $urls);

  $vars = array();
  $vars['header'] = array(
    Intel_Df::t('URI src'),
    Intel_Df::t('URI out'),
    Intel_Df::t('Location'),
    Intel_Df::t('Scheme'),
    Intel_Df::t('Host'),
    Intel_Df::t('Path'),
    Intel_Df::t('Query'),
    Intel_Df::t('Fragment'),
  );
  $vars['rows'] = array();
  foreach ($urls as $url) {
    $parse = intel_parse_url($url);
    $url_out = Intel_Df::url($url);
    $vars['rows'][] = array(
      $url,
      $url_out,
      isset($parse['location']) ? $parse['location'] : '(notset)',
      isset($parse['scheme']) ? $parse['scheme'] : '(notset)',
      isset($parse['host']) ? $parse['host'] : '(notset)',
      isset($parse['path']) ? $parse['path'] : '(notset)',
      isset($parse['query']) ? $parse['query'] : '(notset)',
      isset($parse['fragment']) ? $parse['fragment'] : '(notset)',
    );
  }


  $out = Intel_Df::theme('table', $vars);
  return $out;

}

function intel_util_dev() {
  return Intel_Df::t('Development Utilities');
}

function intel_util_reset_site() {
  if (empty($_GET['pw']) || $_GET['pw'] != 'please') {
    return esc_html__('What is the password?', 'intel');
  }
  if (empty($_GET['flags'])) {
    return esc_html__('No flags set', 'intel');
  }

  $flags = $_GET['flags'];

  if ($flags == '_' || $flags == 'installtest') {
    $mode = $flags;
    $flags = 'c,v,f';
    $sys_meta = get_option('intel_system_meta', array());

    // clear setup status/data
    unset($sys_meta['setup_successes']);
    unset($sys_meta['setup_step']);
    update_option('intel_system_meta', $sys_meta);
  }
  $flags = explode(',', $flags);
  global $wpdb;

  $plugins_enabled = array(
    'gadwp' => is_plugin_active( 'google-analytics-dashboard-for-wp/gadwp.php' ),
    'flamingo' => is_plugin_active( 'flamingo/flamingo.php' ),
    'gravityforms' => is_plugin_active( 'gravityforms/gravityforms.php' ),
    'ninja_forms' => is_plugin_active( 'ninja-forms/ninja-forms.php' ),
  );

  if (in_array('test', $flags)) {
    $identifiers = array(
      'email' => 'tomm@getlevelten.com',
    );
    $visitor = intel_visitor_load_by_identifiers($identifiers);
    if (!empty($visitor->vid)) {
      intel_visitor_delete($visitor->vid);
      Intel_Df::drupal_set_message(Intel_Df::t('Tommy Tester deleted.'));
    }
    else {
      Intel_Df::drupal_set_message(Intel_Df::t('Tommy Tester not in database.'));
    }

    // clear visitor sync requests
    update_option('intel_sync_visitor_requests', array());
    update_option('intel_sync_visitor_requests_queue', array());
  }

  // clear comments
  if (in_array('c', $flags)) {
    $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}comments");
    $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}commentmeta");
    Intel_Df::drupal_set_message(Intel_Df::t('Comments truncated.'));
  }

  // clear visitor data
  if (in_array('v', $flags)) {
    $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}intel_visitor");
    $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}intel_visitor_identifier");
    $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}intel_submission");

    //clear out cron data
    delete_option('intel_sync_visitor_requests');
    delete_option('intel_sync_visitor_requests_queue');
    Intel_Df::drupal_set_message(Intel_Df::t('Intel visitors truncated.'));
  }

  // clear form data
  if (in_array('f', $flags)) {
    if (is_plugin_active( 'gravityforms/gravityforms.php' )) {
      $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}rg_incomplete_submissions");
      $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}rg_lead");
      $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}rg_lead_detail");
      $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}rg_lead_detail_long");
      $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}rg_lead_meta");
      $delete = $wpdb->query("TRUNCATE TABLE {$wpdb->prefix}rg_lead_notes");

      Intel_Df::drupal_set_message(Intel_Df::t('Gravity Forms form entries truncated.'));
    }

    if (!empty($plugins_enabled['flamingo'])) {
      // flamingo data is stored as posts.
      $sql = "
        SELECT ID
        FROM {$wpdb->prefix}posts
        WHERE post_type = %s
          OR post_type = %s
          OR post_type = %s
      ";
      $data = array(
        'flamingo_contact',
        'flamingo_inbound',
        'flamingo_outbound',
      );

      $results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

      foreach ($results as $row) {
        wp_delete_post($row->ID);
        //$wpdb->delete("{$wpdb->prefix}posts", array('ID' => $row->ID), array('%d'));
        //$wpdb->delete("{$wpdb->prefix}postmeta", array('post_id' => $row->ID), array('%d'));
        //$ids[] = $row->ID;
      }

      Intel_Df::drupal_set_message(Intel_Df::t('Flamingo contacts and messages truncated.'));
    }

    if (!empty($plugins_enabled['ninja_forms'])) {
      // flamingo data is stored as posts.
      $sql = "
        SELECT ID
        FROM {$wpdb->prefix}posts
        WHERE post_type = %s
      ";
      $data = array(
        'nf_sub',
      );

      $results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

      foreach ($results as $row) {
        wp_delete_post($row->ID);
      }

      Intel_Df::drupal_set_message(Intel_Df::t('Ninja Forms submissions truncated.'));
    }
  }

  $del_options = array(
    'intel_addons_enabled',
    'intel_api_level',
    'intel_apikey',
    'intel_custom_embed_script',
    'intel_debug_ga_debug',
    'intel_debug_mode',
    //'intel_extended_mode',
    'intel_ga_data_source',
    'intel_ga_profile',
    'intel_ga_tid',
    'intel_ga_view',
    'intel_goals',
    'intel_icon_urls',
    //'intel_imapi_url',
    'intel_intel_scripts_enabled',
    'intel_l10iapi_connector',
    'intel_l10ipai_custom_params',
    'intel_l10iapi_js_embed_style',
    'intel_l10iapi_js_ver',
    'intel_l10iapi_url',
    'intel_scorings',
    'intel_sync_visitor_requests',
    'intel_sync_visitordata_fullcontact',
    'intel_system_meta',
    'intel_track_emailclicks',
    'intel_track_phonecalls',
    'intel_track_realtime',
  );

  if (in_array('is', $flags)) {
    if (!empty($plugins_enabled['gadwp'])) {
      // see google-analytics-dashboard-for-wp/install/install.php
      $del_keys = array(
        'ga_dash_tableid_jail' => '',
        'ga_dash_token' => '',
        'ga_dash_profile_list' => array(),
      );
      $gadwp_options = get_option('gadash_options', '{}');
      $gadwp_options = json_decode($gadwp_options, 1);
      foreach ($del_keys as $k => $v) {
        $gadwp_options[$k] = $v;
      }
      update_option('gadash_options', json_encode( $gadwp_options) );

      Intel_Df::drupal_set_message(Intel_Df::t('GADWP options cleared.'));
    }

    foreach ($del_options as $option) {
      $wpdb->delete( $wpdb->prefix . 'options', array( 'option_name' => $option ), array( '%s') );
    }
    Intel_Df::drupal_set_message(Intel_Df::t('Intelligence options cleared.'));
  }


}

function intel_util_clear_test_visitors() {
  $query = db_select('intel_visitor_identifier', 'i')
    ->fields('i')
    ->condition('type', 'email')
    ->condition('value', '%@example.com', 'LIKE');

  $visitors = $query->execute()->fetchAll();
  $vids = array();
  if (is_array($visitors)) {
    foreach ($visitors as $obj) {
      $vids[] = $obj->vid;
      intel_visitor_delete($obj->vid);
    }
  }
  return Intel_Df::t('deleted: ') . implode(', ', $vids);
}

