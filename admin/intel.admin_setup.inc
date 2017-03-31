<?php
/**
 * @file
 * Admin configuration management
 */

include_once INTEL_DIR . 'includes/class-intel-form.php';

function intel_admin_setup($form, &$form_state) {
  $form['markup_start'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="row"><div class="col-md-3">',
  );

  $sys_meta = get_option('intel_system_meta', array());
  if (!isset($sys_meta['setup_step'])) {
    $sys_meta['setup_step'] = 0;
  }


  $setup_step = -1;
  $statuses = array();
  for ($i = 0; $i < 20; $i++) {
    $func = 'intel_admin_setup_check_' . $i;
    if (function_exists($func)) {
      $statuses[$i] = $func($form, $form_state);
    }
    if (empty($statuses[$i]['success'])) {
      $setup_step = $i;
      break;
    }
  }
  if ($setup_step == -1) {
    $setup_step = count($statuses);
  }

  if (!empty($_GET['step'])) {
    $setup_step = (int) $_GET['step'];
  }

  if ($setup_step > $sys_meta['setup_step']) {
    $sys_meta['setup_step'] = $setup_step;
    update_option('intel_system_meta', $sys_meta);
  }

/*
  //d($statuses);
  foreach ($statuses as $i => $status) {
    if (empty($status['success'])) {
      $setup_step = $i;
      break;
    }
  }
*/

  $items = array();
  $items[] = array(
    'text' => Intel_Df::t('Primary GA plugin'),
  );
  $items[] = array(
    'text' => Intel_Df::t('Primary GA profile'),
  );
  $items[] = array(
    'text' => Intel_Df::t('Intelligence GA profile'),
  );
  $items[] = array(
    'text' => Intel_Df::t('Intelligence API key'),
  );
  $items[] = array(
    'text' => Intel_Df::t('AddOns'),
  );
  $items[] = array(
    'text' => Intel_Df::t('Goals'),
  );
  $items[] = array(
    'text' => Intel_Df::t('Scoring'),
  );
  $items[] = array(
    'text' => Intel_Df::t('Complete'),
  );

  $checklist = '';
  $checklist .= '<ul class="list-group checked-list-box wizard-list-group">';
  foreach ($items as $i => $item) {
    $tag = array(
      'tag' => 'li',
      'value' => Intel_Df::l($item['text'], Intel_Df::current_path(), array('query' => array('step' => $i))),
      'attributes' => array(
        'class' => array(
          'list-group-item',
        )
      )
    );
    if ($setup_step > $i) {
      $tag['attributes']['class'][] = 'list-group-item-success';
      $tag['value'] = '<span class="glyphicon glyphicon-check" aria-hidden="true"></span> ' . $tag['value'];
    }
    else {
      if ($setup_step == $i) {
        $tag['attributes']['class'][] = 'active';
      }
      $tag['value'] = '<span class="glyphicon glyphicon-unchecked" aria-hidden="true"></span> ' . $tag['value'];
    }

    $checklist .= Intel_Df::theme_html_tag($tag);
  }
  $checklist .= '</ul>';
  $form_state['intel_statuses'] = $statuses;
  $form_state['intel_setup_step'] = $setup_step;

  $form['markup_sidebar'] = array(
    '#type' => 'markup',
    '#markup' => $checklist,
  );


  $form['markup_mid'] = array(
    '#type' => 'markup',
    '#markup' => '</div><div class="col-md-9"><div class="card"><div class="card-block">',
  );

  $func = 'intel_admin_setup_' . $setup_step;
  if (function_exists($func)) {
    $f = $func($form, $form_state);
    $form = Intel_Df::drupal_array_merge_deep($form, $f);
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => Intel_Df::t('Completed'),
    '#prefix' => '<br><br>',
  );

  $form['markup_end'] = array(
    '#type' => 'markup',
    '#markup' => '</div></div></div></div>',
  );
  $form_state['intel_setup_step'] = $setup_step;
  return $form;
}

function intel_admin_setup_statuses($form, &$form_state) {
  $statuses = array();
  for ($i = 0; $i < 20; $i++) {
    $func = 'intel_admin_setup_check_' . $i;
    if (function_exists($func)) {
      $statuses[] = $func($form, $form_state);
    }
  }
  return $statuses;
}

function intel_admin_setup_check($form, &$form_state) {
  $func = 'intel_admin_setup_check_' . $form_state['intel_setup_step'];
  if (function_exists($func)) {
    $f = $func($form, $form_state);
  }
}

function intel_admin_setup_validate($form, &$form_state) {
  $func = 'intel_admin_setup_check_' . $form_state['intel_setup_step'];
  if (function_exists($func)) {
    $status = $func($form, $form_state);
  }

  $func = 'intel_admin_setup_validate_' . $form_state['intel_setup_step'];
  if (function_exists($func)) {
    $f = $func($form, $form_state, $status);
  }
}

function intel_admin_setup_submit($form, &$form_state) {
  $func = 'intel_admin_setup_submit_' . $form_state['intel_setup_step'];
  if (function_exists($func)) {
    $f = $func($form, $form_state);
  }

  if (!empty($_GET['step'])) {
    $l_options = array(
      'query' => array(
        'step' => ((int)$_GET['step']) + 1,
      )
    );
    $msg = Intel_Df::t('Form was submitted. !link', array(
      '!link' => Intel_Df::l(Intel_Df::t('Click here to go to the next step.'), Intel_Df::current_path(), $l_options),
    ));
    Intel_Df::drupal_set_message($msg, 'success');
  }
}

function intel_admin_setup_0($form, &$form_state) {
  $f = array();

  $items = array();
  $items[] = Intel_Df::t('Intelligence requires a compatible Google Analytics plugin.');
  $l_options = array(
    'attributes' => array(
      'target' => 'gadwp',
    ),
  );
  $items[] = Intel_Df::t('Please install and activate the !plugin.',
    array(
      '!plugin' => Intel_Df::l(Intel_Df::t('Google Analytics Dashboard for WP plugin'), 'https://wordpress.org/plugins/google-analytics-dashboard-for-wp/', $l_options),
    ));

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  return $f;
}

function intel_admin_setup_check_0($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();

  // check dependencies
  // verify if gadwp exists
  $ga_data_source = intel_ga_data_source();
  if ($ga_data_source) {
    $status['success'] = 1;
  }
  else {
    if (is_plugin_active( 'google-analytics-dashboard-for-wp/gadwp.php' ) ) {
      update_option('intel_ga_data_source', 'gadwp');
      $status['success'] = 1;
    }
  }

  return $status;
}

function intel_admin_setup_1($form, &$form_state) {
  $f = array();

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'gadwp',
    ),
  );
  $items[] = Intel_Df::t('Complete the Google Analytics Dashboard For WP plugin setup on the !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('plugin settings page'), 'wp-admin/admin.php?page=gadash_settings', $l_options),
  ));
  $items[] = Intel_Df::t('Be sure to complete the Plugin Authorization and Select View configuration.');

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  return $f;
}

function intel_admin_setup_check_1($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';
  //include_once INTEL_DIR . 'includes/class-intel-gapi.php';

  $status = array();

  // check if gadwp ga authorization is complete
  $gadwp = GADWP();
  if (empty($gadwp->config->options['ga_dash_token'])) {
    $status['error_msg'] = Intel_Df::t('Plugin Authorization is not complete.');
    return $status;
  }

  if ( null === $gadwp->gapi_controller ) {
    $gadwp->gapi_controller = new GADWP_GAPI_Controller();
  }

  $ga_profiles = intel_fetch_ga_profiles();
  if (empty($ga_profiles)) {
    $status['error_msg'] = Intel_Df::t('Unable to retrieve profile list from Google Analytics.');
  }
  else {
    $status['success'] = 1;
  }

  $form_state['gadwp'] = $gadwp;
  $form_state['intel_ga_profiles'] = $ga_profiles;

  return $status;
}

function intel_admin_setup_2($form, &$form_state) {
  $f = array();

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'ga',
    ),
  );
  $items[] = '<p>';
  $items[] = Intel_Df::t('Intelligence tracks enhanced data in a seperate Google Analytics data set (property) from the primary one setup in the previous step.');
  $items[] = Intel_Df::t('Create a second Google Analytics property for storing Intelligence data in the !link.',
    array(
      '!link' => Intel_Df::l( Intel_Df::t('Google Analytics Admin'),  'https://analytics.google.com/analytics/web/#management/Settings', $l_options)
    )
    );
  $items[] = '</p>';

  $items[] = '<p>';
  $items[] = Intel_Df::t('Once the the second Google Analytics property has been created, select the property and view you want to use to track Intelligence data below.');
  $items[] = '</p>';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  $gap_options = array(
    '' => '--  ' . Intel_Df::t('none') . '  --',
  );
  $ga_profiles = $form_state['intel_ga_profiles'];
  foreach ($ga_profiles as $profile) {
    // split off http protocal on domain
    $domain = explode('//', $profile['url']);
    $domain = count($domain) == 2 ? $domain[1] : $domain[0];
    $gap_options[$profile['viewId']] = "$domain ({$profile['propertyId']}) / {$profile['viewName']}";
  }

  $f['intel_ga_view'] = array(
    '#type' => 'select',
    '#title' => Intel_Df::t('Intelligence Google Analytics profile / view'),
    '#default_value' => get_option('intel_ga_view', ''),
    '#options' => $gap_options,
    //'#description' => $desc,
  );

  return $f;
}

function intel_admin_setup_check_2($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();
  $ga_profiles = $form_state['intel_ga_profiles'];
  // if a form submission, form_state values set, use the submitted value to verify
  // designate the view
  if (isset($form_state['values']['intel_ga_view'])) {
    $intel_ga_view = $form_state['values']['intel_ga_view'];
  }
  else {
    $ga_profile = get_option('intel_ga_profile', array());
    $form_state['intel_ga_profile'] = $ga_profile;
    $intel_ga_view = !empty($ga_profile['viewId']) ? $ga_profile['viewId'] : '';
  }

  if (empty($intel_ga_view)) {
    $status['error_msg'] = Intel_Df::t('Intelligence Google Analytics property / view not set.');
    return $status;
  }

  $intel_profile = GADWP_Tools::get_selected_profile( $form_state['gadwp']->config->options['ga_dash_profile_list'], $intel_ga_view );
  $intel_ga_tid = $intel_profile[2];

  $prime_profile = GADWP_Tools::get_selected_profile( $form_state['gadwp']->config->options['ga_dash_profile_list'], $form_state['gadwp']->config->options['ga_dash_tableid_jail'] );
  $prime_ga_tid = $prime_profile[2];

  if ($intel_ga_tid == $prime_ga_tid) {
    $status['error_msg'] = Intel_Df::t('Intelligence Google Analytics property (tracking id) matches the primary property. Please select a seperate property.');
  }
  else {
    $status['success'] = 1;
  }

  return $status;
}

function intel_admin_setup_validate_2($form, &$form_state, $status) {
  if (!empty($status['error_msg'])) {
    Intel_Form::form_set_error('intel_ga_view', $status['error_msg']);
  }
}

function intel_admin_setup_submit_2($form, &$form_state) {
  $values = $form_state['values'];

  $profile = $form_state['intel_ga_profiles'][$values['intel_ga_view']];

  update_option('intel_ga_profile', $profile);
  update_option('intel_ga_tid', $profile['propertyId']);
  update_option('intel_ga_view', $profile['viewId']);

}

function intel_admin_setup_3($form, &$form_state) {
  $f = array();
  $status = $form_state['intel_statuses'][3];
  if (!empty($status['error_type']) && ($status['error_type'] = 'property_not_configured')) {
    Intel_Df::drupal_set_message($status['error_msg'], 'error');
  }

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'intl',
    ),
  );
  $imapi_domain = get_option('intel_imapi_url', INTEL_IMAPI_URL);
  $imapi_domain = explode('/', $imapi_domain);
  $imapi_domain = $imapi_domain[0];
  $items[] = '<p>';
  $items[] = Intel_Df::t('You will need to create an Intelligence API key and add it to your site using the field below.');
  $items[] = Intel_Df::t('First you will want to login or create an account on the !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('API management site'), "http://$imapi_domain", $l_options),
  ));
  $items[] = '</p><p>';
  $items[] = Intel_Df::t('Your Google Analytics Tracking ID is: @tid', array(
    '@tid' => $form_state['intel_ga_profile']['propertyId'],
  ));

  $items[] = '</p><p>';
  $l_options['query'] = array(
    'gaviewid' => $form_state['intel_ga_profile']['viewId'],
  );
  $items[] = Intel_Df::t('Once logged in, use the !link option to add a new property and create your API key.', array(
    '!link' => Intel_Df::l(Intel_Df::t('API management site'), "http://$imapi_domain/property/add", $l_options),
  ));
  $items[] = Intel_Df::t('Copy and paste the API key into the field below.');
  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  $f['intel_apikey'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('API key'),
    '#default_value' => get_option('intel_apikey', ''),
    //'#description' => $desc,
    '#size' => 40,
  );

  return $f;
}

function intel_admin_setup_check_3($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();

  $intel_apikey = get_option('intel_apikey', '');
  if (isset($form_state['values']['intel_apikey'])) {
    $intel_apikey = $form_state['values']['intel_apikey'];
  }

  $tid = get_option('intel_ga_tid', '');
  $message = '';
  $property = array();
  $options = array(
    'tid' => $tid,
    'apikey' => $intel_apikey,
  );
  $api_level = intel_verify_apikey($message, $property, $options);
  if (!empty($api_level)) {
    $status['success'] = 1;
  }
  else {
    $status['error_msg'] = Intel_Df::t('Intelligence API not connected.');
  }
  $form_state['intel_api_level'] = $api_level;

  // check custom dimensions setup correctly
  if ($api_level) {
    $dims = intel_fetch_ga_custom_dimensions();
    if (empty($dims)) {
      $status['success'] = 0;
      $status['error_type'] = 'property_not_configured';
      $l_options = Intel_Df::l_options_add_target('imapi');
      $status['error_msg'] = Intel_Df::t('GA property configuration is not complete. !link', array(
        '!link' => Intel_Df::l(Intel_Df::t('Click here to configure property.'), 'http://intl.getlevelten.com/property/' . $tid . '/ga_config', $l_options),
      ));
    }
  }

  return $status;
}

function intel_admin_setup_validate_3($form, &$form_state, $status) {
  if (!empty($status['error_msg'])) {
    Intel_Form::form_set_error('intel_apikey', $status['error_msg']);
  }
}

function intel_admin_setup_submit_3($form, &$form_state) {
  $values = $form_state['values'];
  update_option('intel_apikey', trim($values['intel_apikey']));
}

function intel_admin_setup_4($form, &$form_state) {
  $f = array();

  $form_state['intel_addon_info'] = $addon_info = intel()->addon_info();

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'intl',
    ),
  );
  $items[] = '<p>';
  $items[] = Intel_Df::t('Intelligence automates tracking of exteneded events in Google Analytics such as social sharing, video playback and commenting.');
  $items[] = '</p><p>';

  $items[] = '<h3>' . Intel_Df::t('Available Addons') . '</h3>';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  $field_default = array(
    '#type' => 'checkbox'
  );

  $addon_enabled = get_option('intel_addons_enabled', array());

  foreach ($addon_info as $k => $info) {
    $default = isset($info['enable_default']) ? $info['enable_default'] : 0;
    if (isset($addon_enabled[$k])) {
      $default = $addon_enabled[$k];
    }

    $f['addon_' . $k] = array(
      '#type' => 'checkbox',
      '#title' => $info['title'],
      '#description' => $info['description'],
      '#default_value' => $default,
    );
  }

  return $f;
}

function intel_admin_setup_check_4($form, &$form_state) {
  $status = array();

  $form_state['intel_addons_enabled'] = get_option('intel_addons_enabled', array());

  if (!empty($form_state['intel_addons_enabled'])) {
    $status['success'] = 1;
  }

  return $status;
}

function intel_admin_setup_validate_4($form, &$form_state, $status) {
  if (!empty($status['error_msg'])) {
    Intel_Form::form_set_error('intel_apikey', $status['error_msg']);
  }
}

function intel_admin_setup_submit_4($form, &$form_state) {
  $values = $form_state['values'];
  $addon_info = $form_state['intel_addon_info'];

  $enabled = array();
  foreach ($values as $k => $v) {
    if (substr($k, 0, 6) == 'addon_') {
      $name = substr($k, 6);
      $enabled[$name] = !empty($v) ? 1 : 0;
      if (!empty($addon_info[$name]['enable_callback'])) {
        if (function_exists($addon_info[$name]['enable_callback'])) {
          $addon_info[$name]['enable_callback']($enabled[$name]);
        }
      }
    }
  }
  update_option('intel_addons_enabled', $enabled);
}







function intel_admin_setup_5($form, &$form_state) {
  $f = array();

  $status = $form_state['intel_statuses'][4];

  $ga_goals = $form_state['intel_ga_goals'];
  $goals = get_option('intel_goals', array());

  $goals_default = intel_get_intel_goals_default();

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'intl',
    ),
  );
  $items[] = '<p>';
  $items[] = Intel_Df::t('A vital component of results oriented analytics is measuring when visitors convert by reaching an organizational goal.');
  $items[] = Intel_Df::t('Google Analytics enables you to track custom goals specific to your site\'s objectives.');
  $items[] = '</p><p>';
  $url = intel_get_ga_admin_goals_url();
  $items[] = Intel_Df::t('Intelligence can work with a mix of traditional Google Analytics Goals setup in !link and Intl Goals managed directly within WordPress.', array(
    '!link' => Intel_Df::l(Intel_Df::t('Google Analytics Goals Admin'), $url, $l_options),
  ));
  $items[] = Intel_Df::t('Intl Goals are more configurable, easier to manage and provide rich context. Therefore, it is recommended to primarly use Intl Goals.');
  $items[] = '</p><p>';
  // TODO WP - post and link to articles
  $l_options = array(
    'query' => array(
      'name' => 'test',
      'tid' => 'UA-1'
    ),
  );
  $items[] = Intel_Df::t('To learn more about strategies for setting up your goals see the article !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('Creating Results Oriented Analytics Goals'), 'http://intl.getlevelten.com', $l_options),
  ));
  $items[] = '</p><p>';

  //$items[] = '<h3>' . Intel_Df::t('Intl Goal Presents') . '</h3>';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  /*
  $goals_defs = intel_get_intel_goals_default();
  $cells = array();
  $cells[] = Intel_Df::t('Action');
  $cells[] = Intel_Df::t('Name');
  $cells[] = Intel_Df::t('Description');

  $col_classes = array();
  $markup = '';
  $markup .= '<style>
    .defaults th.col-add { width: 10%; }
    .defaults th.col-name { width: 30%; }
    .defaults th.col-description { width: 60%; }
</style>';
  $cell_markup = '';
  foreach ($cells as $i => $cell) {
    $col_classes[$i] = 'col-' . Intel_Df::drupal_clean_css_identifier(strtolower($cell));
    $cell_markup .= '<th class="' . $col_classes[$i] . '">' . $cell . '</th>';
  }
  $markup .= '<table class="table table-striped form-table defaults"><thead><tr>' . $cell_markup . '</tr></thead><tbody>';
  $f['table_defs_0'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  foreach ($goals_defs as $i => $v) {
    $f["row_defs_{$i}_a"] = array(
      '#type' => 'markup',
      '#markup' => "<tr>",
    );
    $action_options = array(
      'attributes' => array(
        'data-goal-default' => json_encode($v),
        'class' => array('goal-default-add', 'goal-default-add-' . $v['name']),
      ),
    );
    $f["row_defs_{$i}_action"] = array(
      '#type' => 'markup',
      '#markup' => "<td class=\"{$col_classes[0]}\">" . Intel_Df::l(Intel_Df::t('add'), 'javascript:', $action_options) ."</td>",
    );
    $f["row_defs_{$i}_title"] = array(
      '#type' => 'markup',
      '#markup' => "<td class=\"{$col_classes[1]}\">{$v['title']}</td>",
    );
    $f["row_defs_{$i}_description"] = array(
      '#type' => 'markup',
      '#markup' => "<td class=\"{$col_classes[1]}\">{$v['description']}</td>",
    );
    $f["row_defs_{$i}_b"] = array(
      '#type' => 'markup',
      '#markup' => "</tr>",
    );
  }
  $markup = '</tbody></table>';
  $f['table_defs_1'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  */

  $items = array();
  $items[] = '<h3>' . Intel_Df::t('Goals List') . '</h3>';

  $f['instructions2'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  $cells = array();
  $cells[] = Intel_Df::t('ID');
  $cells[] = Intel_Df::t('Name');
  $cells[] = Intel_Df::t('Type');
  $cells[] = Intel_Df::t('Description');

  $col_classes = array();
  $markup = '';
  $markup .= '<style>
    th.col-id { width: 4%; }
    th.col-name { width: 26%; }
    th.col-type { width: 15%; }
    th.col-description { width: 65%; }
</style>';
  $cell_markup = '';
  foreach ($cells as $i => $cell) {
    $col_classes[$i] = 'col-' . Intel_Df::drupal_clean_css_identifier(strtolower($cell));
    $cell_markup .= '<th class="' . $col_classes[$i] . '">' . $cell . '</th>';
  }
  $markup .= '<table class="table table-striped form-table"><thead><tr>' . $cell_markup . '</tr></thead><tbody>';
  $f['table_goals_0'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $context_options = array(
    'general' => Intel_Df::t('General'),
    'submission' => Intel_Df::t('Submission'),
    'phonecall' => Intel_Df::t('Phone call'),
  );
  $intl_goal_cnt = 0;
  //foreach ($ga_goals as $i => $ga_goal) {
  for ($i = 1; $i <= 20; $i++) {
    $ga_goal = !empty($ga_goals["$i"]) ? $ga_goals["$i"] : array();
    $goal = !empty($goals["$i"]) ? $goals["$i"] : array();

    $row_class = '';
    $col_classes1 = $col_classes;
    if (!empty($status['goal_errors'][$i])) {
      $err = $status['goal_errors'][$i];
      $row_class .= ' danger';
      /*
      if ($err['type'] == 'notset') {
        $row_class .= ' danger';
      }
      elseif ($err['type'] == 'wrongscope') {
        $row_class .= ' danger';
        $col_classes1[2] .= ' error';
      }
      elseif ($err['type'] == 'notactive') {
        $row_class .= ' danger';
      }
      */
    }

    $f["row_{$i}_a"] = array(
      '#type' => 'markup',
      '#markup' => "<tr class=\"$row_class\"><td class=\"{$col_classes[0]}\">$i</td>",
    );



    if (intel_is_intl_goal($ga_goal)) {
      $ga_goal['type'] = 'INTL';
      $ga_goal['typeLabel'] = Intel_Df::t('Intl');
    }

    if (empty($ga_goal) || $ga_goal['type'] == 'INTL') {
      // if no goals exist, add first default so at least one intel goal is created
      if (empty($ga_goal) && !$intl_goal_cnt) {
        reset($goals_default);
        $goal_default = current($goals_default);
        $ga_goal['name'] = $goal_default['title'];
        $goal['description'] = $goal_default['description'];
      }
      $intl_goal_cnt++;


      $f["goal_{$i}_name"] = array(
        '#type' => 'textfield',
        //'#attributes' => array(
        //  'placeholder' => Intel_Df::t('name') . ' ' . $i,
        //),
        '#prefix' => '<td class="' . $col_classes[1] . '">',
        '#suffix' => '</td>',
        '#default_value' => !empty($ga_goal['name']) ? $ga_goal['name'] : '',
      );

    }
    else {


      $f["goal_{$i}_name"] = array(
        '#type' => 'item',
        '#prefix' => '<td class="' . $col_classes[1] . '">',
        '#suffix' => '</td>',
        '#markup' => $ga_goal['name'],
      );
    }

    $f["goal_{$i}_typeLabel"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="' . $col_classes[2] . '">',
      '#suffix' => '</td>',
      '#markup' => !empty($ga_goal['typeLabel']) ? $ga_goal['typeLabel'] : 'Intl',
    );

    $f["goal_{$i}_type"] = array(
      '#type' => 'value',
      '#value' => !empty($ga_goal['type']) ? $ga_goal['type'] : 'INTL',
    );

    $f["goal_{$i}_description"] = array(
      '#type' => 'textfield',
      '#prefix' => '<td class="' . $col_classes[3] . '">',
      '#suffix' => '</td>',
      //'#attributes' => array(
      //  'placeholder' => Intel_Df::t('goal description ') . " $i",
      //),
      '#default_value' => !empty($goal['description']) ? $goal['description'] : '',
    );

    $f["row_{$i}_b"] = array(
      '#type' => 'markup',
      '#markup' => '</tr>',
    );
  }

  $f['table_goals_1'] = array(
    '#type' => 'markup',
    '#markup' => '</tbody></table>'
  );


  return $f;
}

function intel_admin_setup_check_5($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.imapi.inc';
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();

  $ga_goals = get_option('intel_ga_goals', array());
  $op_meta = get_option('intel_option_meta', array());

  if (
    !empty($_GET['refresh'])
    || empty($op_meta['ga_goals_updated'])
    || (time() - $op_meta['ga_goals_updated']) > 3600) {

    try {
      $ga_goals = intel_imapi_ga_goal_get();
    }
    catch (Exception $e) {
      if ($e instanceof \LevelTen\Intel\LevelTen_Service_Exception) {
        $e_type = $e->getType();
        $e_errors = $e->getErrors();
        $status['error_msg'] = $e->getMessage();
        return $status;
      }
      else {
        Intel_Df::drupal_set_message($e->getMessage(), 'error');
      }
    }
  }

  $form_state['intel_ga_goals'] = $ga_goals;
  $form_state['intel_goals'] = $goals = get_option('intel_goals', array());

  // if form submitted, process form_state values
  if (!empty($form_state['values'])) {
    $values = $form_state['values'];
    $has_intl_goal = 0;
    for ($i = 1; $i <= 20; $i++) {
      if (!empty($values["goal_{$i}_name"])) {
        $goals["$i"] = array(
          'name' => $values["goal_{$i}_name"],
          'type' => $values["goal_{$i}_type"],
        );
      }
    }
  }

  $has_intl_goal = 0;
  foreach ($goals as $goal) {
    if ($goal['type'] == 'INTL') {
      $has_intl_goal = 1;
      break;
    }
  }

  if (!$has_intl_goal) {
    $status['error_msg'] = Intel_Df::t('No Intl Goals have been set. Please set at least one Intl Goal.');
  }
  else {
    $status['success'] = 1;
  }

  return $status;
}

function intel_admin_setup_validate_5($form, &$form_state, $status) {
  $values = &$form_state['values'];
  $goals = $form_state['intel_goals'];

  foreach ($goals as $i => $goal) {
    if ($values["goal_{$i}_type"] == 'INTL' && empty($values["goal_{$i}_name"])) {
      $msg = Intel_Df::t('Intl Goal name missing. Intl Goals must have a name. If you are trying to delete a goal, not that GA goals cannot be deleted. Previous goal name was @name',
        array(
          '@name' => $goal['title'],
        ));
      Intel_Form::form_set_error('goal_' . $i . '_name', $msg);
    }
  }


  if (!empty($status['goal_errors'])) {
    foreach ($status['goal_errors'] as $i => $err) {
      Intel_Form::form_set_error('goal_' . $i . '_name', $err['message']);
    }
  }
}

function intel_admin_setup_submit_5($form, &$form_state) {
  $values = $form_state['values'];

  $ga_goals = $form_state['intel_ga_goals'];

  $goals_default = intel_get_intel_goals_default();

  $saves = array();
  for($i = 1; $i <= 20; $i++) {
    $id = "$i";
    if (!empty($values["goal_{$id}_name"])) {
      $goal_arr = array(
        'id' => $id,
        'name' => $values["goal_{$id}_name"],
        'type' => 'EVENT',
        'active' => TRUE,
        'value' => 0,
        'details' => array(
          'useEventValue' => TRUE,
          'conditions' => array(
            array(
              'type' => 'CATEGORY',
              'matchType' => 'REGEXP',
              'expression' => $values["goal_{$id}_name"] . '\+$',
            ),
          ),
        ),
      );
      // check if goal settings in values matches existing goal, don't save if so
      if (!empty($ga_goals[$id])) {
        $ga_goal = $ga_goals[$id];
        if (
          $ga_goal['name'] == $goal_arr['name']
          && !empty($ga_goal['details']['conditions'][0]) && empty($ga_goal['details']['conditions'][1])
          && $ga_goal['details']['conditions'][0]['type'] == 'CATEGORY'
          && $ga_goal['details']['conditions'][0]['matchType'] == 'REGEXP'
          && $ga_goal['details']['conditions'][0]['expression'] == $goal_arr['name'] . '\+$'
        ) {
          continue;
        }
      }

      if (!empty($ga_goals[$id])) {
        $saves[$id] = 'update';
        try {
          intel_imapi_ga_goal_update($goal_arr);
        }
        catch(Exception $e) {
          Intel_Df::drupal_set_message($e->getMessage(), 'error');
        }
      }
      else {
        $saves[$id] = 'insert';
        try {
          intel_imapi_ga_goal_insert($goal_arr);
        }
        catch(Exception $e) {
          Intel_Df::drupal_set_message($e->getMessage(), 'error');
        }
      }
    }
  }

  if (!empty($saves)) {
    $ga_goals = intel_imapi_ga_goal_get();
  }

  $goals0 = get_option('intel_goals', array());
  $goals = array();
  foreach ($ga_goals as $id => $ga_goal) {

    $name = str_replace('-', '_', strtolower(Intel_Df::drupal_clean_css_identifier($ga_goal['name'])));

    if (intel_is_intl_goal($ga_goal)) {
      if (!empty($goals0[$id]['value'])) {
        $value = $goals0[$id]['value'];
      }
      else if (!empty($goals_defalt[$name]['value'])) {
        $value = $goals_defalt[$name]['value'];
      }
      else {
        $value = 50;
      }
    }
    else {
      $value = $ga_goal['value'];
    }

    $goals[$id] = array(
      'ga_id' => $id,
      'name' => $name,
      'title' => $ga_goal['name'],
      'description' => !empty($goals0[$id]['description']) ? $goals0[$id]['description'] : '',
      'type' => !empty($goals0[$id]['type']) ? $goals0[$id]['type'] : '',
      'value' => $value,
      'context' => array(),
    );

    if (isset($values["goal_{$id}_type"])) {
      $goals[$id]['type'] = $values["goal_{$id}_type"];
    }

    if (isset($values["goal_{$id}_description"])) {
      $goals[$id]['description'] = $values["goal_{$id}_description"];
    }

    // if goal type INTL, set contexts
    if ($goals[$id]['type'] == 'INTL') {
      // if no context exists for goal, add all contexts
      if (empty($goals0[$id]['context'])) {
        $goals[$id]['context'] = array(
          'general' => 1,
          'phonecall' => 1,
          'submission' => 1,
        );
      }
      else {
        $goals[$id]['context'] = $goals0[$id]['context'];
      }
    }
  }
  update_option('intel_goals', $goals);
}

function intel_admin_setup_6($form, &$form_state) {
  $f = array();

  $status = $form_state['intel_statuses'][5];

  $items = array();

  $items[] = '<p>';
  $items[] = Intel_Df::t('Intelligence enables you to score a variety of items such as traffic metrics, valued events and goals.');
  $items[] = Intel_Df::t('Use the fields below to set value scores.');
  $items[] = '</p><br />';

  $items[] = '<h3>' . Intel_Df::t('Scores') . '</h3>';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );



  $scorings = intel_get_scorings();
  $scorings = get_option('intel_scorings', array());
  $f['base'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Traffic metrics'),
    '#collapsible' => TRUE,
  );
  $f['events'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Valued events'),
    '#collapsible' => TRUE,
  );
  $f['goals'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Goals'),
    '#collapsible' => TRUE,
  );
  $base_scorings = intel_get_base_scorings();
  foreach ($base_scorings AS $i => $m) {
    $value = !empty($scorings[$i]) ? $scorings[$i] : $m['value'];
    $f['base']['score_' . $i] = array(
      '#type' => 'textfield',
      '#title' => $m['title'],
      '#default_value' => $value,
      '#description' => $m['description'],
      '#size' => 8,
    );
  }

  $events = intel_get_intel_event_info();
  foreach ($events AS $i => $m) {
    if (empty($m['valued_event'])) {
      continue;
    }
    $value = !empty($scorings[$i]) ? $scorings[$i] : $m['value'];
    $f['events']['score_' . $i] = array(
      '#type' => 'textfield',
      '#title' => $m['title'],
      '#default_value' => $value,
      '#description' => $m['description'],
      '#size' => 8,
    );
  }
  $ga_goals =
  $goals = get_option('intel_goals', array());

  $form_state['goals'] = $goals;
  foreach ($goals AS $i => $m) {
    $value = !empty($scorings['goal_' . $i]) ? $scorings['goal_' . $i] : (isset($m['value']) ? $m['value'] : 0);
    if ($goals[$i]['type'] == 'INTL') {
      $f['goals']['score_goal_' . $i] = array(
        '#type' => 'textfield',
        '#title' => $m['title'],
        '#default_value' => $value,
        '#description' => $m['description'],
        '#size' => 8,
      );
    }
    else {
      $f['goals']['score_goal_' . $i] = array(
        '#type' => 'item',
        '#title' => $m['title'],
        '#markup' => $value,
        '#description' => $m['description'] . ' (' . Intel_Df::t('Goal value set in Google Analytics.') . ')',
        '#size' => 8,
      );
    }

  }

  return $f;
}



function intel_admin_setup_check_6($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();

  $form_state['intel_scorings'] = $scorings = intel_get_scorings();

  $scorings_option = get_option('intel_scorings', array());

  // successful if scorings option has been set.
  if (!empty($scorings_option)) {
    $status['success'] = 1;
  }

  return $status;
}

function intel_admin_setup_validate_6($form, &$form_state, $status) {
  if (!empty($status['dimension_errors'])) {
    foreach ($status['dimension_errors'] as $i => $err) {
      Intel_Form::form_set_error('dim_' . $i . '_name', $err['message']);
    }

  }
}

function intel_admin_setup_submit_6($form, &$form_state) {
  $values = $form_state['values'];

  $scores = array();
  $goals = $form_state['intel_goals'];
  foreach ($values AS $k => $value) {
    if (substr($k, 0, 6) == 'score_') {
      $key = substr($k, 6);
      $id = substr($key, 5);
      $scores[$key] = (float)$value;
      if (!empty($goals[$id]['name'])) {
        $scores['goal_' . $goals[$id]['name']] = $scores[$key];
      }
    }
  }

  update_option('intel_scorings', $scores);
}

function intel_admin_setup_7($form, &$form_state) {
  $f = array();

  $status = $form_state['intel_statuses'][5];

  $items = array();

  $items[] = '<p>';
  $items[] = Intel_Df::t('Congratulations. Intelligence setup is complete.');
  $items[] = '</p><br />';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  $sys_meta = get_option('intel_system_meta', array());
  if (!isset($sys_meta['setup_step']) || $sys_meta['setup_step'] < 100) {
    $sys_meta['setup_step'] = 100;
    $sys_meta['setup_complete'] = time();
    $sys_meta = update_option('intel_system_meta', $sys_meta);
  }


  return $f;
}



function intel_admin_setup_check_7($form, &$form_state) {

  $status = array();

  return $status;
}

function intel_admin_setup_validate_7($form, &$form_state, $status) {
  if (!empty($status['dimension_errors'])) {
    foreach ($status['dimension_errors'] as $i => $err) {
      Intel_Form::form_set_error('dim_' . $i . '_name', $err['message']);
    }

  }
}

function intel_admin_setup_submit_7($form, &$form_state) {

}

