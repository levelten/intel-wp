<?php
/**
 * Process variables for user-profile.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $account
 *
 * @see user-profile.tpl.php
 */

function template_process_intel_page(&$variables) {
  if (empty($variables['class'])) {
    $variables['class'] = '';
  }
  if (!empty($_GET['q'])) {
    $q = str_replace('/', '--', $_GET['q']);
    $variables['class'] .= ' ' . Intel_Df::drupal_clean_css_identifier($q);
  }
  if (empty($variables['title'])) {
    $variables['title'] = intel()->get_page_title();
  }
}

function template_process_intel_navbar(&$variables) {
  $base_path = $variables['base_path'];
  $navs = array(
    'nav' => 'tree',
    'nav_right' => 'tree2',
  );

  foreach ($navs as $k => $kk) {
    $variables[$k] = '';
    $variables[$k] .= ($k == 'nav_right') ? '<ul class="nav navbar-nav navbar-right">' : '<ul class="nav navbar-nav">';
    foreach ($variables[$kk] as $key =>$item) {
      if (!isset($item['#info'])) {
        continue;
      }
      $info = $item['#info'];
      $class = !empty($info['active']) ? 'active' : '';
      $dropdown = 0;
      $link_text = $info['title'];
      $link_options = array();
      if (count($item) > 1) {
        $dropdown = 1;
        $link_text .= ' <span class="caret"></span>';
        $class .= ' dropdown';
        $link_options['html'] = TRUE;
        $link_options['attributes'] = array(
          //'data-toggle' => 'dropdown',
          'role' => 'button',
          'aria-haspopup' => 'true',
          'aria-expanded' => 'false',
          'class' => array(
            //'dropdown-toggle'
          ),
        );
        if (!empty($info['active'])) {
          $link_options['attributes']['data-toggle'] = 'dropdown';
          $link_options['attributes']['class'][] = 'dropdown-toggle';
        }
      }
      $variables[$k] .= "<li class=\"$class\">";
      if (!empty($_GET['report_params'])) {
        $link_options['query'] = array(
          'report_params' => $_GET['report_params']
        );
      }
      $variables[$k] .= Intel_Df::l($link_text, $base_path . '/' . $key, $link_options);
      if ($dropdown) {
        $variables[$k] .= '<ul class="dropdown-menu">';
        foreach ($item as $skey => $sitem) {
          if (substr($skey, 0, 1) == '#') {
            continue;
          }
          $sinfo = $sitem['#info'];
          $slink_options = array();
          if (!empty($_GET['report_params'])) {
            $slink_options['query'] = array(
              'report_params' => $_GET['report_params']
            );
          }
          $class = !empty($sinfo['active']) ? 'active' : '';
          $variables[$k] .= "<li class=\"$class\">";
          $variables[$k] .= Intel_Df::l($sinfo['title'], $base_path . '/' . $key . '/' . $skey, $slink_options);
          $variables[$k] .= "</li>\n";
        }
        $variables[$k] .= '</ul>';
      }
      $variables[$k] .= "</li>\n";
    }
    $variables[$k] .= "</ul>\n";
  }
}

function template_process_intel_shortcode(&$variables) {
  if (!isset($variables['class'])) {
    $variables['class'] = '';
  }
  if (!empty($_GET['q'])) {
    $q = str_replace('/', '--', $_GET['q']);
    $variables['class'] .= ' ' . Intel_Df::drupal_clean_css_identifier($q);
  }
  if (!isset($variables['messages'])) {
    $variables['messages'] = Intel_Df::drupal_get_messages();
  }
}

function template_process_intel_visitor_profile(&$variables) {
  //$visitor = $variables['elements']['#visitor'];

  $defs = array(
    'attributes' => array(),
  );
  $variables += $defs;
  // Helpful $user_profile variable for templates.
  foreach (Intel_Df::element_children($variables['elements']) as $key) {
    $region = 'main';
    if (!empty($variables['elements'][$key]['#region'])) {
      $region = $variables['elements'][$key]['#region'];
    }
    if (!isset($variables[$region])) {
      $variables[$region] = array();
    }
    if ($region == 'header') {
       $variables[$key] = $variables['elements'][$key];
    }
    else {
      $variables[$region][$key] = $variables['elements'][$key];
    }
  }
  $variables['attributes'] = Intel_Df::drupal_attributes($variables['attributes']);
  // Preprocess fields.
  //field_attach_preprocess('intel_visitor', $visitor, $variables['elements'], $variables);
}

/**
 * Process variables for user-profile-item.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $element
 *
 * @see user-profile-item.tpl.php
 */
function template_preprocess_intel_visitor_profile_item(&$variables) {
  /*
  $variables['title'] = $variables['element']['#title'];
  $variables['value'] = $variables['element']['#markup'];
  $variables['attributes'] = '';
  if (isset($variables['element']['#attributes'])) {
    $variables['attributes'] = drupal_attributes($variables['element']['#attributes']);
  }
  */
}

function template_process_intel_visitor_profile_item_list(&$variables) {
  $variables['attributes'] = Intel_Df::drupal_attributes($variables['attributes']);
  /*
  $variables['title'] = $variables['element']['#title'];
  $variables['value'] = $variables['element']['#markup'];
  $variables['attributes'] = '';
  if (isset($variables['element']['#attributes'])) {
    $variables['attributes'] = drupal_attributes($variables['element']['#attributes']);
  }
  */
}

function template_preprocess_intel_visitor_profile_block(&$variables) {
  /*
  if (isset($variables['element'])) {
    $variables['title'] = isset($variables['element']['#title']) ? $variables['element']['#title'] : '';
    $variables['markup'] = isset($variables['element']['#markup']) ? $variables['element']['#markup'] : '';
  }
  $variables['attributes'] = '';
  if (!isset($variables['element']['#attributes'])) {
    $variables['element']['#attributes'] = array();
  }
  if (!isset($variables['element']['#attributes']['class'])) {
    $variables['element']['#attributes']['class'] = array();
  }
  $variables['element']['#attributes']['class'][] = 'profile-block';
  
  if (isset($variables['element']['#attributes'])) {
    $variables['attributes'] = Intel_Df::drupal_attributes($variables['element']['#attributes']);
  }

  */
  $variables += array(
    'header' => NULL,
    'body' => '',
    'footer' => NULL,
  );
  // translate visitor_profile_block vars into bootstrap card
  if (!empty($variables['title'])) {
    $variables['body'] .= '<h3 class="card-header">' . $variables['title'] . '</h3>';
  }
  if (!empty($variables['markup'])) {
    $variables['body'] .= '<div class="card-block">' . $variables['markup'] . '</div>';
  }
}

/**
 * Process variables for user-picture.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $account: A user, node or comment object with 'name', 'uid' and 'picture'
 *   fields.
 *
 * @see user-picture.tpl.php
 */
function template_preprocess_intel_visitor_picture(&$variables) {
  $variables['picture'] = '';
  $entity = $variables['entity'];
  $image_variables = isset($variables['image_variables']) ?  $variables['image_variables'] : array();
  $avatar_options = isset($variables['avatar_options']) ?  $variables['avatar_options'] : array();

  $filepath = get_option('intel_visitor_default_image_path', '');
  if (empty($filepath)) {
    $filepath = get_option('user_picture_default', '');

    //$filepath = get_avatar_url( mixed $id_or_email, array $args = null )
  }
  if (isset($entity->data['image']) && !empty($entity->data['image']['url'])) {
    $filepath = $entity->data['image']['url'];
  }
  else {
    $id = isset($avatar_options['id']) ? $avatar_options['id'] : $entity->getEmail();
    $filepath = get_avatar_url( $id, $avatar_options);
  }

  if (!empty($filepath)) {
    $alt = Intel_Df::t("@user's picture", array('@user' => $entity->name));
    $image_variables['path'] = $filepath;
    if (!isset($image_variables['alt'])) {
      $image_variables['alt'] = $alt;
    };
    if (!isset($image_variables['title'])) {
      $image_variables['title'] = $alt;
    };
    if (!isset($image_variables['attributes'])) {
      $image_variables['attributes'] = array();
    };
    if (!isset($image_variables['attributes']['class'])) {
      $image_variables['attributes']['class'] = array();
    };
    $image_variables['attributes']['class'][] = 'img-responsive';
    $image_variables['attributes']['class'][] = 'center-block';

    // If the image does not have a valid Drupal scheme (for eg. HTTP),
    // don't load image styles.
    if (FALSE && module_exists('image') && file_valid_uri($filepath) && $style = get_option('user_picture_style', '')) {
      $image_variables['style_name'] = $style;
      $variables['picture'] = Intel_Df::theme('image_style', $image_variables);
    }
    else {
      $variables['picture'] = Intel_Df::theme('image', $image_variables);
    }
    // TODO set correct user_access permission
    if (FALSE && !empty($entity->vid) && user_access('access user profiles')) {
      $attributes = array('attributes' => array('title' => Intel_Df::t('View user profile.')), 'html' => TRUE);
      $variables['picture'] = Intel_Df::l($variables['picture'], $entity->uri(), $attributes);
    }
  }
}

function intel_visitor_format_profile_item($title, $value, $attributes = array()) {
  $item = array(
    'title' => $title,
    'value' => $value,
  );
  if (!empty($attributes)) {
    $item['attributes'] = $attributes;
  }
  return Intel_Df::theme('intel_visitor_profile_item', $item);
}

function intel_visitor_render_profile_items($items) {
  $output = '';
  $output = '';
  foreach ($items as $item) {
    //$output .= Intel_Df::render($item);
    $output .= $item;
  }
  return $output;
}

function template_preprocess_intel_visitor_social_links (&$variables) {
  $entity = $variables['entity'];
  $properties = $entity->data;
  $property_info = intel()->visitor_property_info();

  $variables['markup'] = '';
  foreach ($property_info AS $prop_name => $info) {
    if (!isset($info['category']) || ($info['category'] != 'social') || empty($info['icon'])) {
      continue;
    }
    $a = explode('.', $prop_name);
    $scope = $a[0];
    $namespace = $a[1];
    if (empty($properties[$namespace]) || empty($properties[$namespace]['url'])) {
      continue;
    }
    $prop = $properties[$namespace];
    $alt = Intel_Df::t('@title profile (source: @source)',
      array(
        '@title' => $info['title'],
        '@source' => !empty($prop['_source']) ? $prop['_source'] : Intel_Df::t('(not set)'),
      )
    );
    $iv = array(
      'path' => '/wp-content/uploads/' . $info['icon'],
      'alt' => $alt,
      'title' => $alt,
    );
    $img = Intel_Df::theme('image', $iv);
    $options = array(
      'html' => true,
      'attributes' => array(
        'target' => $namespace,
      ),
    );
    
    $vars = array(
      'link' => Intel_Df::l($img, $prop['url'], $options),
      'class' => preg_replace('/[^\x{002D}\x{0030}-\x{0039}\x{0041}-\x{005A}\x{005F}\x{0061}-\x{007A}\x{00A1}-\x{FFFF}]/u', '', $namespace),
    );
    
    $variables['markup'] .= Intel_Df::theme('intel_visitor_social_link', $vars);
  }
}

function intel_format_location_name($location, $format = 'country') {
  $out = '';
  if ($format == 'city, state, country') {
    $out = !empty($location['city']) ? $location['city'] : t('(not set)') . ', ';
    $out .= ', ' . !empty($location['region']) ? $location['region'] : t('(not set)');
    $out .= ', ' . !empty($location['country']) ? $location['country'] : t('(not set)');
  }
  else if ($format == 'map') {
    $out = !empty($location['city']) ? $location['city'] : '(not set)';
    $out .= ', ' . (!empty($location['region']) ? $location['region'] : t('(not set)'));
    if (isset($location['metro']) && ($location['metro'] != '(not set)')) {
      $out .= ' (' . $location['metro'] . ')';
    }
    $out .= "<br />\n" . (!empty($location['country']) ? $location['country'] : t('(not set)'));
  }
  else {
    $out = !empty($location['country']) ? $location['country'] : t('(not set)');
  }
  return $out;
}

function theme_intel_location(&$variables) {
  $output = '';
  $location = array();
  if (!empty($variables['entity'])) {
    $entity = $variables['entity'];
    $location = $entity->getVar('data', 'location');
  }
  else if (!empty($variables['location'])) {
    $location = $variables['location'];
  }

  if (empty($location)) {
    return;
  }

  $location['name'] = $location['city'] . ', ' . $location['region']  . ', ' . $location['country'];
  $vars = array(
    'locations' => array($location),
  );
  $output .= Intel_Df::theme('intel_map', $vars);
  $output .= '<div class="card-block">' . intel_format_location_name($location, 'map') . '</div>';
  return $output;
}



function template_process_intel_location_block(&$variables) {
  $variables['markup'] = Intel_Df::theme('intel_location', $variables);
  if (empty($variables['markup'])) {
    // remove title to clear block
    $variables['title'] = '';
    return;
  }
  if (empty($variables['title'])) {
    $variables['title'] = Intel_Df::t('Location');
  }
  $variables['no_card_block'] = 1;
  if (!isset($variables['attributes']['class'])) {
    $variables['attributes']['class'] = array();
  }
  $variables['attributes']['class'][] = 'card';
  $variables['attributes']['class'][] = 'location-map';
  $variables['attributes'] = Intel_Df::drupal_attributes($variables['attributes']);
}

function x_template_process_intel_visitor_location(&$variables) {
  $variables['title'] = '';
  $variables['markup'] = '';
  if (!empty($variables['entity'])) {
    $entity = $variables['entity'];
    $location = $entity->getVar('data', 'location');
  }
  else if (!empty($variables['location'])) {
    $location = $variables['location'];
  }

  if (empty($location)) {
    return;
  }
  
  $variables['title'] = Intel_Df::t('Location');
  $location['name'] = $location['city'] . ', ' . $location['region']  . ', ' . $location['country']; 
  $vars = array(
    'locations' => array($location),
  );

  $map = Intel_Df::theme('intel_map', $vars);

  $map .= '<div class="card-block">' . intel_format_location_name($location, 'map') . '</div>';
  $variables['markup'] = $map;
  $variables['no_card_block'] = 1;
  if (!isset($variables['attributes']['class'])) {
    $variables['attributes']['class'] = array();
  }
  $variables['attributes']['class'][] = 'card';
  $variables['attributes']['class'][] = 'location-map';
  $variables['attributes'] = Intel_Df::drupal_attributes($variables['attributes']);
}

function template_preprocess_intel_map(&$variables) {
  $locations = $variables['locations'];
  $gmap_apikey = get_option('intel_gmap_apikey', '');

  static $map_index;
  if (!isset($map_index)) {
    $map_index = 0;
    wp_enqueue_script( 'intel_googleapis_map', 'https://maps.googleapis.com/maps/api/js?v=3.exp&key=' . $gmap_apikey . '&callback=intel_init_map_all');
    //wp_enqueue_script( 'intel_map', INTEL_URL . 'js/intel.map.js');
  }

  $variables['map_index'] = $map_index++;
  $variables['map_options'] = array();
  $variables['map_options']['zoom'] = 4;
  $variables['map_options']['center'] = array(
    'lat' => 0, 
    'lon' => 0,
  );
  $variables['locations'] = array();

  foreach ($locations AS $loc) {
    if (!isset($loc['lat']) && isset($loc['latitude'])) {
      $loc['lat'] = $loc['latitude'];
    }
    if (!isset($loc['lon']) && isset($loc['longitude'])) {
      $loc['lon'] = $loc['longitude'];
    }
    $variables['locations'][] = array(
      'lat' => $loc['lat'],
      'lon' => $loc['lon'],
      'title' => $loc['name'],
    );
    //$vars['locations_json'] .= "[" . $loc['lat'] . ", " . $loc['lon'] . ", " . "'" . (isset($loc['name']) ? $loc['name'] : '') . "'],\n";
    $variables['map_options']['center']['lat'] = $loc['lat']; 
    $variables['map_options']['center']['lon'] = $loc['lon']; 
  }
}

function template_preprocess_intel_visitor_browser_environment(&$variables) {
  $variables['title'] = '';
  $variables['markup'] = '';
  $entity = $variables['entity'];
  $env = $entity->getVar('data', 'environment');
  if (empty($env)) {
    return;
  }
  $variables['title'] = Intel_Df::t('Browser environment');
  $items = array();
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('Browser'), $env['browser'] . " v" . $env['browserVersion']);
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('OS'), $env['operatingSystem'] . " " . $env['operatingSystemVersion']);
  if (!empty($env['mobileDeviceInfo'])) {
    $items[] = intel_visitor_format_profile_item(Intel_Df::t('Mobile device'), $env['mobileDeviceInfo']);
  }
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('Screen'), $env['screenResolution']);
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('Language'), $env['language']);

  $variables['markup'] = intel_visitor_render_profile_items($items);  
}

/**
 * Process variables for intel-visitor-browser-env.tpl.php.
 *
 * @see user-picture.tpl.php
 */
function template_preprocess_intel_visitor_bio(&$variables) {
  //$variables['markup'] = $variables['entity']->getVar('data', 'bio', 'summary');
  $variables['markup'] = $variables['entity']->getVar('data', 'description', '@value');
  if (strlen($variables['markup']) > 260) {
    $variables['markup'] = substr($variables['markup'], 0, 260);
    $variables['markup'] = preg_replace('/\W\w+\s*(\W*)$/', '$1', $variables['markup']) . ' ...';

  }
  //$variables['markup'] = '<span class="lead">' . $variables['markup'] . '</span>';
}

/**
 * Process variables for user-picture.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $account: A user, node or comment object with 'name', 'uid' and 'picture'
 *   fields.
 *
 * @see user-picture.tpl.php
 */
function template_preprocess_intel_visitor_browser_location(&$variables) {
  $variables['intel_visitor_browser_location'] = '';
  $entity = $variables['entity'];
}

function theme_intel_visit_steps_table(&$variables) {
  $output = '';

  $steps = $variables['steps'];
  $header = array(
    __('Time', 'intel'),
    __('Page/Event', 'intel'),
    __('Value', 'intel'),
  );
  // preprocess to asign events to correct pages
  $last_page_ts = 0;
  foreach ($steps as $i => $step) {
    $ts = $step['time'];
    if ($step['type'] == 'page') {
      $last_page_ts = $ts;
    }
    if ($step['type'] == 'event') {
      if ($ts == $last_page_ts) {
        if (substr($step['category'], 0, 16) == 'Form submission:') {
          $steps[$i]['time'] -= 1;
        }
      }
    }
  }

  uasort($steps, '_intel_sort_session_steps');


  $rows = array();
  $start_time = 0;
  $last_page_ts = 0;
  foreach ($steps as $i => $step) {
    $ts = $step['time'];
    if (!$start_time) {
      $start_time = $ts;
    }
    $row = array();
    $event_rows = array();
    $time = $ts - $start_time;
    $tf = ($time > 3600) ? 'H:i:s' : 'i:s';
    if ($step['type'] == 'page') {
      $rows[] = array(
        '+' . date($tf, $time),
        Intel_Df::l($step['pageTitle'], '//' . $step['hostname'] . $step['pagePath']),
        '-',
      );
      $last_page_ts = $ts;
    }
    else if ($step['type'] == 'event') {
      $class = 'event';
      if (substr($step['category'], -1) == '!') {
        $class .= ' valued-event';
      }
      else if (substr($step['category'], -1) == '+') {
        $class .= ' goal-event';
      }
      $rows[] = array(
        ($ts == $last_page_ts) ? '"' : '+' . date($tf, $time),
        '<span class="' . $class . '">' . $step['category'] . ': ' . $step['action'] . '</span>',
        (!empty($step['value'])) ? number_format($step['value'], 2) : '-',
      );
    }
    if (!empty($event_rows)) {
      $rows = array_merge($rows, $event_rows);
    }
  }
  $vars = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(
      'class' => array(
        'visit-steps'
      )
    ),
  );
  return Intel_Df::theme('table', $vars);
}

function template_preprocess_intel_visitor_visits_table(&$variables) {
  $vdata = $variables['entity']->data;
  $variables['title'] = '';
  $variables['markup'] = '';
  // generate visits table
  if (!empty($vdata['analytics_visits'])) {
    $rows = array();
    uasort($vdata['analytics_visits'], function ($a, $b) {
        if (!isset($a['time']) || !isset($b['time'])) {
          return 1;
        }
        return ($a['time'] < $b['time']) ? 1 : -1;
      }
    );
    foreach ($vdata['analytics_visits'] AS $i => $visit) {
      if (substr($i, 0, 1) == '_') {
        continue;
      }
      $medium = isset($visit['trafficsource']['medium']) ? $visit['trafficsource']['medium'] : Intel_Df::t('(not set)');
      $source = isset($visit['trafficsource']['source']) ? $visit['trafficsource']['source'] : Intel_Df::t('(not set)');
      $score = !empty($visit['score']) ? $visit['score'] : 0;
      $rows[] = array(
        isset($visit['time']) ? Intel_Df::format_date($visit['time'], 'medium') : Intel_Df::t('(not set)'),
        $medium . ' / ' . $source,
        $visit['entrance']['pageviews'],
        number_format($score, 2),
        Intel_Df::l(Intel_Df::t('view'), $variables['entity']->uri() . '/clickstream', array('query' => array('visit-count' => $i))),
      );
    }
    if (count($rows)) {
      $tvars = array();
      $tvars['rows'] = $rows;
      $tvars['header'] = array(
        Intel_Df::t('Visit time'),
        Intel_Df::t('Traffic source'),
        Intel_Df::t('Pageviews'),
        Intel_Df::t('Value'),
        Intel_Df::t('Ops'),
      );
      $table = Intel_Df::theme('table', $tvars);
      $variables['title'] = Intel_Df::t('Recent site visits') . ((count($rows) > 9) ? ' (last 10)' : '');
      $variables['markup'] = $table;
    }
  }
}

function theme_intel_property_browser_environment($variables) {
  if (!empty($variables['entity'])) {
    $entity = $variables['entity'];
    $env = $entity->getVar('data', 'environment');
  } else if (!empty($variables['environment'])) {
    $env = $variables['environment'];
  }

  if (empty($env)) {
    return;
  }
  $items = array();
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('Browser'), $env['browser'] . " v" . $env['browserVersion']);
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('OS'), $env['operatingSystem'] . " " . $env['operatingSystemVersion']);
  if (!empty($env['mobileDeviceInfo'])) {
    $items[] = intel_visitor_format_profile_item(Intel_Df::t('Mobile device'), $env['mobileDeviceInfo']);
  }
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('Screen'), $env['screenResolution']);
  $items[] = intel_visitor_format_profile_item(Intel_Df::t('Language'), $env['language']);

  $output = Intel_Df::theme('intel_visitor_profile_item_list', array('items' => $items));
  return $output;
}

function template_process_intel_browser_environment_block(&$variables) {
  $variables['markup'] = Intel_Df::theme('intel_browser_environment', $variables);
  if (empty($variables['markup'])) {
    // remove title to clear block
    $variables['title'] = '';
    return;
  }
  if (empty($variables['title'])) {
    $variables['title'] = Intel_Df::t('Browser environment');
  }
  if (!isset($variables['attributes']['class'])) {
    $variables['attributes']['class'] = array();
  }
  $variables['attributes']['class'][] = 'card';
  $variables['attributes']['class'][] = 'browser-environment';
  $variables['attributes'] = Intel_Df::drupal_attributes($variables['attributes']);
}

function theme_intel_property_trafficsource($variables) {
  $output = '';
  if (!empty($variables['entity'])) {
    $entity = $variables['entity'];
    $source = $entity->getVar('data', 'analytics_session.trafficsource');
    if (isset($source[0])) {
      $source = $source[0];
    }
    d($source);
  }
  else if (!empty($variables['trafficsource'])) {
    $source = $variables['trafficsource'];
  }

  $items = array();
  $tldata_link = '';
  //$tlreferrer = '';
  $ref_alts = array(
    'http://google.com/search?q=(not provided)' => 'http://google.com/search',
    'http://facebook.com' => 'http://www.facebook.com'
  );

  if ($source['medium'] == '(none)') {
    $items[] = intel_visitor_format_profile_item(Intel_Df::t("Source"), $source['source']);
  }
  elseif ($source['medium'] == 'referral') {
    $items[] = intel_visitor_format_profile_item(Intel_Df::t("Source"), $source['medium']);
  }
  else {
    $items[] = intel_visitor_format_profile_item(Intel_Df::t("Source"), $source['medium'] . ' / ' . $source['source']);
  }
  if ($source['medium'] != '(none)') {
    if ($source['medium'] == 'organic') {
      $keyword = $source['keyword'];
      if ($source['source'] == 'google') {
        $keyword = Intel_Df::l($keyword, 'http://google.com/search?q=' . $keyword, array('attributes' => array('target' => $source['source'])));
      }
      $items[] = intel_visitor_format_profile_item(Intel_Df::t("Keyword"), $keyword);
    }
    if ($source['medium'] == 'referral') {
      $url = $source['source'] . $source['referralPath'];
      $l = Intel_Df::l($url, "http://" . $url, array('attributes' => array('target' => $source['source'])));
      $item[] = intel_visitor_format_profile_item(Intel_Df::t("Path"), $l);
    }
    if ($source['socialNetwork'] != '(not set)') {
      $item[] = intel_visitor_format_profile_item(Intel_Df::t("Social network"), $source['socialNetwork']);
    }
    if ($source['campaign'] != '(not set)') {
      $item[] = intel_visitor_format_profile_item(Intel_Df::t("Campaign"), $source['campaign']);
    }
  }

  return Intel_Df::theme('intel_visitor_profile_item_list', array('items' => $items));
}

function template_process_intel_trafficsource_block(&$variables) {
  $variables['markup'] = Intel_Df::theme('intel_trafficsource', $variables);
  if (empty($variables['title'])) {
    $variables['title'] = Intel_Df::t('Traffic source');
  }
  if (!isset($variables['attributes']['class'])) {
    $variables['attributes']['class'] = array();
  }
  $variables['attributes']['class'][] = 'card';
  $variables['attributes']['class'][] = 'trafficsource';
  $variables['attributes'] = Intel_Df::drupal_attributes($variables['attributes']);
}