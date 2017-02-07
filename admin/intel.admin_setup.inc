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
    'text' => Intel_Df::t('GA goals'),
  );
  $items[] = array(
    'text' => Intel_Df::t('GA dimensions'),
  );

  $checklist = '';
  $checklist .= '<ul class="list-group checked-list-box">';
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
}

function intel_admin_setup_0($form, &$form_state, $status) {
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
  $items[] = Intel_Df::t('Complete the Google Analytics Dashboard For WP setup on the !link.', array(
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
  include_once INTEL_DIR . 'includes/class-intel-gapi.php';

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

  /*
  $gadwp = GADWP();
  $network_settings = false;
  if ( null === $gadwp->gapi_controller ) {
    $gadwp->gapi_controller = new GADWP_GAPI_Controller();
  }
  if (empty($gadwp->config->options['ga_dash_token'])) {
    $status['error_msg'] = Intel_Df::t('Plugin Authorization is not complete.');
  }
  else {
    $profiles = $gadwp->gapi_controller->refresh_profiles();
    if (empty($profiles)) {
      $status['error_msg'] = Intel_Df::t('Unable to retrieve profile list from Google Analytics.');
    }
    else {
      $status['success'] = 1;
    }
  }
  $form_state['gadwp'] = $gadwp;
  $form_state['intel_ga_profiles'] = $profiles;
  return $status;
  */
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

  $gap_options = array();
  $ga_profiles = $form_state['intel_ga_profiles'];
  foreach ($ga_profiles as $profile) {
    // split off http protocal on domain
    $domain = explode('//', $profile['url']);
    $domain = count($domain) == 2 ? $domain[1] : $domain[0];
    $gap_options[$profile['viewId']] = "$domain ({$profile['propertyId']}) / {$profile['viewName']}";
  }

  $f['intel_ga_view'] = array(
    '#type' => 'select',
    '#title' => Intel_Df::t('Google Analytics profile / view'),
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
d($form_state);

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'intl',
    ),
  );
  $items[] = '<p>';
  $items[] = Intel_Df::t('You will need to create an Intelligence API key and add it to your site using the field below.');
  $items[] = Intel_Df::t('First you will want to login or create an account on the !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('API management site'), 'http://intl.getlevelten.com', $l_options),
  ));
  $items[] = '</p><p>';
  $items[] = Intel_Df::t('Your Google Analytics Tracking ID is: @tid', array(
    '@tid' => $form_state['intel_ga_profile']['propertyId'],
  ));

  $items[] = '</p><p>';
  $l_options = array(
    'query' => array(
      'name' => 'test',
      'tid' => 'UA-1'
    ),
  );
  $items[] = Intel_Df::t('Once logged in, use the !link option to add a new property and create your API key.', array(
    '!link' => Intel_Df::l(Intel_Df::t('API management site'), 'http://intl.getlevelten.com/property/add'),
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
    '#size' => 32,
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

  $message = '';
  $property = array();
  $options = array(
    'tid' => get_option('intel_ga_tid', ''),
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

  return $status;
}

function intel_admin_setup_validate_3($form, &$form_state, $status) {
  d($status);
  if (!empty($status['error_msg'])) {
    Intel_Form::form_set_error('intel_apikey', $status['error_msg']);
  }
}

function intel_admin_setup_submit_3($form, &$form_state) {
  $values = $form_state['values'];
  update_option('intel_apikey', $values['intel_apikey']);
}


function intel_admin_setup_4($form, &$form_state) {
  $f = array();

  $status = $form_state['intel_statuses'][4];

  $ga_goals = $form_state['ga_goals'];
  $goals = get_option('intel_goals', array());

  $items = array();
  $l_options = array(
    'attributes' => array(
      'target' => 'intl',
    ),
  );
  $items[] = '<p>';
  $items[] = Intel_Df::t('A vital components of results oriented analytics is measuring when visitors achieve an organizational goal on the site.');
  $items[] = Intel_Df::t('Google Analytics enables you to track goals. Since goals are unique to each site, you must customize them for your sites objectives.');
  $url = intel_get_ga_admin_goals_url();
  $items[] = Intel_Df::t('You can setup goals for your Intelligence property via the !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('Google Analytics Goals Admin'), $url, $l_options),
  ));
  $items[] = '</p><p>';
  // TODO WP - post and link to articles
  $items[] = Intel_Df::t('To learn more about strategies for setting up your goals see the article !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('Creating Results Oriented Analytics Goals'), 'http://intl.getlevelten.com', $l_options),
  ));
  $items[] = '</p><p>';
  $l_options = array(
    'query' => array(
      'name' => 'test',
      'tid' => 'UA-1'
    ),
  );
  $items[] = Intel_Df::t('To configure goals to be triggered by Intelligence use the following steps:');
  $it = array();
  $it[] = Intel_Df::t('Click the <em>+New Goal</em> button');
  $it[] = Intel_Df::t('Name: create a name for your goal. E.g. "Sales inquiry"');
  $it[] = Intel_Df::t('Copy and paste the Name. You will need it on the next settings page.');
  $it[] = Intel_Df::t('Goal slot ID: Leave default setting.');
  $it[] = Intel_Df::t('Type: select Event.');
  $it[] = Intel_Df::t('Click <em>Continue</em>');
  $it[] = Intel_Df::t('Under Goal details set Category match drop down to <em>Regular expression</em>');
  $it[] = Intel_Df::t('In the Category match text field, paste the name of your goal and add "\+$" to the end. E.g. "Sales inquiry\+$"');
  $it[] = Intel_Df::t('Leave Use the Event value set to <em>Yes</em>');
  $it[] = Intel_Df::t('Click <em>Save</em>');
  $items[] = Intel_Df::theme('item_list', array('items' => $it, 'type' => 'ol'));
  $items[] = '</p><p>';

  $l_options = array(
    'query' => array(
      'refresh' => 1,
    ),
  );
  $items[] = Intel_Df::t('Once your goals are setup, !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('click here to refresh the list'), '.', $l_options),
  ));
  $items[] = Intel_Df::t('Set the <em>Value</em> field to the default value you would like to associate with the goal. This value can be overridden per form.');
  $items[] = Intel_Df::t('Event context sets what type of intel event can trigger a goal.');
  $items[] = Intel_Df::t('For example, the submission context will make a goal avaiable to be triggered from a form submission.');
  $items[] = Intel_Df::t('Uncheck any contexts where you would want to exclude the goal.');
  $items[] = '</p><br />';

  $items[] = '<h3>' . Intel_Df::t('Google Analytics Goals List') . '</h3>';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );


  $cells = array();
  $cells[] = Intel_Df::t('ID');
  $cells[] = Intel_Df::t('Name');
  $cells[] = Intel_Df::t('Type');
  $cells[] = Intel_Df::t('Value');
  $cells[] = Intel_Df::t('Event context');

  $col_classes = array();
  $markup = '';
  $markup .= '<style>
    th.col-id { width: 5%; }
    th.col-name { width: 35%; }
    th.col-type { width: 15%; }
    th.col-context { width: 30%; }
    th.col-value { width: 15%; }
</style>';
  $cell_markup = '';
  foreach ($cells as $i => $cell) {
    $col_classes[$i] = 'col-' . Intel_Df::drupal_clean_css_identifier(strtolower($cell));
    $cell_markup .= '<th class="' . $col_classes[$i] . '">' . $cell . '</th>';
  }
  $markup .= '<table class="table table-striped form-table"><thead><tr>' . $cell_markup . '</tr></thead><tbody>';
  $f['lista'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $context_options = array(
    'general' => Intel_Df::t('General'),
    'submission' => Intel_Df::t('Submission'),
    'phonecall' => Intel_Df::t('Phone call'),
  );
  foreach ($ga_goals as $i => $ga_goal) {
    $goal = !empty($goals[$i]) ? $goals[$i] : array();

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
      //'#markup' => '<div class="row"><div class="col-md-1">Goal ' . $i . '</div>'
      '#markup' => "<tr class=\"$row_class\"><td class=\"{$col_classes[0]}\">$i</td>",
    );

    $f["goal_{$i}_name"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="' . $col_classes[1] . '">',
      '#suffix' => '</th>',
      '#markup' => $ga_goal['name'],
    );

    $f["goal_{$i}_type"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="' . $col_classes[2] . '">',
      '#suffix' => '</th>',
      '#markup' => $ga_goal['typeLabel'],
    );

    if ($ga_goal['type'] == 'event') {
      $defaults = !empty($goal['context']) ? $goal['context'] : $context_options;
      foreach ($defaults as $kk => $vv) {
        if ($vv) {
          $defaults[$kk] = $kk;
        }
      }


      $f["goal_{$i}_value"] = array(
        '#type' => 'textfield',
        '#attributes' => array(
          'placeholder' => Intel_Df::t('value') . ' ' . $i,
        ),
        //'#field_prefix' => '$',
        '#prefix' => '<td class="' . $col_classes[4] . '">',
        '#suffix' => '</td>',
        '#default_value' => !empty($goal['value']) ? $goal['value'] : 0,
        '#size' => 8,
      );

      $f["goal_{$i}_context"] = array(
        '#type' => 'checkboxes',
        '#prefix' => '<td class="' . $col_classes[3] . '" rowspan="2">',
        '#suffix' => '</td>',
        '#options' => $context_options,
        '#default_value' => $defaults,
      );
    }
    else {
      $f["goal_{$i}_value"] = array(
        '#type' => 'item',
        '#prefix' => '<td class="' . $col_classes[4] . '">',
        '#suffix' => '</td>',
        '#markup' => $ga_goal['value'],
      );

      $f["goal_{$i}_context"] = array(
        '#type' => 'item',
        '#prefix' => '<td class="' . $col_classes[3] . '" rowspan="2">',
        '#suffix' => '</td>',
        '#markup' => Intel_Df::t('NA'),
      );
    }

    $f["goal_{$i}_description"] = array(
      '#type' => 'textfield',
      '#prefix' => '<tr><td></td><td class="' . $col_classes[3] . '" colspan="3">',
      '#suffix' => '</td></tr>',
      '#attributes' => array(
        'placeholder' => Intel_Df::t('goal description ') . " $i",
      ),
      '#default_value' => !empty($goal['description']) ? $goal['description'] : '',
    );

    $f["row_{$i}_b"] = array(
      '#type' => 'markup',
      '#markup' => '</tr>',
    );
  }

  $f['listb'] = array(
    '#type' => 'markup',
    '#markup' => '</tbody></table>'
  );


  return $f;
}

function intel_admin_setup_check_4($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();

  $ga_goals = get_option('intel_ga_goals', array());
  $op_meta = get_option('intel_option_meta', array());
  if (
    !empty($_GET['refresh'])
    || empty($op_meta['ga_goals_updated'])
    || (time() - $op_meta['ga_goals_updated']) > 3600) {

    $ga_goals = intel_fetch_ga_goals(TRUE);
  }

  $form_state['ga_goals'] = $ga_goals;

  if (empty($ga_goals)) {
    $status['error_msg'] = Intel_Df::t('No goals have been set. Please set at least one goal in Google Analytics.');
    return $status;
  }

  $goals = get_option('intel_goals', array());
  // build goals from the ground up, if validating form submission
  if (!empty($form_state['values'])) {
    foreach ($ga_goals as $id => $ga_goal) {
      $goals[$id] = array(
        'ga_id' => $id,
        'name' => strtolower(Intel_Df::drupal_clean_css_identifier($ga_goal['name'])),
        'title' => $ga_goal['name'],
        'description' => !empty($goals0[$id]['description']) ? $goals0[$id]['description'] : '',
        'value' => !empty($goals0[$id]['value']) ? $goals0[$id]['value'] : 0,
      );

      if (isset($values["goal_{$id}_context"])) {
        $goals[$id]['context'] = $values["goal_{$id}_context"];
        foreach ($goals[$id]['context'] as $k => $v) {
          $goals[$id]['context'][$k] = !empty($v) ? 1 : 0;
        }
        $goals[$id]['value'] = (float) $values["goal_{$id}_value"];
        $goals[$id]['description'] = $values["goal_{$id}_description"];
      }
    }
  }

  $status['success'] = 1;
  $status['goal_errors'] = array();
  foreach ($ga_goals as $id => $ga_goal) {
    if (empty($goals[$id])) {
      unset($status['success']);
      break;
    }
    $goal = $goals[$id];
    if ($ga_goal['type'] == 'event') {
      if (empty($ga_goal['details']['conditions']['category'])) {
        $msg = Intel_Df::t('Event conditions not set in Google Analytics for goal @goal.',
          array(
            '@goal' => $ga_goal['id'] . ' ' . $ga_goal['name'],
          ));
        $status['goal_errors'][$id] = array(
          'type' => 'condition_category_notset',
          'message' => $msg,
        );
      }
      if (empty($ga_goal['details']['conditions']['category']['matchType'])) {
        $msg = Intel_Df::t('Event condition match type not set in Google Analytics for goal @goal.',
          array(
            '@goal' => $ga_goal['id'] . ' ' . $ga_goal['name'],
          ));
        $status['goal_errors'][$id] = array(
          'type' => 'condition_category_matchtype_notset',
          'message' => $msg,
        );
      }
      if ($ga_goal['details']['conditions']['category']['matchType'] != 'regexp') {
        $msg = Intel_Df::t('Event condition match type not set properly in Google Analytics for goal @goal. Please set to "Regular expression"',
          array(
            '@goal' => $ga_goal['id'] . ' ' . $ga_goal['name'],
          ));
        $status['goal_errors'][$id] = array(
          'type' => 'condition_category_matchtype_incorrect',
          'message' => $msg,
        );
      }
      if ($ga_goal['details']['conditions']['category']['expression'] != $ga_goal['name'] . '\+$') {
        $msg = Intel_Df::t('Event condition match text set properly in Google Analytics for goal %goal. Please set to "@name\+$"',
          array(
            '%goal' => $ga_goal['id'] . ' ' . $ga_goal['name'],
            '@name' => $ga_goal['name'],
          ));
        $status['goal_errors'][$id] = array(
          'type' => 'condition_category_expression_incorrect',
          'message' => $msg,
        );
      }
    }

  }

  if (!empty($status['goal_errors'])) {
    unset($status['success']);
  }

  return $status;
}

function intel_admin_setup_validate_4($form, &$form_state, $status) {
  if (!empty($status['error_msg'])) {
    Intel_Form::form_set_error('intel_apikey', $status['error_msg']);
    return;
  }
  if (!empty($status['goal_errors'])) {
    foreach ($status['goal_errors'] as $i => $err) {
      Intel_Form::form_set_error('goal_' . $i . '_name', $err['message']);
    }

  }
}

function intel_admin_setup_submit_4($form, &$form_state) {
  $values = $form_state['values'];

  $ga_goals = $form_state['ga_goals'];

  $goals0 = get_option('intel_goals', array());
  $goals = array();
  foreach ($ga_goals as $id => $ga_goal) {

    $goals[$id] = array(
      'ga_id' => $id,
      'name' => str_replace('-', '_', strtolower(Intel_Df::drupal_clean_css_identifier($ga_goal['name']))),
      'title' => $ga_goal['name'],
      'description' => !empty($goals0[$id]['description']) ? $goals0[$id]['description'] : '',
      'value' => !empty($goals0[$id]['value']) ? $goals0[$id]['value'] : 0,
    );

    if (isset($values["goal_{$id}_context"])) {
      $goals[$id]['context'] = $values["goal_{$id}_context"];
      foreach ($goals[$id]['context'] as $k => $v) {
        $goals[$id]['context'][$k] = !empty($v) ? 1 : 0;
      }
      $goals[$id]['value'] = (float) $values["goal_{$id}_value"];
      $goals[$id]['description'] = $values["goal_{$id}_description"];
    }
  }

  update_option('intel_goals', $goals);
}

function intel_admin_setup_5($form, &$form_state) {
  $f = array();

  $status = $form_state['intel_statuses'][5];

  $items = array();

  $items[] = '<p>';
  $items[] = Intel_Df::t('Google Analytics custom dimensions enable you to attach attributes and segementation data to pages, events, sessions and visitors.');
  $items[] = Intel_Df::t('Intelligence faciliates trasmission of attributes, like authors, content types and taxonomies, into custom dimensions.');
  $url = intel_get_ga_admin_custom_dimensions_url();
  $l_options = array(
    'attributes' => array(
      'target' => 'ga',
    ),
  );
  $items[] = Intel_Df::t('In order for Google Analytics to store these attributes, you need so setup the definitions in !link.', array(
    '!link' => Intel_Df::l(Intel_Df::t('Google Analytics Custom Dimensions Admin'), $url, $l_options),
  ));
  $items[] = '</p><p>';
  $items[] = Intel_Df::t('Setup custom the custom dimensions following the table below.');
  $items[] = Intel_Df::t('Once complete, click the <em>Completed</em> button to verify setup in Google Analytics.');
  $items[] = '</p><br />';

  $items[] = '<h3>' . Intel_Df::t('Google Analytics Custom Dimensions') . '</h3>';

  $f['instructions'] = array(
    '#type' => 'markup',
    '#markup' => implode(' ', $items),
  );

  $dim_infos = intel_admin_setup_custom_dimensions_info();
  $ga_dimensions = $form_state['ga_dimensions'];

  $cells = array();
  $cells[] = Intel_Df::t('Name');
  $cells[] = Intel_Df::t('Index');
  $cells[] = Intel_Df::t('Scope');
  $cells[] = Intel_Df::t('State');
  $cells[] = Intel_Df::t('Description');
  $col_classes = array();
  $markup = '';
  $markup .= '<style>
    th.col-id { width: 5%; }
    th.col-name { width: 20%; }
    th.col-scope { width: 10%; }
    th.col-state { width: 10%; }
    th.col-description { width: 55%; }
</style>';
  $cell_markup = '';
  foreach ($cells as $i => $cell) {
    $col_classes[$i] = Intel_Df::drupal_clean_css_identifier(strtolower($cell));
    $cell_markup .= '<th class="col-' . Intel_Df::drupal_clean_css_identifier(strtolower($cell)) . '">' . $cell . '</th>';
  }
  $markup .= '<table class="table table-striped form-table"><thead><tr>' . $cell_markup . '</tr></thead><tbody>';
  $f['lista'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  foreach ($dim_infos as $i => $info) {
    $ga_dim = !empty($ga_dimensions[$i]) ? $ga_dimensions[$i] : array();
    $row_class = '';
    $col_classes1 = $col_classes;
    if (!empty($status['dimension_errors'][$i])) {
      $err = $status['dimension_errors'][$i];
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
    }

    $f["row_{$i}_a"] = array(
      '#type' => 'markup',
      //'#markup' => '<div class="row"><div class="col-md-1">Goal ' . $i . '</div>'
      '#markup' => "<tr class=\"$row_class\">",
    );

    $f["dim_{$i}_name"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="col-name">',
      '#suffix' => '</td>',
      '#markup' => $info['name'],
    );

    $f["dim_{$i}_id"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="col-id">',
      '#suffix' => '</td>',
      '#markup' => $i,
    );

    $f["dim_{$i}_scope"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="col-scope">',
      '#suffix' => '</td>',
      '#markup' => ucfirst($info['scope']),
    );

    $state = Intel_Df::t('Not created');
    if (isset($ga_dim['active'])) {
      $state = ($ga_dim['active']) ? Intel_Df::t('Active') : Intel_Df::t('Inactive');
    }
    $f["dim_{$i}_state"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="col-state">',
      '#suffix' => '</td>',
      '#markup' => $state,
    );

    $f["dim_{$i}_description"] = array(
      '#type' => 'item',
      '#prefix' => '<td class="col-descrption">',
      '#suffix' => '</td>',
      '#markup' => $info['description'],
    );

    $f["row_{$i}_b"] = array(
      '#type' => 'markup',
      '#markup' => '</tr>',
    );
  }

  $f['listb'] = array(
    '#type' => 'markup',
    '#markup' => '</tbody></table>'
  );


  return $f;
}

function intel_admin_setup_custom_dimensions_info() {
  $info = array();
  $info['1'] = array(
    'name' => Intel_Df::t('Page attributes'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores content segmentation data such as author, publish date, categories, and tags.'),
  );
  $info['2'] = array(
    'name' => Intel_Df::t('Session attributes'),
    'scope' => 'session',
    'description' => Intel_Df::t('Stores session segmentation data. Used for A/B and before & after testing.'),
  );
  $info['3'] = array(
    'name' => Intel_Df::t('Visitor attributes'),
    'scope' => 'user',
    'description' => Intel_Df::t('Stores visitor segmentation data such as roles, lead scoring, categories, and tags.'),
  );
  $info['4'] = array(
    'name' => Intel_Df::t('Time'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Used to establish timing.'),
  );
  $info['5'] = array(
    'name' => Intel_Df::t('Vtk'),
    'scope' => 'user',
    'description' => Intel_Df::t('Used for visitor identification.'),
  );
  $info['6'] = array(
    'name' => Intel_Df::t('Page identifiers'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores page resource ids such as post number and content type.'),
  );
  $info['7'] = array(
    'name' => Intel_Df::t('Object selectors'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores document selectors for embeded objects such as where a form is located on a page when submitted.'),
  );
  $info['8'] = array(
    'name' => Intel_Df::t('Object attributes'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores object segmentation data such as the fields displayed in a form that is submitted.'),
  );
  $info['9'] = array(
    'name' => Intel_Df::t('Object identifiers'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores object resource ids such as the id and type of a form that is submitted.'),
  );
  $info['10'] = array(
    'name' => Intel_Df::t('Publish age'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores the calculated time since a post was published enabling analysis of content lifecycles.'),
  );
  $info['11'] = array(
    'name' => Intel_Df::t('Page attributes 2'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores extended content segmentation statistics and data such as the length of a post, readability, and extracted terms.'),
  );
  $info['12'] = array(
    'name' => Intel_Df::t('Secondary object attributes'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores secondary object resource attributes such as which fields were submitted in a form.'),
  );
  $info['13'] = array(
    'name' => Intel_Df::t('Secondary object identifiers'),
    'scope' => 'hit',
    'description' => Intel_Df::t('Stores secondary object resource ids such as the id of form submission data.'),
  );

  return $info;
}

function intel_admin_setup_check_5($form, &$form_state) {
  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $status = array();

  $ga_dimensions = intel_fetch_ga_custom_dimensions(TRUE);

  $form_state['ga_dimensions'] = $ga_dimensions;

  $status['dimension_errors'] = array();
  $dim_infos = intel_admin_setup_custom_dimensions_info();
  foreach ($dim_infos as $i => $info) {
    if (empty($ga_dimensions[$i])) {
      $msg = Intel_Df::t('%id is missing. Please added it using the Google Analytics Admin.',
        array(
          '%id' => Intel_df::t('Dimension ') . $i,
        )
      );
      $status['dimension_errors'][$i] = array(
        'type' => 'notset',
        'message' => $msg,
      );
      //Intel_Df::drupal_set_message(t('Dimension @i has not been set in Google Analytics.'));
    }
    // dimension is setup, check if issues
    else {
      $ga_dim = $ga_dimensions[$i];
      if ($ga_dim['scope'] != $info['scope']) {
        $msg = Intel_Df::t('%id scope is set to %wrong. Please set the scope to %correct.',
          array(
            '%id' => Intel_df::t('Dimension ') . $i,
            '%wrong' => $ga_dim['scope'],
            '%correct' => $info['scope'],
          )
        );
        $status['dimension_errors'][$i] = array(
          'type' => 'wrongscope',
          'message' => $msg,
        );
      }
      else if (empty($ga_dim['active'])) {
        $msg = Intel_Df::t('%id is not active. Please enable the dimension by click the active checkbox.',
          array(
            '%id' => Intel_df::t('Dimension ') . $i,
          )
        );
        $status['dimension_errors'][$i] = array(
          'type' => 'notactive',
          'message' => $msg,
        );
      }
    }
  }

  if (empty($status['dimension_errors'])) {
    $status['success'] = 1;
  }

  return $status;
}

function intel_admin_setup_validate_5($form, &$form_state, $status) {
  if (!empty($status['dimension_errors'])) {
    foreach ($status['dimension_errors'] as $i => $err) {
      Intel_Form::form_set_error('dim_' . $i . '_name', $err['message']);
    }

  }
}

function intel_admin_setup_submit_5($form, &$form_state) {
  $values = $form_state['values'];

  $ga_goals = $form_state['ga_goals'];

  $goals0 = get_option('intel_goals', array());
  $goals = array();
  foreach ($ga_goals as $id => $ga_goal) {
    if (substr($id, 0 ,1) == '_') {
      continue;
    }
    $goals[$id] = array(
      'ga_id' => $id,
      'name' => strtolower(Intel_Df::drupal_clean_css_identifier($ga_goal['name'])),
      'title' => $ga_goal['name'],
      'description' => !empty($goals0[$id]['description']) ? $goals0[$id]['description'] : '',
      'value' => !empty($goals0[$id]['value']) ? $goals0[$id]['value'] : 0,
    );

    if (isset($values["goal_{$id}_context"])) {
      $goals[$id]['context'] = $values["goal_{$id}_context"];
      foreach ($goals[$id]['context'] as $k => $v) {
        $goals[$id]['context'][$k] = !empty($v) ? 1 : 0;
      }
      $goals[$id]['value'] = (float) $values["goal_{$id}_value"];
    }
  }

  update_option('intel_goals', $goals);
}

