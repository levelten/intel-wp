<?php
/**
 * @file
 * Functions to support extended Google Analytics data.
 *
 * @author Tom McCracken <tomm@getlevelten.com>
 */


/**
 * Loads a goal or all goals if no key is provided.
 *
 * If options.refresh is set, will also check if all goals are saved in GA and
 * add intel goal to ga if not.
 *
 * @param string $name
 * @param array $options
 * @return int|mixed|void
 */
function intel_goal_load($key = '', $options = array()) {
  $index_by = 'name';
  if (!empty($options['index_by'])) {
    $index_by = $options['index_by'];
  }
  else if ($key && is_numeric($key)) {
    $index_by = 'ga_id';
  }
  $goal_static = Intel_Df::drupal_static(__FUNCTION__, array());

  $rebuild_cache = 0;

  //$goals = get_option('intel_goals', array());

  // if refresh, sync goals with ga_goals
  // first, update ga goals with goals data, then check if any new data is in
  // ga that needs to be updated in goal data.
  if (!empty($options['refresh'])) {
    $op_meta = get_option('intel_option_meta', array());
    $time = is_numeric($options['refresh']) ? $options['refresh'] : 0;
    $ga_goals_updated = !empty($op_meta['ga_goals_updated']) ? $op_meta['ga_goals_updated'] : 0;
    if ((time() - $ga_goals_updated) > $time) {
      //$goals = intel_sync_goals_ga_goals();
      $goal_static['ga_id'] = intel_sync_goals_ga_goals();
      $op_meta['ga_goals_updated'] = time();
      update_option('intel_option_meta', $op_meta);
      $rebuild_cache = 1;
    }
  }

  if (!isset($goal_static['ga_id'])) {
    $goal_static['ga_id'] = get_option('intel_goals', array());
  }

  if (!isset($goal_static['name']) || $rebuild_cache) {
    $goal_static['name'] = array();
    foreach ($goal_static['ga_id'] as $id => $goal) {
      $goal_static['name'][$goal['name']] = $goal;
    }
  }

  if (!empty($key) && !empty($goal_static[$index_by])) {
    return !empty($goal_static[$index_by][$key]) ? $goal_static[$index_by][$key] : 0;
  }
  return !empty($goal_static[$index_by]) ? $goal_static[$index_by] : array();
}

function intel_goal_const($goal, $ga_goal = array()) {
  if (!empty($ga_goal)) {
    if (!isset($goal['title']) && isset($ga_goal['name'])) {
      $goal['title'] = $ga_goal['name'];
    }
    if (!isset($goal['ga_id']) && isset($ga_goal['id'])) {
      $goal['ga_id'] = $ga_goal['id'];
    }
    if (!isset($goal['type']) && isset($ga_goal['type'])) {
      $goal['type'] = !intel_is_intl_goal($ga_goal) ? $ga_goal['type'] : 'INTL';
    }
    if (!isset($goal['value']) && isset($ga_goal['value'])) {
      $goal['value'] = $ga_goal['value'];
    }
  }

  if (empty($goal['title'])) {
    return FALSE;
  }

  // if not ga_id set, find first available id
  if (empty($goal['ga_id'])) {
    $ga_goals = intel_ga_goal_load();

    for ($i = 1; $i <= 20; $i++) {
      if (!isset($ga_goals["$i"])) {
        $goal['ga_id'] = "$i";
        break;
      }
    }
  }

  if (empty($goal['ga_id'])) {
    return FALSE;
  }

  $defaults = array(
    'ga_id' => $goal['ga_id'],
    'name' => str_replace('-', '_', strtolower(Intel_Df::drupal_clean_css_identifier($goal['title']))),
    'title' => $goal['title'],
    'description' => '',
    'type' => 'INTL',
    'value' => 100,
    'context' => array(
      'general' => 1,
      'submission' => 1,
      'phonecall' => 1,
    ),
  );
  $goal = Intel_Df::drupal_array_merge_deep($defaults, $goal);

  return $goal;
}

/**
 * Saves an Intelligence goal and saves goal to GA if needed.
 *
 * For goals already in ga_goals, set the ga_id property.
 * To create a new Intel goal, don't set ga_id and next ga id will be used.
 *
 *
 * @param $goal
 * @param array $options
 * @return array|bool
 */
function intel_goal_save($goal, $options = array()) {
  $goals = intel_goal_load(null, array('index_by' => 'ga_id'));
  $ga_goals = intel_ga_goal_load();

  // if goal already exists, merge in existing properties
  if (isset($goal['ga_id']) && isset($goals[$goal['ga_id']])) {
    $goal = Intel_Df::drupal_array_merge_deep($goals[$goal['ga_id']], $goal);
  }

  // if ga goal exists, set ga_goal and use it to set the type if not already
  // set
  if (!empty($goal['ga_id']) && !empty($ga_goals[$goal['ga_id']])) {
    $ga_goal = $ga_goals[$goal['ga_id']];
    if (empty($goal['type']) && !empty($ga_goal['type'])) {
      if (intel_is_intl_goal($ga_goal)) {
        $goal['type'] = 'INTL';
      }
      else {
        $goal['type'] = $ga_goal['type'];
      }
    }
    if (!isset($goal['value']) && isset($ga_goal['value'])) {
      $goal['value'] = $ga_goal['value'];
    }
  }

  // fill in missing defaults
  $goal = intel_goal_const($goal);

  // goal is type intel, save to ga
  if (!isset($goal['type']) || $goal['type'] == 'INTL' || $goal['type'] == 'INTEL' ) {
    // if goal type is Intel goal, save to ga goal also
    $ga_goal = array(
      'id' => $goal['ga_id'],
      'name' => $goal['title'],
      'type' => $goal['type'],
    );
    $ga_goal = intel_ga_goal_save($ga_goal);
  }

  $goals[$goal['ga_id']] = $goal;
  update_option('intel_goals', $goals);

  return $goal;
}

function intel_ga_goal_load($name = '', $options = array()) {

  $op_meta = get_option('intel_option_meta', array());

  // refresh if goal have never been updated
  if (empty($op_meta['ga_goals_updated'])) {
    $options['refresh'] = 1;
  }

  // cache for one day unless requested
  if (!isset($options['refresh'])) {
    $options['refresh'] = 86400;
  }

  // load goals from ga
  if (!empty($options['refresh'])) {
    $time = is_numeric($options['refresh']) ? $options['refresh'] : 0;
    $ga_goals_updated = !empty($op_meta['ga_goals_updated']) ? $op_meta['ga_goals_updated'] : 0;
    if ((time() - $ga_goals_updated) > $time) {

      include_once INTEL_DIR . 'includes/intel.imapi.inc';

      try {
        $ga_goals = intel_imapi_ga_goal_get();
        update_option('intel_ga_goals', $ga_goals);
        $op_meta['ga_goals_updated'] = time();
        update_option('intel_option_meta', $op_meta);
      }
      catch (Exception $e) {
        Intel_Df::drupal_set_message($e->getMessage(), 'error');
        $ga_goals = array();
      }
    }
  }

  if (!isset($ga_goals)) {
    $ga_goals = get_option('intel_ga_goals', array());
  }

  if (!empty($name)) {
    return !empty($ga_goals[$name]) ? $ga_goals[$name] : 0;
  }
  return $ga_goals;
}

function intel_ga_goal_const($ga_goal) {
  if (empty($ga_goal['name'])) {
    return FALSE;
  }
  // if not id set, find first available id
  if (empty($ga_goal['id'])) {
    $ga_goals = intel_ga_goal_load();

    for ($i = 1; $i <= 20; $i++) {
      if (!isset($ga_goals["$i"])) {
        $ga_goal['id'] = "$i";
        break;
      }
    }
  }

  if (empty($ga_goal['id'])) {
    return FALSE;
  }

  $defaults = array(
    'id' => $ga_goal['id'],
    'name' => $ga_goal['name'],
    'type' => 'EVENT',
    'active' => TRUE,
    'value' => 0,
    'details' => array(
      'useEventValue' => TRUE,
    ),
  );
  $ga_goal = Intel_Df::drupal_array_merge_deep($defaults, $ga_goal);
  if (empty($ga_goal['details']['conditions'])) {
    $ga_goal['details']['conditions'] = array(
      array(
        'type' => 'CATEGORY',
        'matchType' => 'REGEXP',
        'expression' => $ga_goal['name'] . '\+$',
      )
    );
  }
  return $ga_goal;
}

function intel_ga_goal_save($ga_goal, $options = array()) {
  include_once INTEL_DIR . 'includes/intel.imapi.inc';
  if (empty($ga_goal['name'])) {
    return FALSE;
  }

  $is_intel_goal = 0;
  $ga_goals = intel_ga_goal_load(NULL, $options);
  // determine if goal id exists in
  $ga_goal0 = 0;
  if (isset($ga_goal['id']) && isset($ga_goals[$ga_goal['id']])) {
    $ga_goal0 = $ga_goals[$ga_goal['id']];
  }

  // data needs to be merged in from data in GA or defaults
  // for a new goal
  if ($ga_goal['type'] == 'INTL' || $ga_goal['type'] == 'INTEL' ) {
    if (!empty($ga_goal0)) {
      $ga_goal = Intel_Df::drupal_array_merge_deep($ga_goal0, $ga_goal);
      $ga_goal['details']['conditions'][0]['expression'] = $ga_goal['name'] . '\+$';
    }
    else {
      $ga_goal = intel_ga_goal_const($ga_goal);
    }
    $ga_goal['type'] = 'EVENT';
  }

  // if intel goal, check if should be saved
  if (intel_is_intl_goal($ga_goal)) {
    $update = 0;
    if (empty($ga_goal0)) {
      intel_imapi_ga_goal_insert($ga_goal);
    }
    // check if original goal and new goal are equivalent, if not save to GA.
    elseif (
      $ga_goal['name'] != $ga_goal0['name']
      || !intel_is_intl_goal($ga_goal0)
    ) {
      intel_imapi_ga_goal_update($ga_goal);
    }
  }

  $ga_goals[$ga_goal['id']] = $ga_goal;
  update_option('intel_ga_goals', $ga_goals);
  return $ga_goal;
}

function intel_sync_goals_ga_goals() {

  $goals = get_option('intel_goals', array());
  $ga_goals = intel_ga_goal_load(null, array('refresh' => 1));
  $processed = array();

  if (!empty($_GET['debug'])) {
    intel_d($goals);//
    intel_d($ga_goals);//
  }
  // first update ga_goals to match what is in goals data
  foreach ($goals as $id => $goal) {

    // check if goal does not exist in GA
    if (!isset($ga_goals[$id])) {
      // if intel goal, save it to ga
      if ($goal['type'] == 'INTL' || $goal['type'] == 'INTEL') {
        $ga_goal = array(
          'id' => $goal['ga_id'],
          'name' => $goal['title'],
          'type' => $goal['type'],
        );
        $ga_goal = intel_ga_goal_save($ga_goal);
        $processed[$id] = 1;
      }
      else {
        // if non intel goal, remove it from goals list
        $processed[$id] = 1;
        unset($goals[$id]);
      }
    }
    else {
      $ga_goal = $ga_goals[$id];
      if ($goal['type'] == 'INTL' || $goal['type'] == 'INTEL') {
        if (
          $ga_goal['name'] != $goal['title']
          || !intel_is_intl_goal($ga_goal)
        ) {
          $ga_goal = array(
            'id' => $goal['ga_id'],
            'name' => $goal['title'],
            'type' => $goal['type'],
          );
          $ga_goal = intel_ga_goal_save($ga_goal);
        }
        $processed[$id] = 1;
      }
    }
  }

  // second update goals list for any goals added in ga
  $ga_goals = intel_ga_goal_load();
  if (!empty($_GET['debug'])) {
    intel_d($goals);//
    intel_d($ga_goals);//
    intel_d($processed);//
  }
  foreach ($ga_goals as $id => $ga_goal) {
    if (!empty($processed[$id])) {
      continue;
    }
    if (!intel_is_intl_goal($ga_goal)) {
      if (!isset($goals[$id])) {
        $goals[$id] = intel_goal_const(array(), $ga_goal);
      }
      else {
        $goals[$id]['title'] = $ga_goal['name'];
        $goals[$id]['type'] = $ga_goal['type'];
        $goals[$id]['value'] = $ga_goal['value'];
      }
    }
    elseif (empty($goals[$id])) {
      // unset 0 value in GA so default intel goal value is used
      unset($ga_goal['value']);
      $goals[$id] = intel_goal_const(array(), $ga_goal);
    }
  }

  update_option('intel_goals', $goals);
  return $goals;
}

function intel_is_intl_goal($ga_goal) {
  if (!empty($ga_goal['type']) && $ga_goal['type'] == 'EVENT') {
    if (!empty($ga_goal['details']['conditions'])) {
      $cat_cond = array();
      foreach ($ga_goal['details']['conditions'] as $v) {
        if ($v['type'] == 'CATEGORY') {
          $cat_cond = $v;
        }
      }
      if (!empty($cat_cond)) {
        if (!empty($cat_cond['matchType']) && !empty($cat_cond['expression']) && $cat_cond['matchType'] == 'REGEXP') {
          $name = substr($cat_cond['expression'], 0, -3);
          $term = substr($cat_cond['expression'], -3);
          if ($term == '\+$' && $name == $ga_goal['name']) {
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}

function intel_get_goal_categories() {
  $goal_categories = array(
    'event' => Intel_Df::t('Event'),
    'submission' => Intel_Df::t('Event'),
  );
  $goal_categories = apply_filters('intel_goal_categories', $goal_categories);
  return $goal_categories;
}

/**
 * types = flag, list, scalar, vector
 */
add_filter('intel_intel_event_info', 'intel_get_intel_event_info_default');
function intel_get_intel_event_info_default($event = array()) {
  $scripts_enabled = get_option('intel_intel_scripts_enabled', array());

  /*
  $event['session_stick'] = array(
    'title' => Intel_Df::t('Session stick'),
    'description' => Intel_Df::t('When a visitor visits a second page or triggers a interaction event.'),
    'mode' => 'valued',
    'ga_event_auto' => array(
      'description' => Intel_Df::t('Automatically generated by main Intelligence script.'),
    ),
    'trigger_auto' => array(
      'description' => Intel_Df::t('Automatically triggered after a second page hit or interaction event is triggered.'),
    ),
    'availability_auto' => array(
      'description' => Intel_Df::t('Automatically available on all pages.'),
    ),
    //'overridable' => 1,
  );
  */

  $event['form_submission'] = array(
    'title' => Intel_Df::t('Form submission'),
    'description' => Intel_Df::t('General form submission'),
    'valued_event' => 1,
    'value' => 10,
    'overridable' => 0,
    'ga_event_auto' => array(
      'description' => Intel_Df::t('Automatically generated by form support add-ons.'),
    ),
    'trigger_auto' => array(
      'description' => Intel_Df::t('Automatically triggered after form submission by form support add-ons.'),
    ),
    'availability_auto' => array(
      'description' => Intel_Df::t('Automatically available on all supported form pages.'),
    ),
    'overridable' => 0,
  );

  // event mode should be admin configurable
  $event['wp_comment_submission'] = array(
    'title' => Intel_Df::t('WP comment submission'),
    'category' => Intel_Df::t('Comment submission'),
    'description' => Intel_Df::t('Comment submitted via WordPress'),
    'mode' => 'valued',
    'value' => 10,
    'trigger_auto' => array(
      'description' => Intel_Df::t('Triggered automatically when a WP comment is submitted.')
    ),
    'availability_auto' => array(
      'description' => Intel_Df::t('Automatically available on pages with WP comments.'),
    ),
    'enable' => 1,
    'js_setting' => 1,
  );

  if (intel_is_extended()) {
    $event['wp_search_form_submission'] = array(
      'title' => Intel_Df::t('WP Search form submission'),
      'description' => Intel_Df::t('Triggered on site search submission'),
      'category' => Intel_Df::t('Search form submission'),
      'value' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically generated upon site search submission.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered after upon site search submission.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically available on site search results page.'),
      ),
      'overridable' => 0,
    );

    $event['wp_search_result_click'] = array(
      'title' => Intel_Df::t('WP Search result click'),
      'description' => Intel_Df::t('Triggered by click on site search results link'),
      'category' => Intel_Df::t('Search result click'),
      'value' => 0,
      'selector' => 'article a',
      'selector_not' => '.post-edit-link',
      'on_event' => 'click',
      'transport' => 'beacon',
      'overridable' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically generated on site search results links.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically bound to trigger when site search results links are clicked.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically available on site search results page.'),
      ),
      'overridable' => 0,
    );
  }


  if (intel_is_extended()) {
    $event['landing_page_view'] = array(
      'title' => Intel_Df::t('Landing page view'),
      'description' => Intel_Df::t('Landing page pageview'),
      //'event' => 'pageshow',
    );
    $event['landing_page_conversion'] = array(
      'title' => Intel_Df::t('Landing page conversion'),
      'description' => Intel_Df::t('Form submission from a landing page'),
      'valued_event' => 1,
      'value' => 0,
    );
    $event['cta_view'] = array(
      'title' => Intel_Df::t('CTA view'),
      'description' => Intel_Df::t('Call to action impression'),
    );
    $event['cta_click'] = array(
      'title' => Intel_Df::t('CTA click'),
      'description' => Intel_Df::t('Call to action is clicked'),
      'valued_event' => 1,
      'value' => 0,
    );
    $event['cta_conversion'] = array(
      'title' => Intel_Df::t('CTA conversion'),
      'description' => Intel_Df::t('Conversion after CTA is clicked'),
      'valued_event' => 1,
      'value' => 0,
    );
  }

  /*
  $event['cta_click'] = array(
    'title' => Intel_Df::t('CTA click'),
    'description' => Intel_Df::t('Call to action is clicked'),
    'mode' => 'valued',
    //'valued_event' => 1,
    'value' => 0,
    'selector' => '.track-cta-click',
    'on_event' => 'click',
    'enable_all_pages' => 1,
    'js_settings' => 1,
  );
  */

  if (intel_is_intel_script_enabled('linktracker')) {
    $event['linktracker_mailto_click'] = array(
      'title' => Intel_Df::t('Mailto link click'),
      'description' => Intel_Df::t('Link to mailto: is clicked'),
      'category' => Intel_Df::t('Mailto link click'),
      'mode' => '',
      'value' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically configured by Link Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered when link href starting with mailto: is clicked.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically enabled on all pages.'),
      ),
      //'selector' => 'a:track-link-mailto',
      //'on_event' => 'click',
      'js_setting' => 1, // processed via _linktracker.js
      'overridable' => 0,
    );

    $event['linktracker_tel_click'] = array(
      'title' => Intel_Df::t('Tel link click'),
      'description' => Intel_Df::t('Link to tel: is clicked'),
      'mode' => '',
      'category' => Intel_Df::t('Tel link click'),
      'value' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically configured by Link Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered when link href starting with tel: is clicked.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically enabled on all pages.'),
      ),
      //'selector' => 'a:track-link-tel',
      //'on_event' => 'click',
      'js_setting' => 1, // processed via _linktracker.js
    );

    $event['linktracker_download_click'] = array(
      'title' => Intel_Df::t('Download link click'),
      'description' => Intel_Df::t('Link to file download link is clicked'),
      'mode' => '',
      'category' => Intel_Df::t('Download link click'),
      'value' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically configured by Link Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered when link href ending in file download extensions is clicked.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically enabled on all pages.'),
      ),
      //'selector' => 'a:track-link-download',
      //'on_event' => 'click',
      'js_setting' => 1,
    );

    $event['linktracker_external_click'] = array(
      'title' => Intel_Df::t('External link click'),
      'description' => Intel_Df::t('Link to external URL is clicked'),
      'mode' => '',
      'category' => Intel_Df::t('External link click'),
      'value' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically configured by Link Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Triggered when a link to an external website is clicked.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically enabled on all pages.'),
      ),
      'js_setting' => 1,
    );

    $event['linktracker_internal_click'] = array(
      'title' => Intel_Df::t('Internal link click'),
      'description' => Intel_Df::t('Link to internal URL is clicked'),
      'mode' => '',
      'category' => Intel_Df::t('Internal link click'),
      'value' => 0,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically configured by Link Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Triggered when a link with the class track-link-internal is clicked.'),
      ),
      'availability_auto' => array(
        'description' => Intel_Df::t('Automatically enabled on all pages.'),
      ),
      'js_setting' => 1,
    );
  }

  if (intel_is_intel_script_enabled('pagetracker')) {
    $event['pagetracker_page_consumed'] = array(
      'title' => Intel_Df::t('Page consumed'),
      'description' => Intel_Df::t('When visitors read a significant amount of page content'),
      'mode' => '',
      'valued_event' => 1,
      'value' => 0.1,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically set by Page Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggers on page unload set in Page Tracker script.'),
      ),
      'js_setting' => 1, // processed via _linktracker.js
      'overridable' => 0,
    );

    $event['pagetracker_page_time'] = array(
      'title' => Intel_Df::t('Page time'),
      'description' => Intel_Df::t('Sends visible time on page values when page unloads'),
      'mode' => '',
      'value' => 0,
      'value_format' => 'time',
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically set by Page Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggers on page unload set in Page Tracker script.'),
      ),
      'js_setting' => 0, // processed via _linktracker.js
      'overridable' => 0,
    );

    $event['pagetracker_page_scroll'] = array(
      'title' => Intel_Df::t('Page scroll'),
      'description' => Intel_Df::t('Sends max page  scroll depth when page unloads'),
      'mode' => '',
      'value' => 0,
      'value_format' => 'percent',
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically set by Page Tracker script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggers on page unload set in Page Tracker script.'),
      ),
      'js_setting' => 0, // processed via _linktracker.js
      'overridable' => 0,
    );
  }

  if (intel_is_intel_script_enabled('socialtracker')) {

    $event['socialtracker_social_share_click'] = array(
      'title' => Intel_Df::t('Social share click'),
      //'category' => Intel_Df::t('Social share'),
      'description' => Intel_Df::t('Click on social share button'),
      'mode' => 'valued',
      'valued_event' => 1,
      'value' => 10,
      'selector' => '.track-social-share-click',
      'on_event' => 'click',
      'enable' => 1,
      'overridable' => array(
        'selector' => 1,
      ),
      'social_action' => 'share',
      'trigger_alter_callback' => 'socialtracker:eventHandlerAlter',
      'trigger_callback' => 'socialtracker:eventHandler',
      //'js_setting' => 1,
    );

    $event['socialtracker_social_like_click'] = array(
      'title' => Intel_Df::t('Social like click'),
      //'category' => Intel_Df::t('Social share'),
      'description' => Intel_Df::t('Click on social like button'),
      'mode' => 'valued',
      'valued_event' => 1,
      'value' => 10,
      'selector' => '.track-social-like-click',
      'on_event' => 'click',
      'enable' => 1,
      'overridable' => array(
        'selector' => 1,
      ),
      'social_action' => 'like',
      'trigger_alter_callback' => 'socialtracker:eventHandlerAlter',
      'trigger_callback' => 'socialtracker:eventHandler',
      //'js_setting' => 1,
    );

    $event['socialtracker_social_follow_click'] = array(
      'title' => Intel_Df::t('Social follow click'),
      //'category' => Intel_Df::t('Social share'),
      'description' => Intel_Df::t('Click on social follow button'),
      'mode' => 'valued',
      'valued_event' => 1,
      'value' => 10,
      'selector' => '.track-social-follow-click',
      'on_event' => 'mouseover',
      'enable' => 1,
      'overridable' => array(
        'selector' => 1,
      ),
      'social_action' => 'follow',
      'trigger_alter_callback' => 'socialtracker:eventHandlerAlter',
      'trigger_callback' => 'socialtracker:eventHandler',
      //'js_setting' => 1,
    );

    $event['socialtracker_social_profile_click'] = array(
      'title' => Intel_Df::t('Social profile click'),
      //'category' => Intel_Df::t('Social profile'),
      'description' => Intel_Df::t('Click on button to visit social profile'),
      'mode' => 'valued',
      'valued_event' => 1,
      'value' => 10,
      'selector' => '.track-social-profile-click',
      'on_event' => 'click',
      'transport' => 'beacon',
      'enable' => 1,
      'overridable' => array(
        'selector' => 1,
      ),
      'social_action' => 'profile visit',
      'trigger_alter_callback' => 'socialtracker:eventHandlerAlter',
      'trigger_callback' => 'socialtracker:eventHandler',
      //'js_setting' => 1,
    );

  }

  /*
  $event['phone_call'] = array(
    'title' => Intel_Df::t('Phone call'),
    'description' => Intel_Df::t('General phone call'),
    'valued_event' => 1,
    'value' => 25,
    'overridable' => 0,
  );
  */

  if (intel_is_intel_script_enabled('addthis')) {
    $event['addthis_social_share_click'] = array(
      'title' => Intel_Df::t('AddThis social share click'),
      'category' => Intel_Df::t('Social share click'),
      'description' => Intel_Df::t('Click on social sharing widget'),
      'valued_event' => 1,
      'value' => 10,
      'js_setting' => 1,
    );
    $event['addthis_social_share_clickback'] = array(
      'title' => Intel_Df::t('AddThis social share clickback'),
      'category' => Intel_Df::t('Social share clickback'),
      'description' => Intel_Df::t('Visitor referred from a social share'),
      'valued_event' => 1,
      'value' => 0,
      'js_setting' => 1,
    );
    $event['addthis_social_follow_click'] = array(
      'title' => Intel_Df::t('AddThis social profile click'),
      'category' => Intel_Df::t('Social profile click'),
      'description' => Intel_Df::t('Click on social profile follow button'),
      'valued_event' => 1,
      'value' => 10,
      'js_setting' => 1,
    );
  }

  if (intel_is_intel_script_enabled('youtube')) {
    $event['youtube_video_play'] = array(
      'title' => Intel_Df::t('YouTube video play'),
      'category' => Intel_Df::t('Video play'),
      'description' => Intel_Df::t('When a video is played'),
      'valued_event' => 1,
      'value' => 0.05,
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered by the YouTube script when the play button of the YouTube player is pressed.'),
      ),
      'js_setting' => 1,
    );
    $event['youtube_video_stop'] = array(
      'title' => Intel_Df::t('YouTube video stop'),
      'category' => Intel_Df::t('Video stop'),
      'description' => Intel_Df::t('When a video is paused or stopped'),
      'valued_event' => 0,
      'value' => 0,
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered by the YouTube script when the stop button of the YouTube player is pressed.'),
      ),
      'js_setting' => 1,
    );
    $event['youtube_video_watch'] = array(
      'title' => Intel_Df::t('YouTube video watched'),
      'category' => Intel_Df::t('Video watched'),
      'description' => Intel_Df::t('When a video has been watched. Value is the % that was watched.'),
      'valued_event' => 0,
      'value' => 0,
      'value_format' => 'percent',
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically set by the YouTube script. The event value is the percentage the video was watched.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggered by the YouTube script when a YouTube video stops playing.'),
      ),
      'js_setting' => 1,
      'overridable' => 0,
    );

    $event['youtube_video_consumed'] = array(
      'title' => Intel_Df::t('YouTube video consumed'),
      'category' => Intel_Df::t('Video consumed'),
      'description' => Intel_Df::t('When visitors watch a significant amount of a video'),
      'mode' => '',
      'valued_event' => 1,
      'value' => 0.1,
      'ga_event_auto' => array(
        'description' => Intel_Df::t('Automatically set by YouTube script.'),
      ),
      'trigger_auto' => array(
        'description' => Intel_Df::t('Automatically triggers on page unload set inYouTube script.'),
      ),
      'js_setting' => 1, // processed via _linktracker.js
      'overridable' => 0,
    );
  }




  return $event;
}



//add_filter('intel_intel_event_info', 'intel_get_goal_event_info');
function intel_get_goal_event_info($info = array()) {
  $info = array_merge($info, intel_get_event_goal_info());
  return $info;
}

function intel_get_event_goal_info($filter = '') {
  $event = array();
  //$goals = get_option('intel_event_goals', array());

  $goals = get_option('intel_goals', array());

  foreach ($goals AS $key => $goal) {
    if (empty($goal['context'])) {
      $goal['context'] = array();
    }
    foreach ($goal['context'] as $c => $v) {
      if ($v) {
        if ($filter && $filter != $c) {
          continue;
        }
        if ($c == 'general' || $c == 'phonecall') {
          continue;
        }
        $cat = 'Goal';
        $trig_by = Intel_Df::t('an event');
        $ckey = '';
        if ($c == 'submission' || $c == 'formsubmission') {
          $ckey = 'form_submission';
          $cat = Intel_Df::t('Form submission');
          $trig_by = Intel_Df::t('a form submission');
        }
        elseif ($c == 'phonecall') {
          $ckey = 'phone_call';
          $cat = Intel_Df::t('Phone call');
          $trig_by = Intel_Df::t('a phone call');
        }
        $ekey = $ckey . '__' . $goal['name'];
        $event[$ekey] = $goal;
        $event[$ekey]['mode'] = 'goal';
        $event[$ekey]['goal_name'] = $event[$ekey]['name'];
        $event[$ekey]['goal_title'] = $event[$ekey]['title'];
        $event[$ekey]['goal_description'] = $event[$ekey]['description'];
        $event[$ekey]['name'] = $ekey;
        $event[$ekey]['title'] = $cat . ': ' . $goal['title'];
        $event[$ekey]['category'] = $cat;
        $event[$ekey]['selectable'] = 1;
        $event[$ekey]['overridable'] = 1;
        if (empty($event[$ekey]['description'])) {
          $event[$ekey]['description'] = Intel_Df::t('Goal');
        }
        $event[$ekey]['description'] = $goal['title'] . ' ' . Intel_Df::t('goal triggered by') . " $trig_by";

        if ($ckey == 'form_submission') {
          $event[$ekey]['ga_event_auto'] = array(
            'description' => Intel_Df::t('Automatically generated by form support add-ons.'),
          );
          $event[$ekey]['trigger_auto'] = array(
            'description' => Intel_Df::t('Automatically triggered after form submission by form support add-ons.'),
          );
          $event[$ekey]['availability_auto'] = array(
            'description' => Intel_Df::t('Automatically available on all supported form pages.'),
          );
        }
        elseif ($ckey == 'phonecall') {
          $event[$ekey]['ga_event_auto'] = array(
            'description' => Intel_Df::t('Automatically generated by phone call support add-ons.'),
          );
          $event[$ekey]['trigger_auto'] = array(
            'description' => Intel_Df::t('Automatically triggered after phone call by phone call support add-ons.'),
          );
        }

        //$event[$ekey]['description'] .= ' ' . Intel_Df::t('triggered by') . " $trig_by";
      }
    }

  }
  return $event;

/*
  foreach ($goals AS $key => $goal) {
    $ekey = 'goal_' . $key;
    $event[$ekey] = $goal;
    $event[$ekey]['title'] = $event[$ekey]['category'] = Intel_Df::t('Goal') . ': ' . $goal['title'];
    $event[$ekey]['category'] .= '+';
    $event[$ekey]['selectable'] = 1;
  }

  $goals = get_option('intel_submission_goals', intel_get_submission_goals_default());

  foreach ($goals AS $key => $goal) {
    $ekey = 'submission_goal_' . $key;
    $event[$ekey] = $goal;
    $event[$ekey]['title'] = $event[$ekey]['category'] = Intel_Df::t('Form submission') . ': ' . $goal['title'];
    $event[$ekey]['category'] .= '+';
    $event[$ekey]['event'] = 'form_submission';
  }

  $goals = get_option('intel_phonecall_goals', intel_get_phonecall_goals_default());

  foreach ($goals AS $key => $goal) {
    $ekey = 'phonecall_goal_' . $key;
    $event[$ekey] = $goal;
    $event[$ekey]['title'] = $event[$ekey]['category'] = Intel_Df::t('Phone call') . ': ' . $goal['title'];
    $event[$ekey]['category'] .= '+';
    $event[$ekey]['event'] = 'phonecall';
  }
  return $event;
*/
}

function intel_format_intel_event_eventcategory($event, $goal_info) {
  $str = !empty($event['title']) ? $event['title'] : '';
  $str = !empty($event['category']) ? $event['category'] : $str;
  if (!empty($event['goal_event']) || (!empty($event['mode']) && $event['mode'] == 'goal')) {
    if (!empty($goal_info[$event['ga_id']])) {
      $str .= ': ' . $goal_info[$event['ga_id']]['title'] . '+';
    }
    elseif (!empty($goal_info[$event['goal_name']])) {
      $str .= ': ' . $goal_info[$event['goal_name']]['title'] . '+';
    }
  }
  elseif (!empty($event['valued_event']) || (!empty($event['mode']) && $event['mode'] == 'valued')) {
    $str .= '!';
  }

  return $str;
}

function intel_format_intel_event_id($event, $goal_info = array()) {
  $str = !empty($event['title']) ? $event['title'] : '';
  $str = !empty($event['category']) ? $event['category'] : $str;

  return intel_to_camelcase($str);
}

function intel_to_camelcase($string) {
  $search = array('_', '-');
  $string = str_replace($search, ' ', $string);
  $string = ucwords($string);
  $string = str_replace(' ', '', $string);
  $string = lcfirst($string);
  return $string;
}

function intel_get_intel_events_overridable_fields($event) {
  $overridable_defaults = array(
    'mode' => 1,
    'ga_id' => 1,
    'value' => 1,
    'enable' => 1,
    'enable_pages' => 1,
  );

  $overridable_all = array(
      'title' => 1,
      'description' => 1,
      'category' => 1,
      'action' => 1,
      'label' => 1,
      'value' => 1,
      'non_interaction' => 1,
      'selector' => 1,
      'on_event' => 1,
      'on_selector' => 1,
      'on_data' => 1,
      'on_data_format' => 1,
      'on_function' => 1,
      'on_function_format' => 1,
      'selector_filter' => 1,
      'callback' => 1,
      'refresh_force' => 1,
      'selectable' => 1,
  ) + $overridable_defaults;

  // a new unitialize event
  if (empty($event) || empty($event['key'])) {
    return $overridable_all;
  }

  if (!empty($event['custom'])) {
    return $overridable_all;
  }

  if (isset($event['overridable'])) {
    // if array, used to override defaults
    if (is_array($event['overridable'])) {
      return Intel_Df::drupal_array_merge_deep($overridable_defaults, $event['overridable']);
    }
    // if override set to false, set to empty array
    if (empty($v['overridable'])) {
      return array();
    }
    else {
      return $overridable_all;
    }
  }
  // if overridable field not set, initialize with defaults
  else {
    return $overridable_defaults;
  }

  return $overridable;
}

function intel_get_intel_event_info($name = NULL, $options = array()) {
  $events = &Intel_Df::drupal_static(__FUNCTION__);
  if (count($events)) {
    if (isset($name)) {
      if (isset($events[$name])) {
        return $events[$name];
      }
    }
    else {
      return $events;
    }
  }
  //$events = intel_get_intel_event_info_default();
  $events = array();
  // get event info provided by hook_intel_intel_event_info;
  $events = apply_filters('intel_intel_event_info', $events);

  $events = array_merge($events, intel_get_event_goal_info());
  $goal_info = get_option('intel_goals', array());

  foreach ($events AS $k => $v) {
    if (isset($v['static_options'])) {
      $events[$k]['options'] = $v['static_options'];
    }

    if (!empty($v['valued_event']) && empty($v['ga_id'])) {
      $events[$k]['mode'] = $v['mode'] = 'valued';
    }
    if (!empty($v['mode'])) {
      if ($v['mode'] == 'valued') {
        $events[$k]['valued_event'] = 1;
      }
    }
    else {
      $events[$k]['mode'] = '';
    }

    // construct eventCategory
    if (empty($v['event_category'])) {
      $events[$k]['event_category'] = intel_format_intel_event_eventcategory($v, $goal_info);
    }

    $events[$k]['module']  = 'intel';
    $events[$k]['key']  = $k;
  }

  $custom = get_option('intel_intel_events_custom', array());

  foreach ($custom AS $k => $v) {
    // check if custom attribute
    if (isset($events[$k])) {
      foreach ($v AS $a => $b) {
        $events[$k][$a] = $b;
      }
    }
    else {
      $custom[$k]['module']  = 'intel';
      $custom[$k]['custom'] = 1;
      $events[$k] = $custom[$k];
      if (empty($v['event_category'])) {
        $events[$k]['event_category'] = intel_format_intel_event_eventcategory($v, $goal_info);
      }
    }
    if (!empty($custom[$k]['custom_options'])) {
//sdm($custom[$k]['custom_options']);
      if (!isset($events[$k]['options'])) {
        $events[$k]['options'] = $custom[$k]['custom_options'];
      }
      else {
        $events[$k]['options'] += $custom[$k]['custom_options'];
      }
    }
    $events[$k]['key']  = $k;
  }

  /* TODO WP
  foreach (module_implements('intel_intel_event_info') AS $module) {
    $function = $module . '_intel_intel_event_info';
    $a = $function();
    if (empty($a) || !is_array($a)) {
      continue;
    }
    foreach ($a AS $k => $v) {
      $v['module'] = $module;
      $v['key']  = $k;
      $events[$k] = $v;
    }
  }

  drupal_alter('intel_intel_event_info', $events);
  END TODO WP */

  // allow plugins to alter events
  $events = apply_filters('intel_intel_event_info_alter', $events);

  uasort($events, '_intel_sort_by_eventcategory');

  // allow plugins to alter theme_info

  if (isset($name)) {
    if (isset($events[$name])) {
      return $events[$name];
    }
    else {
      return FALSE;
    }
  }
  else {
    return $events;
  }
}

/**
 * Alias of intel_get_intel_event_info for use with hook_menu autoloading
 *
 * @param $key
 * @return mixed
 */
function intel_intel_event_load($key = NULL) {
  return intel_get_intel_event_info($key);
}

function intel_intel_event_save($event) {
  if (empty($event['key'])) {
    return FALSE;
  }

  $events = get_option('intel_intel_events_custom', array());
  $events[$event['key']] = $event;

  update_option('intel_intel_events_custom', $events);
}

function intel_get_enabled_intel_events($context) {
  $events = &Intel_Df::drupal_static(__FUNCTION__);
  if (isset($events)) {
    return $events;
  }

  $events = array();
  // allow other plugins to alter enabled events
  $events = apply_filters('intel_intel_events_enabled', $events, $context);

  $e = intel_get_intel_event_info();

  // determine alias path
  $parsed_url = $context['parsed_url'];
  $to_match = array();
  $alias = add_query_arg(NULL, NULL);
  $alias = substr($alias, strlen($parsed_url['base_path']));
  $to_match[] = $alias;
  $a = explode('?', $alias);
  // check for search query and create to match without it
  if (count($a) == 2) {
    $to_match[] = $alias = $a[0];
  }
  // check if alias ends in / and create to match without it
  if (substr($alias, -1) == '/') {
    $to_match[] = substr($alias, 0, -1);
  }

  foreach ($e AS $key => $event) {
    if (!empty($event['enable_all_pages'])) {
      $events[$key] = $event;
    }
    else if (isset($event['enable'])) {
      $page_match = 0;
      if (!empty($event['enable_pages'])) {
        foreach ($to_match as $alias) {
          if (Intel_Df::drupal_match_path($alias, $event['enable_pages'])) {
            $page_match = 1;
            break;
          }
        }
      }
      if (
        ($event['enable'] && !$page_match)
        || (!$event['enable'] && $page_match)
      ) {
        $events[$key] = $event;
      }
    }
  }

  // allow other plugins to alter enabled events
  $events = apply_filters('intel_intel_events_enabled_alter', $events, $context);

  return $events;
}

/**
 * types = flag, list, scalar, vector
 */
function intel_get_visitor_attribute_info_default() {
  $attributes = array();
  /* TODO WP
  $user_roles = user_roles();
  // remove anonymous user
  unset($user_roles[1]);
  */
  $user_roles = array();

  $options = array();
  foreach ($user_roles AS $rid => $role) {
    $options[$rid] = array(
      'title' => $role,
    );
  }
  $attributes['r'] = array(
    'title' => Intel_Df::t('User roles'),
    'description' => Intel_Df::t('Set to Drupal user roles for authenticated visitors.'),
    'type' => 'list',
    'static_options' => $options,
  );
  $attributes['k'] = array(
    'title' => Intel_Df::t('Known'),
    'description' => Intel_Df::t('If a visitor has submitted an email address.'),
    'type' => 'flag',
  );
  $attributes['s'] = array(
    'title' => Intel_Df::t('Score'),
    'description' => Intel_Df::t('Sum of valued events and goals triggered by visitor.'),
    'type' => 'scalar',
    'selectable' => 1,
  );
  $attributes['l'] = array(
    'title' => Intel_Df::t('Lead score'),
    'description' => Intel_Df::t('Used to value visitors customer potential.'),
    'type' => 'scalar',
    'selectable' => 1,
  );
  $attributes['g'] = array(
    'title' => Intel_Df::t('Groups'),
    'description' => Intel_Df::t('Used to categorize visitors by a defining characteristics.'),
    'type' => 'list',
    'custom_options' => array(),
    'selectable' => 1,
  );
  $attributes['i'] = array(
    'title' => Intel_Df::t('Interests'),
    'description' => Intel_Df::t('Used to value a visitors interest in various items.'),
    'type' => 'vector',
    'custom_options' => array(),
    'selectable' => 1,
  );
  $attributes['e1l'] = array(
    'title' => Intel_Df::t('First entrance page'),
    'title_plural' => Intel_Df::t('First entrance pages'),
    'description' => Intel_Df::t('The first page hit on a visitors first visit to the site.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated in javascript.'),
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 15,
        'format' => 'single',
      )
    ),
  );
  $attributes['e1rl'] = array(
    'title' => Intel_Df::t('First referrer'),
    'title_plural' => Intel_Df::t('First referrers'),
    'description' => Intel_Df::t('The first referrer url for a visitors first visit to the site.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated in javascript.'),
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 16,
        'format' => 'single',
      )
    ),
  );
  $attributes['e1s'] = array(
    'title' => Intel_Df::t('First source'),
    'title_plural' => Intel_Df::t('First sources'),
    'description' => Intel_Df::t('The first referrer url for a visitors first visit to the site.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated in javascript.'),
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 17,
      )
    ),
  );
  $attributes['e1m'] = array(
    'title' => Intel_Df::t('First medium'),
    'title_plural' => Intel_Df::t('First mediums'),
    'description' => Intel_Df::t('The first referrer url for a visitors first visit to the site.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated in javascript.'),
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 17,
      )
    ),
  );
  return $attributes;
}

function intel_get_page_attribute_info_default() {
  $attributes = array();

  $attributes['a'] = array(
    'title' => Intel_Df::t('Author'),
    'title_plural' => Intel_Df::t('Authors'),
    'description' => Intel_Df::t('The uid of a node author.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated from entity uid.'),
    'options info callback' => 'intel_page_attribute_author_option_info',
  );
  /*
  $attributes['rt'] = array(
    'title' => Intel_Df::t('Entity type'),
    'title_plural' => Intel_Df::t('Entity types'),
    'description' => Intel_Df::t('The entity type of the page. (node, user...)'),
    'type' => 'value',
    'options_description' => Intel_Df::t('Auto generated from page entity.'),
    'options info callback' => 'intel_page_attribute_entity_type_option_info',
    'access callback' => 0,
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 6,
      )
    ),
  );
  */
  $attributes['rt2'] = array(
    'title' => Intel_Df::t('Content type'),
    'title_plural' => Intel_Df::t('Content types'),
    'description' => Intel_Df::t('Node type or entity bundle type.'),
    'type' => 'value',
    'options_description' => Intel_Df::t('Auto generated from entity type/bundle.'),
    'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 6,
      )
    ),
  );
  /*
  $attributes['rk'] = array(
    'title' => Intel_Df::t('Entity id'),
    'title_plural' => Intel_Df::t('Entity ids'),
    'description' => Intel_Df::t('The standard id for the entity, e.g. node id.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated from entity type/bundle.'),
    'access callback' => 0,
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 6,
      )
    ),
  );
  $attributes['rvi'] = array(
    'title' => Intel_Df::t('Revision id'),
    'title_plural' => Intel_Df::t('Revision ids'),
    'description' => Intel_Df::t('The revision number of an entity.'),
    'type' => 'item',
    'encode' => FALSE,
    'options_description' => Intel_Df::t('Auto generated from entity data.'),
    'access callback' => FALSE,
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 1,
      )
    ),
  );

  $attributes['rl'] = array(
    'title' => Intel_Df::t('Entity path'),
    'title_plural' => Intel_Df::t('Entity path'),
    'description' => Intel_Df::t('The Drupal system path for an entity.'),
    'type' => 'value',
    'options_description' => Intel_Df::t('Auto generated from entity data.'),
    'access callback' => FALSE,
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 6,
      )
    ),
  );
  /*
  $attributes['url'] = array(
    'title' => Intel_Df::t('URL'),
    'title_plural' => Intel_Df::t('URLs'),
    'description' => Intel_Df::t('The default url for a node or entity, using the url function.'),
    'type' => 'value',
    'options_description' => Intel_Df::t('Auto generated from entity type/bundle.'),
    'options info callback' => 'intel_page_attribute_content_type_option_info',
    'access callback' => FALSE,
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 6,
        'format' => 'single',
      )
    ),
  );
  /*
  $attributes['lang'] = array(
    'title' => Intel_Df::t('Language'),
    'title_plural' => Intel_Df::t('Languages'),
    'description' => Intel_Df::t('The language of the page.'),
    'type' => 'value',
    'options_description' => Intel_Df::t('Auto generated from entity data.'),
    //'options info callback' => 'intel_page_attribute_content_type_option_info',
    'access callback' => FALSE,
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 13,
      )
    ),
  );
  */
  /*
  $attributes['ct'] = array(
    'title' => Intel_Df::t('Content type'),
    'title_plural' => Intel_Df::t('Content types'),
    'description' => Intel_Df::t('Node type or entity bundle type.'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated from entity type/bundle.'),
    'options info callback' => 'intel_page_attribute_content_type_option_info',
  );
  $attributes['et'] = array(
    'title' => Intel_Df::t('Entity type'),
    'title_plural' => Intel_Df::t('Entity types'),
    'description' => Intel_Df::t('The entity type of the page. (node, user...)'),
    'type' => 'item',
    'options_description' => Intel_Df::t('Auto generated from page entity.'),
    'options info callback' => 'intel_page_attribute_entity_type_option_info',
  );
  */
  /*
  $attributes['i'] = array(
    'title' => Intel_Df::t('Page intent'),
    'description' => Intel_Df::t('The role a page plays on the site.'),
    'type' => 'list',
    'static_options' => intel_get_page_intents_default('config'),
    'custom_options' => array(),
    'selectable' => 1,
  );
  */
  $attributes['pd'] = array(
    'title' => Intel_Df::t('Published date'),
    'description' => Intel_Df::t('The time (unix timestamp) a node was created.'),
    'options_description' => Intel_Df::t('Auto generated from entity created date.'),
    'options info callback' => 'intel_page_attribute_published_date_option_info',
    'type' => 'value',
    'format' => 'datetimedow',
    'access callback' => '_intel_user_access_extended',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 1,
      )
    ),
  );

  $attributes['pda'] = array(
    'title' => Intel_Df::t('Published age'),
    'description' => Intel_Df::t('The time in seconds since the page was published (created).'),
    'options_description' => Intel_Df::t('Auto generated from entity created date.'),
    'options info callback' => 'intel_page_attribute_published_age_option_info',
    'type' => 'value',
    'format' => 'timeago',
    'access callback' => '_intel_user_access_extended',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 10,
        'format' => 'timeago',
      )
    ),
    'index_grouping' => array(
      0,
      86400,
      86400*7,
      86400*30,
      86400*90,
      86400*365,
    ),
  );
  /*
  $attributes['pdw'] = array(
    'title' => Intel_Df::t('Published DOW'),
    'description' => Intel_Df::t('The day of the week (integer 1-7) of the day of week a page was published.'),
    'options_description' => Intel_Df::t('Auto generated from entity created date.'),
    'options info callback' => 'intel_page_attribute_published_dow_option_info',
    'type' => 'scalar',
//    'storage' => array(
//      'analytics' => array(
//        'struc' => 'metric',
//        'index' => 5,
//      )
//    ),
  );
  $attributes['pdt'] = array(
    'title' => Intel_Df::t('Published TOD'),
    'description' => Intel_Df::t('The time in HH.MM format a page was published.'),
    'options_description' => Intel_Df::t('Auto generated from entity created date.'),
    'options info callback' => 'intel_page_attribute_published_tod_option_info',
    'type' => 'scalar',
//    'storage' => array(
//      'analytics' => array(
//        'struc' => 'metric',
//        'index' => 3,
//      )
//    ),
    'index_grouping' => array(),
  );
  for ($i = 0; $i <= 2400; $i += 100) {
    $attributes['pdt']['index_grouping'][] = $i;
  }
  /*
  $attributes['s'] = array(
    'title' => Intel_Df::t('Section'),
    'title_plural' => Intel_Df::t('Sections'),
    'description' => Intel_Df::t('Node type or entity bundle type.'),
    'type' => 'item',
    'custom_options' => array(),
    'selectable' => 1,
    'access callback' => FALSE,
  );
  */
  $attributes['c'] = array(
    'title' => Intel_Df::t('Category'),
    'title_plural' => Intel_Df::t('Categories'),
    'description' => Intel_Df::t('List of taxomony term ids.'),
    'type' => 'list',
    'options_description' => Intel_Df::t('Auto generated from terms in selected taxonomy fields.'),
    'options info callback' => 'intel_page_attribute_taxomony_term_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 1,
      )
    ),
  );
  $attributes['t'] = array(
    'title' => Intel_Df::t('Tag'),
    'title_plural' => Intel_Df::t('Tags'),
    'description' => Intel_Df::t('List of taxomony term ids.'),
    'type' => 'list',
    'options_description' => Intel_Df::t('Auto generated from terms in selected taxonomy fields.'),
    'options info callback' => 'intel_page_attribute_taxomony_term_option_info',
    'storage' => array(
      'analytics' => array(
        'struc' => 'dimension',
        'index' => 1,
      )
    ),
  );
  return $attributes;
}

/**
 * Returns page attribute definitions.
 * @param null $name
 *   Designates a specific visitor attribute to return. If null, all visitor
 *   attributes will be returned as an array.
 * @return array
 */
function intel_get_page_attribute_info($name = NULL) {
  return intel_get_attribute_info($name, 'page');
}

/**
 * Alias for intel_get_page_attribute_info() to be used with hook_menu's
 * autoloading
 */
function intel_page_attribute_load($name) {
  return intel_get_page_attribute_info($name);
}

/**
 * Returns page attribute definitions.
 * @param null $name
 *   Designates a specific visitor attribute to return. If null, all visitor
 *   attributes will be returned as an array.
 * @return array
 */
function intel_get_visitor_attribute_info($name = NULL) {
  return intel_get_attribute_info($name, 'visitor');
}

/**
 * Alias for intel_get_visitor_attribute_info() to be used with hook_menu's
 * autoloading
 */
function intel_visitor_attribute_load($name) {
  return intel_get_visitor_attribute_info($name);
}

function intel_get_attribute_info($name = NULL, $mode = 'visitor') {
  $attributes = &Intel_Df::drupal_static(__FUNCTION__);
  if (isset($attributes[$mode])) {
    if (isset($name)) {
      if (isset($attributes[$mode][$name])) {
        return $attributes[$mode][$name];
      }
    } else {
      return $attributes[$mode];
    }
  }
  $attrs = ($mode == 'page') ? intel_get_page_attribute_info_default() : intel_get_visitor_attribute_info_default();

  foreach ($attrs AS $k => $v) {
    if (isset($v['static_options'])) {
      $attrs[$k]['options'] = $v['static_options'];
    }
    $attrs[$k]['module']  = 'intel';
    $attrs[$k]['key'] = $k;
  }

  // process vocabularies set as custom attributes
  $entity_settings = intel_get_entity_settings_multi('taxonomy');
  foreach ($entity_settings AS $k => $v) {

    // check if attribute key is set or if intel_track_page_terms_visitor
    if ((isset($v[$mode . '_attribute']) && !empty($v[$mode . '_attribute']['key']))
      || (($mode == 'visitor') && !empty($v['track_page_terms_visitor']))
    ) {
      $vocab_info = taxonomy_vocabulary_machine_name_load($k);
      if (empty($vocab_info->vid)) {
        continue;
      }

      // if mode == visitor,
      if (($mode == 'visitor') && (empty($v[$mode . '_attribute']['key']))) {
        $key = !empty($v['page_attribute']['key']) ? $v['page_attribute']['key'] : 't';
        $prop = !empty($v['page_attribute']['prop']) ? $v['page_attribute']['prop'] : 'tag';

        if (!isset($v['visitor_attribute'])) {
          $v['visitor_attribute'] = array();
        }
        $v['visitor_attribute'] += $v['page_attribute'];
      }
      else {
        $key = $v[$mode . '_attribute']['key'];
        $prop = $v[$mode . '_attribute']['prop'];
      }

      $attrs[$key] = $v[$mode . '_attribute'];
      if (empty($attrs[$key]['title'])) {
        $attrs[$key]['title'] = $vocab_info->name;
      }
      if (empty($attrs[$key]['description'])) {
        $attrs[$key]['description'] = $vocab_info->description;
      }
      $attrs[$key]['options_description'] = Intel_Df::t('Generated from taxonomy vocabulary !link.',
        array(
          '!link' => l($vocab_info->name, 'admin/structure/taxonomy/' . $k . '/edit'),
        )
      );
      $attrs[$key]['key'] = $key;
      $attrs[$key]['type'] = ($mode == 'visitor') ? 'vector' : 'list';
      $attrs[$key]['module']  = 'intel';
      $attrs[$key]['source_type']  = 'taxonomy';
      $attrs[$key]['options info callback']  = 'intel_page_attribute_taxomony_term_option_info';
      $attrs[$key]['module']  = 'intel';
      $attrs[$key]['options'] = array();
      if ($prop) {
        $attrs[$key]['prop'] = $prop;
      }
      if (!isset($attrs[$key]['storage'])) {
        $attrs[$key]['storage'] = array(
          'analytics' => array(
            'struc' => 'dimension',
            'index' => 1,
          )
        );
      }
      $attrs[$key]['storage']['analytics'] = array(
        'siteType' => 'taxonomy_term',
        'siteBundle' => $vocab_info->machine_name,
      );
    }
  }

  // load custom attributes and overrides
  $custom = intel_get_page_attributes_custom_multi($mode);
  foreach ($custom AS $k => $v) {
    // check if custom attribute
    if (isset($attrs[$k])) {
      foreach ($v AS $a => $b) {
        $attrs[$k][$a] = $b;
      }
    }
    else {
      $custom[$k]['module']  = 'intel';
      $custom[$k]['key']  = $k;
      $custom[$k]['custom'] = 1;
      $attrs[$k] = $custom[$k];
    }
    if (!empty($custom[$k]['custom_options'])) {
//sdm($custom[$k]['custom_options']);
      if (!isset($attrs[$k]['options'])) {
        $attrs[$k]['options'] = $custom[$k]['custom_options'];
      }
      else {
        $attrs[$k]['options'] += $custom[$k]['custom_options'];
      }
    }
  }

  /* TODO WP
  foreach (module_implements('intel_' . $mode . '_attribute_info') AS $module) {
    $function = $module . '_intel_' . $mode . '_attribute_info';
    $a = $function();
    if (empty($a) || !is_array($a)) {
      continue;
    }
    foreach ($a AS $k => $v) {
      $v['module'] = $module;
      $v['key'] = $k;
      $attrs[$k] = $v;
    }
  }

  drupal_alter('intel_' . $mode . '_attribute_info', $attrs);
  END TODO WP */

  uasort($attrs, '_intel_sort_by_title');

  $attributes[$mode] = $attrs;

  if (isset($name)) {
    if (isset($attributes[$mode][$name])) {
      return $attributes[$mode][$name];
    }
    else {
      return FALSE;
    }
  } else {
    return $attributes[$mode];
  }
}

/**
 * Provides visitor and page attribute option meta data. Primarily used to provide
 * a human readable title for a given attribute option id.
 *
 * @param $mode
 *   sets the type of attribute, page | visitor
 * @param $attr_key
 *   The name that identifies the attribute
 * @param $option_id
 *   Attribute specific key that identifies which option id to provide info.
 *   Typically a numeric id or machine name, e.g. uid for authors, tid for tax
 *   terms, content type machine name.
 * @param array $data_options (optional)
 *
 * @return array
 *   - title: human readable label for the option
 *   - [extras]: additional data may be added as requested in $data_options
 */
function intel_get_attribute_option_info($mode, $attr_key, $option_id, $data_options = array()) {
  $info = intel_get_visitor_attribute_info();
  //dsm($info);
  if ($mode == 'page') {
    $attrInfo = intel_get_page_attribute_info($attr_key);
  }
  else {
    $attrInfo = intel_get_visitor_attribute_info($attr_key);
  }
  if (!empty($attrInfo)) {

  }

  //dsm($attrInfo);

  $data = array(
    'title' => '',
  );

  if (!empty($attrInfo['options info callback'])) {
    $data = $attrInfo['options info callback']($option_id, $data_options);
  }
  elseif (($attrInfo['module'] == 'intel') && isset($attrInfo['source_type']) && ($attrInfo['source_type'] == 'taxonomy')) {
    $term = taxonomy_term_load($option_id);
    if (!empty($term)) {
      $data['title'] = $term->name;
    }
    if (!empty($data_options['page_count'])) {
      $query = db_query("SELECT count(nid) FROM {taxonomy_index} WHERE tid = :tid", array(
        ':tid' => $option_id,
      ));

      $data['page_count'] = $query->fetchField();
    }
  }
  elseif (isset($attrInfo['options'][$option_id])) {
    $data = $attrInfo['options'][$option_id];
  }

  return $data;
}

function intel_page_attribute_author_option_info($option_id, $data_options) {

  $data = array(
    'title' => '',
  );
  if (INTEL_PLATFORM == 'wp') {
    $option_id = intval($option_id);
    if ($option_id < 0) {
      require_once INTEL_DIR . 'includes/intel.demo.inc';
      $account = intel_demo_user_load($option_id);
    }
    else {
      $account = get_userdata($option_id);
    }

    if (!empty($account)) {
      $data['title'] = $account->display_name;
      if ($option_id < 0) {
        $data['page_count'] = intel_demo_count_user_posts( $option_id, 'post');
      }
      else {
        $data['page_count'] = count_user_posts( $option_id, 'post', TRUE );
      }
    }
  }
  elseif(INTEL_PLATFORM == 'drupal') {
    $account = user_load($option_id);
    if (!empty($account)) {
      $data['title'] = format_username($account);
      $data['uri'] = 'user/' . $account->uid;
    }

    if (!empty($data_options['page_count'])) {
      $query = db_query("SELECT count(nid) FROM {node} WHERE uid = :uid", array(
        ':uid' => $option_id,
      ));

      $data['page_count'] = $query->fetchField();
    }
  }

  return $data;
}

function intel_page_attribute_content_type_option_info($option_id, $data_options) {
  $content_types = Intel_Df::drupal_static( __FUNCTION__, array());

  $data = array(
    'title' => Intel_Df::t('(unknown)'),
    'page_count' => 1,
  );

  if (empty($content_types[$option_id])) {
    $content_types[$option_id] = get_post_type_object($option_id);
    $content_types[$option_id]->count_posts_published = wp_count_posts( $option_id )->publish;
  }

  if (empty($content_types[$option_id])) {
    return $data;
  }

  $data['title'] = $content_types[$option_id]->label;
  $data['page_count'] = $content_types[$option_id]->count_posts_published;

  return $data;
  //print_r($types);
  $type = '';
  if (isset($types[$option_id])) {
    $data['title'] = $types[$option_id]->name;
    $type = $option_id;
  }
  elseif (isset($types['enterprise_' . $option_id])) {
    $data['title'] = $types['enterprise_' . $option_id]->name;
    $type = 'enterprise_' . $option_id;
  }

  if ($type && !empty($data_options['page_count'])) {
    $query = db_query("SELECT count(nid) FROM {node} WHERE type = :type", array(
      ':type' => $type,
    ));

    $data['page_count'] = $query->fetchField();
  }
  return $data;
}

// TODO: Not completed
function intel_page_attribute_entity_type_option_info($option_id, $data_options) {
  $data = array(
    'title' => '',
  );
  $types = node_type_get_types();
  //print_r($types);
  $type = '';
  if (isset($types[$option_id])) {
    $data['title'] = $types[$option_id]->name;
    $type = $option_id;
  }
  elseif (isset($types['enterprise_' . $option_id])) {
    $data['title'] = $types['enterprise_' . $option_id]->name;
    $type = 'enterprise_' . $option_id;
  }

  if ($type && !empty($data_options['page_count'])) {
    $query = db_query("SELECT count(nid) FROM {node} WHERE type = :type", array(
      ':type' => $type,
    ));

    $data['page_count'] = $query->fetchField();
  }
  return $data;
}

function intel_page_attribute_taxomony_term_option_info($option_id, $data_options) {
  $data = array(
    'title' => '',
  );

  if (INTEL_PLATFORM == 'wp') {
    $option_id = intval($option_id);
    if ($option_id < 0) {
      require_once INTEL_DIR . 'includes/intel.demo.inc';
      $term = intel_demo_term_load($option_id);
    }
    else {
      $term = get_term($option_id);
    }
    if (!empty($term)) {
      $data['title'] = $term->name;
      $data['page_count'] = $term->count;
    }

  }
  elseif (INTEL_PLATFORM == 'drupal') {
    $term = taxonomy_term_load($option_id);
    if (!empty($term)) {
      $data['title'] = $term->name;
      $data['uri'] = 'taxonomy/term/' . $option_id;
    }

    if (!empty($data_options['page_count'])) {
      $query = db_query("SELECT count(nid) FROM {taxonomy_index} WHERE tid = :tid", array(
        ':tid' => $option_id,
      ));

      $data['page_count'] = $query->fetchField();
    }
  }


  return $data;
}

function intel_page_attribute_published_date_option_info($value, $data_options) {
  $data = array();
  $count_value = '';
  $count_op = 'LIKE';
  if (substr($value, 0 , 1) == 'w') {
    $dow = array(
      Intel_Df::t('Sunday'),
      Intel_Df::t('Monday'),
      Intel_Df::t('Tuesday'),
      Intel_Df::t('Wednesday'),
      Intel_Df::t('Thursday'),
      Intel_Df::t('Friday'),
      Intel_Df::t('Saturday'),
    );
    $i = (int)substr($value, 1, 1);
    $data['title'] = $dow[$i];
    $count_value = '^[0-9]{12}' . $i . '$';
    $count_op = 'REGEXP';
  }
  elseif (substr($value, 0, 2) == 'Hi') {
    $i = substr($value, 2, 4) .
    $data['title'] = substr($value, 2, 2) . ':' . substr($value, 4, 2);
    $count_value = '^[0-9]{8}' . $i . '[0-9]$';
    $count_op = 'REGEXP';
  }
  elseif (substr($value, 0, 1) == 'H') {
    $i = substr($value, 1, 2);
    $data['title'] = substr($value, 1, 2) .  ':00 ' . Intel_Df::t('hours');
    $count_value = '^[0-9]{8}' . $i . '[0-9]{3}$';
    $count_op = 'REGEXP';
  }
  // YYYYMMDDHHMM
  elseif (strlen($value) == 8) {
    $data['title'] = substr($value, 0, 4) . '-' . substr($value, 4, 2) . '-' . substr($value, 6, 2);
    $count_value = substr($value, 0, 8) . '%';
  }
  // YYYYMM
  else {
    $data['title'] = substr($value, 0, 4) . '-' . substr($value, 4, 2);
    $count_value = substr($value, 0, 6) . '%';
  }

  if (!empty($data_options['page_count']) && $count_value) {
    $data['page_count'] = intel_entity_attr_entity_count('pd', $count_value, null, $count_op);
  }
  return $data;
}

function intel_page_attribute_published_age_option_info($option_id, $data_options) {
  $group = explode('-', $option_id);
  $titles = array(
    '0' => Intel_Df::t('Last 24 hours'),
    '86400' => Intel_Df::t('A day to a week'),
    '604800' => Intel_Df::t('A week to a month'),
    '2592000' => Intel_Df::t('A month to 3 months'),
    '7776000' => Intel_Df::t('3 months to a year'),
  );

  $data = array(
    'title' => isset($titles[$group[0]]) ? $titles[$group[0]] : $option_id,
  );

  if (!empty($data_options['page_count'])) {
    $data['page_count'] = intel_entity_attr_entity_count('pdw', (int)$option_id);
  }
  return $data;
}

function intel_page_attribute_published_dow_option_info($option_id, $data_options) {
  $dow = array(
    Intel_Df::t('Sunday'),
    Intel_Df::t('Monday'),
    Intel_Df::t('Tuesday'),
    Intel_Df::t('Wednesday'),
    Intel_Df::t('Thursday'),
    Intel_Df::t('Friday'),
    Intel_Df::t('Saturday'),
  );

  $data = array(
    'title' => isset($dow[$option_id]) ? $dow[$option_id] : $option_id,
  );

  if (!empty($data_options['page_count'])) {
      $data['page_count'] = intel_entity_attr_entity_count('pdw', (int)$option_id);
  }
  return $data;
}

function intel_page_attribute_published_tod_option_info($option_id, $data_options) {
  $group = explode('-', $option_id);

  $h0 = (int)substr($group[0], 0, -2);
  $m0 = substr($group[0], -2);
  $pf0 = 'am';
  if ($h0 > 12) {
    $h0 = $h0 - 12;
    $pf0 = 'pm';
  }

  if (isset($group[1])) {
    $h1 = (int)substr($group[1], 0, -2);
    $m1 = 59;
    if ($h1 > 12) {
      $h1 = $h1 - 12;
    }
    $d1 = " - $h1:$m1";
  }

  $data = array(
    'title' => "$h0:$m0$d1 $pf0",
  );

  if (!empty($data_options['page_count'])) {
    if (count($group) == 2) {
      $data['page_count'] = intel_entity_attr_entity_count('pdt', (int)$group[0], (int)$group[1]);
    }
    else {
      $data['page_count'] = intel_entity_attr_entity_count('pdt', (int)$group[0]);
    }
  }
  return $data;
}

function intel_get_page_intents_default($filter = 'report') {
  $intents = array();
  if ($filter == 'report') {
    $intents[''] = array(
      'title' => Intel_Df::t('(not set)'),
      'description' => Intel_Df::t('Intent is not set by page.'),
      'category' => 'system',
    );
  }
  if ($filter == 'entity_edit') {
    $intents['_default'] = array(
      'title' => Intel_Df::t('- Default -'),
      'description' => Intel_Df::t('Use the default for the entity type.'),
      'category' => 'system',
    );
  }
  $intents['a'] = array(
    'title' => Intel_Df::t('Admin'),
    'description' => Intel_Df::t('Pages used to administer the site.'),
  );
  $intents['t'] = array(
    'title' => Intel_Df::t('Attraction'),
    'description' => Intel_Df::t('Pages designed to attract people and generate buzz such as blog posts and podcasts.'),
  );
  $intents['i'] = array(
    'title' => Intel_Df::t('Information'),
    'description' => Intel_Df::t('Page that are informational such as product and services.'),
  );
  $intents['l'] = array(
    'title' => Intel_Df::t('Landing page'),
    'description' => Intel_Df::t('Page that is designed to entice visitors to submit a form or do one specific action.'),
  );
  $intents['u'] = array(
    'title' => Intel_Df::t('Utility'),
    'description' => Intel_Df::t('Support pages that should be excluded from content reports. Useful for thankyou pages.'),
  );
  return $intents;
}

function intel_get_page_intents($filter = 'report') {
  $intents = &Intel_Df::drupal_static(__FUNCTION__);
  if (!empty($intents)) {
    return $intents;
  }
  $page_attributes = intel_get_page_attribute_info();

  $page_intents = $page_attributes['i']['options'];

  uasort($page_intents, '_intel_sort_by_title');

  return $page_intents;
}


/*
function intel_visitor_attribute_load($key) {
  $attributes = intel_get_visitor_attribute_info();
  $attribute = $attributes[$key];
  $attribute['key'] = $key;
  return $attribute;
}
*/


function intel_get_phonenumbers() {
  $phonenumbers = &Intel_Df::drupal_static(__FUNCTION__);
  if (count($phonenumbers)) {
    return $phonenumbers;
  }
  $phonenumbers = get_option('intel_phonenumbers', array());
  foreach ($phonenumbers AS $name => $number) {
    $phonenumbers[$name]['custom'] = 1;
  }

  drupal_alter('intel_phonenumbers', $phonenumbers);

  return $phonenumbers;
}

function intel_phonenumber_load($key) {
  $phonenumbers = intel_get_phonenumbers();
  if (!isset($phonenumbers[$key])) {
    return FALSE;
  }
  $phonenumber = $phonenumbers[$key];
  $phonenumber['key'] = $key;
  return $phonenumber;
}

function intel_phonenumber_load_by_number($number) {
  $numbers = &Intel_Df::drupal_static(__FUNCTION__);
  if (isset($numbers[$number])) {
    return $numbers[$number];
  }
  $phonenumbers = intel_get_phonenumbers();
  foreach ($phonenumbers AS $name => $phonenumber) {
    if ($phonenumber['number'] == $number) {
      $numbers[$number] = $phonenumber;
      break;
    }
  }

  if (isset($numbers[$number])) {
    return $numbers[$number];
  }
  // number not found
  else {
    return FALSE;
  }
}

function intel_phonenumber_save($phonenumber, $key = '') {
  if (empty($key)) {
    if (!empty($phonenumber['key'])) {
      $key = $phonenumber['key'];
    }
    else {
      return FALSE;
    }
  }
  if (empty($phonenumber['title'])) {
    $phonenumber['title'] = $phonenumber['number'];
  }
  if (empty($phonenumber['number_display'])) {
    $phonenumber['number_display'] = $phonenumber['number'];
  }

  $phonenumbers = get_option('intel_phonenumbers', array());
  $phonenumbers[$key] = $phonenumber;
  $phonenumbers = update_option('intel_phonenumbers', $phonenumbers);
  return $phonenumber;
}

function intel_get_page_attributes_custom_multi($mode = 'visitor') {
  return array(); // TODO WP

  $query = db_select('variable', 'v')
    ->fields('v')
    ->condition('name', 'intel_' . $mode . '_attribute_custom_%', 'LIKE');
  $result = $query->execute();

  $data = array();
  while ($row = $result->fetchObject()) {
    $a = explode('_', $row->name);
    $data[$a[4]] = unserialize($row->value);
  }
  return $data;
}

function intel_get_entity_settings_multi($entity_type, $bundle = '') {
  return array(); // TODO WP

  $query = db_select('variable', 'v')
    ->fields('v');

  if ($bundle == '') {
    $query->condition('name', 'intel_entity_settings_' . $entity_type . '__%', 'LIKE');
  }
  else {
    $query->condition('name', 'intel_entity_settings_' . $entity_type . '__' . $bundle);
  }
  $result = $query->execute();

  $data = array();
  while ($row = $result->fetchObject()) {
    $a = explode('_', $row->name, 6);
    $data[$a[5]] = unserialize($row->value);
  }
  return $data;
}

function intel_index_encode($base10) {
  // crockford base32 encoding
  //return strtr( base_convert($base10, 10, 32), "abcdefghijklmnopqrstuv", "ABCDEFGHJKMNPQRSTVWXYZ");
  return strtolower(strtr( base_convert($base10, 10, 32), "abcdefghijklmnopqrstuv", "ABCDEFGHJKMNPQRSTVWXYZ"));
}

function intel_index_decode($base32) {
  // crockford base32 decoding
  $base32 = strtr(strtoupper($base32), "ABCDEFGHJKMNPQRSTVWXYZILO", "abcdefghijklmnopqrstuv110");

  return (int)base_convert($base32, 32, 10);
}

function _intel_sort_by_title($a, $b) {
  return ($a['title'] > $b['title']) ? 1 : -1;
}

function _intel_sort_by_category($a, $b) {
  return ($a['category'] > $b['category']) ? 1 : -1;
}

function _intel_sort_by_eventcategory($a, $b) {
  return ($a['event_category'] > $b['event_category']) ? 1 : -1;
}

function _intel_sort_by_rtime($a, $b) {
  return ($a['time'] < $b['time']) ? 1 : -1;
}

function _intel_sort_session_steps($a, $b) {
  if ($a['time'] == $b['time']) {
    // give page steps priority over all and events over goals
    if ($a['type'] == 'page') {
      return -1;
    }
    if ($a['type'] == 'event' && $b['type'] != 'page') {
      return -1;
    }
    if ($a['type'] == 'goal' && $b['type'] != 'page' && $b['type'] != 'event') {
      return -1;
    }
    return 1;
  }
  return ($a['time'] < $b['time']) ? -1 : 1;
}

function intel_init_targets() {
  $target = array();
  $target['entrances_per_month'] = array(
    'title' => Intel_Df::t('Total visitors / month'),
    'description' => Intel_Df::t('Your target for the number of total visitors you want to get to the site.'),
    'value' => 3000,
    'group' => 'site_kpi_month',
  );
  $target['leads_per_month'] = array(
    'title' => Intel_Df::t('New contacts / month'),
    'description' => Intel_Df::t('Your target for the number of contacts in a month.'),
    'value' => 30,
    'group' => 'site_kpi_month',
  );
  $target['posts_per_month'] = array(
    'title' => Intel_Df::t('Posts / month'),
    'description' => Intel_Df::t('Your target attraction, e.i. blog, posts in a month.'),
    'value' => 8,
    'group' => 'site_kpi_month',
  );


  $target['value_per_day'] = array(
    'title' => Intel_Df::t('Total value / day'),
    'description' => Intel_Df::t('Your target for the total value of all traffic to the site.'),
    'value' => 200.00,
    'group' => 'site_day',
  );
  $target['value_per_day_warning'] = array(
    'title' => Intel_Df::t('Total value / day (warning)'),
    'description' => Intel_Df::t('Your target for the total value of all traffic to the site.'),
    'value' => 20.00,
    'group' => 'site_day',
  );

  $target['entrances_per_day'] = array(
    'title' => Intel_Df::t('Total visitors / day'),
    'description' => Intel_Df::t('Your target for the number of total visitors you want to get to the site.'),
    'value' => 100.00,
    'group' => 'site_day',
  );
  $target['entrances_per_day_warning'] = array(
    'title' => Intel_Df::t('Total visitors / day (warning)'),
    'description' => Intel_Df::t('Your target for the number of total visitors you want to get to the site.'),
    'value' => 10.00,
    'group' => 'site_day',
  );

  $target['conversions_per_day'] = array(
    'title' => Intel_Df::t('Total conversions / day'),
    'description' => Intel_Df::t('Your target for the number of total visitors you want to get to the site.'),
    'value' => 4.00,
    'group' => 'site_day',
  );
  $target['conversions_per_day_warning'] = array(
    'title' => Intel_Df::t('Total conversions / day (warning)'),
    'description' => Intel_Df::t('Your target for the number of total visitors you want to get to the site.'),
    'value' => 1.00,
    'group' => 'site_day',
  );

  $target['value_per_visit'] = array(
    'title' => Intel_Df::t('Value / visit'),
    'description' => Intel_Df::t('Your target for the total value of all traffic to the site.'),
    'value' => 2.00,
    'group' => 'visit',
  );
  $target['value_per_visit_warning'] = array(
    'title' => Intel_Df::t('Value per page / day (warning)'),
    'description' => Intel_Df::t('Your target for the total value of all traffic to the site.'),
    'value' => 0.20,
    'group' => 'visit',
  );

  $target['value_per_page_per_day'] = array(
    'title' => Intel_Df::t('Value per page / day'),
    'description' => Intel_Df::t('Your target for the value produced by a page.'),
    'value' => 2.00,
    'group' => 'page',
  );
  $target['value_per_page_per_day_warning'] = array(
    'title' => Intel_Df::t('Value per page / day (warning)'),
    'description' => Intel_Df::t('Your target for the value produced by a page.'),
    'value' => 0.20,
    'group' => 'page',
  );

  /*
  $target['value_per_page_per_entrance'] = array(
    'title' => Intel_Df::t('Value per page / entry'),
    'description' => Intel_Df::t('Your target for the value per entrance produced by a page.'),
    'value' => 2.00,
    'group' => 'page',
  );
  $target['value_per_page_per_entrance_warning'] = array(
    'title' => Intel_Df::t('Value per page / entry (warning)'),
    'description' => Intel_Df::t('Your target for the value per entrance produced by a page.'),
    'value' => 0.20,
    'group' => 'page',
  );
  */

  $target['value_per_trafficsource_per_day'] = array(
    'title' => Intel_Df::t('Value per source / day'),
    'description' => Intel_Df::t('Your target for the value produced by a traffic source.'),
    'value' => 2.00,
    'group' => 'trafficsource',
  );
  $target['value_per_trafficsource_per_day_warning'] = array(
    'title' => Intel_Df::t('Value per source / day (warning)'),
    'description' => Intel_Df::t('Your target for the value produced by a traffic source.'),
    'value' => 0.20,
    'group' => 'trafficsource',
  );

  /*
  $target['value_per_trafficsource_per_entrance'] = array(
    'title' => Intel_Df::t('Value per source / entry'),
    'description' => Intel_Df::t('Your target for the total value of all traffic to the site.'),
    'value' => 2.00,
    'group' => 'trafficsource',
  );
  $target['value_per_trafficsource_per_entrance_warning'] = array(
    'title' => Intel_Df::t('Value per source / entry (warning)'),
    'description' => Intel_Df::t('Your target for the total value of all traffic to the site.'),
    'value' => 0.20,
    'group' => 'trafficsource',
  );
  */




  return $target;
}

function intel_get_targets() {
  $stargets = &Intel_Df::drupal_static(__FUNCTION__);
  if (isset($targets)) {
    return $targets;
  }
  $targets = array();
  $defaults = intel_init_targets();
  foreach ($defaults AS $k => $v) {
    $targets[$k] = $v['value'];
  }
  $ds = get_option('intel_targets', array());
  foreach ($ds AS $k => $v) {
    if (trim($v)) {
      $targets[$k] = $v;
    }
  }

  if (!isset($targets['value_per_entrance'])) {
    $targets['value_per_entrance'] = $targets['value_per_day'] / $targets['entrances_per_day'];
  }
  if (!isset($targets['value_per_entrance_warning'])) {
    $targets['value_per_entrance_warning'] = $targets['value_per_day_warning'] / $targets['entrances_per_day'];
  }
  // page objectives
  if (!isset($targets['value_per_page_per_entrance'])) {
    $targets['value_per_page_per_entrance'] = $targets['value_per_visit'];
  }
  if (!isset($targets['value_per_page_per_entrance_warning'])) {
    $targets['value_per_page_per_entrance_warning'] = $targets['value_per_visit_warning'];
  }
  if (!isset($targets['entrances_per_page_per_day'])) {
    $targets['entrances_per_page_per_day'] = $targets['value_per_page_per_entrance'] / $targets['value_per_page_per_day'];
  }
  if (!isset($targets['entrances_per_page_per_day_warning'])) {
    $targets['entrances_per_page_per_day_warning'] = $targets['value_per_page_per_entrance_warning'] / $targets['value_per_page_per_day'];
  }
  // trafficsource objectives
  if (!isset($targets['value_per_trafficsource_per_entrance'])) {
    $targets['value_per_trafficsource_per_entrance'] = $targets['value_per_visit'];
  }
  if (!isset($targets['value_per_trafficsource_per_entrance_warning'])) {
    $targets['value_per_trafficsource_per_entrance_warning'] = $targets['value_per_visit_warning'];
  }
  if (!isset($targets['entrances_per_trafficsource_per_day'])) {
    $targets['entrances_per_trafficsource_per_day'] = $targets['value_per_trafficsource_per_entrance'] / $targets['value_per_trafficsource_per_day'];
  }
  if (!isset($targets['entrances_per_trafficsource_per_day_warning'])) {
    $targets['entrances_per_trafficsource_per_day_warning'] = $targets['value_per_trafficsource_per_entrance_warning'] / $targets['value_per_trafficsource_per_day'];
  }

  /*
  if (empty($targets['entrances_per_page_per_day'])) {
    $targets['entrances_per_page_per_day'] = $targets['value_per_page_per_day'] / $targets['value_per_page_per_entrance'];
  }
  if (empty($targets['entrances_per_page_per_day_warning'])) {
    $targets['entrances_per_page_per_day_warning'] = $targets['value_per_page_per_day_warning'] / $targets['value_per_page_per_entrance_warning'];
  }
  */
  return $targets;
}

function intel_get_base_scorings() {
  $scoring = array();
  $scoring['entrance'] = array(
    'title' => Intel_Df::t('Entrance'),
    'description' => Intel_Df::t('A visit, i.e. new session, to the site.'),
    'value' => 0.10,
  );
  $scoring['stick'] = array(
    'title' => Intel_Df::t('Session stick'),
    'description' => Intel_Df::t('A visitor goes to at least one other page or triggered a interaction event. I.e. Sessions - Bounces'),
    'value' => 0.25,
  );
  $scoring['additional_pages'] = array(
    'title' => Intel_Df::t('Additional pages'),
    'description' => Intel_Df::t('Each additional page beyond the initial entrance page.'),
    'value' => 0.05,
  );
  return $scoring;
}

function intel_get_intel_goals_default($info = array()) {

  $title = Intel_Df::t('General');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('General conversion. Use as a default goal.'),
    'value' => 100,
  );

  $title = Intel_Df::t('Contact');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('General contact. Use as a default goal that creates a contact.'),
    'value' => 100,
  );

  $title = Intel_Df::t('Sales inquiry');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Request to initiate sales communication'),
    'value' => 200,
  );

  $title = Intel_Df::t('Research request');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Request to receive top of the funnel education premium offer.'),
    'value' => 100,
  );

  $title = Intel_Df::t('Research request');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Request to receive educational (ToFu) premium offer.'),
    'value' => 50,
  );

  $title = Intel_Df::t('Consideration request');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Request to receive brand building (MoFu) premium offer.'),
    'value' => 100,
  );

  $title = Intel_Df::t('Decision request');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Request to receive sales (BoFu) premium offer.'),
    'value' => 200,
  );

  $title = Intel_Df::t('Subscribe');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Request to subscribe to newsletter style updates.'),
    'value' => 100,
  );

  $title = Intel_Df::t('Job application');
  $name = strtolower(Intel_Df::drupal_clean_machinename($title));
  $info[$name] = array(
    'title' => $title,
    'name' => $name,
    'description' => Intel_Df::t('Job posting submission.'),
    'value' => 100,
  );

  return $info;
}

function intel_get_submission_goals_default() {
  $defs = array();
  $defs['tofu-conversion'] = array(
    'title' => Intel_Df::t('ToFu conversion'),
    'description' => Intel_Df::t('Top of the funnel conversion'),
    'value' => 15,
    'ga_id' => 1,
  );
  $defs['mofu-conversion'] = array(
    'title' => Intel_Df::t('MoFu conversion'),
    'description' => Intel_Df::t('Middle of the funnel conversion'),
    'value' => 50,
    'ga_id' => 2,
  );
  $defs['bofu-conversion'] = array(
    'title' => Intel_Df::t('BoFu conversion'),
    'description' => Intel_Df::t('Bottom of the funnel conversion'),
    'value' => 100,
    'ga_id' => 3,
  );
  $defs['subscribe-form'] = array(
    'title' => Intel_Df::t('Subscribe form'),
    'description' => Intel_Df::t('Subscribe to email updates form submission'),
    'value' => 25,
    'ga_id' => 4,
  );
  $defs['contact-form'] = array(
    'title' => Intel_Df::t('Contact form'),
    'description' => Intel_Df::t('Main contact form submission'),
    'value' => 25,
    'ga_id' => 5,
  );

  return $defs;
}

function intel_get_phonecall_goals_default() {
  $defs = array();
  $defs['contact-call'] = array(
    'title' => Intel_Df::t('Contact call'),
    'description' => Intel_Df::t('Main contact number called'),
    'value' => 25,
    'ga_id' => 6,
  );

  return $defs;
}

function intel_event_value_format($value, $options = array()) {
  $options += intel_get_valued_value_options();
  if (isset($options['mode']) && $options['mode'] == '') {
    return number_format($value, 0);
  }
  return $options['prefix'] . number_format($value, $options['decimals']) . $options['suffix'];
}

function intel_get_valued_value_options() {
  $format = array(
    'prefix' => '',
    'suffix' => '',
    'decimals' => 2,
  );
  return $format;
}

function intel_get_scorings($filter = '') {
  $scorings = &Intel_Df::drupal_static(__FUNCTION__);
  if (isset($scorings[$filter])) {
    return $scorings[$filter];
  }
  if (!isset($scorings)) {
    $scorings = array();
  }
  if (!isset($scorings[$filter])) {
    $scorings[$filter] = array();
  }
  $custom = get_option('intel_scorings', array());
  $base = intel_get_base_scorings();
  foreach ($base AS $i => $m) {
    if (($filter == 'js_setting') && (empty($m['js_setting']))) {
      //continue;
    }
    $scorings[$filter][$i] = isset($custom[$i]) ? $custom[$i] : $m['value'];
  }

  $events = intel_get_intel_event_info();
  foreach ($events AS $i => $m) {
    if (($filter == 'js_setting') && (empty($m['js_setting']))) {
      continue;
    }
    if ($i == 'session_stick') {
      $scorings[$filter][$i] = $base['stick']['value'];
      continue;
    }
    if ($m['mode'] == 'valued') {
      $i = 'event_' . $i;
      $scorings[$filter][$i] = isset($custom[$i]) ? $custom[$i] : $m['value'];
    }
  }

  $goals = intel_goal_load();
  //$goals = array_merge($goals, get_option('intel_submission_goals', intel_get_submission_goals_default()));
  //$goals = array_merge($goals, get_option('intel_phonecall_goals', intel_get_phonecall_goals_default()));
  foreach ($goals AS $i => $m) {
    $i = 'goal_' . $i;
    $scorings[$filter][$i] = isset($custom[$i]) ? $custom[$i] : (isset($m['value']) ? $m['value'] : 0);
  }

  return $scorings[$filter];
}

function intel_get_scoring($event, $mode = 'machinename') {
  $scoring = intel_get_scorings();

  return $scoring;
}

/**
 * Scores data feed item in a entrance (session) context
 *
 * @param $data
 * @param int $divideby
 * @param array $score_components
 * @param string $mode
 * @return mixed
 */
function intel_score_visit_aggregation($data, $divideby = 1, &$score_components = array(), $mode = '') {
  $scoring = intel_get_scorings();
  $scores = array(
    '_all' => 0,
    'events' => 0,
    'goals' => 0,
    'traffic' => 0,
  );

  $entrance = array(
    'entrances' => 0,
  );
  if ($mode == 'social') {
    if (!empty($data['socialNetwork']['_all']['entrance'])) {
      $entrance = $data['socialNetwork']['_all']['entrance'];
    }
    elseif (!empty($data['socialNetwork']['entrance'])) { // for all rows
      $entrance = $data['socialNetwork']['entrance'];
    }
  }
  elseif ($mode == 'seo') {
    if (!empty($data['organicSearch']['_all']['entrance'])) {
      $entrance = $data['organicSearch']['_all']['entrance'];
    }
    elseif (!empty($data['organicSearch']['entrance'])) { // for all rows
      $entrance = $data['organicSearch']['entrance'];
    }
  }
  else {
    if (!empty($data['entrance'])) {
      $entrance = $data['entrance'];
    }
  }

  // entrance scoring
  if ($entrance['entrances']) {
    // the value of any goals achieved during sessions started by the item
    $scores['goals'] += $entrance['goalValueAll'];
    // the value of any valued events triggered during sessions started by the item
    if (isset($entrance['events']['_all'])) {
      $scores['events'] += ($entrance['events']['_all']['value']);
    }
    // scoring of traffic genearted by the item
    $scores['traffic'] += $entrance['entrances'] * $scoring['entrance'];
    if (!empty($entrance['sticks'])) {
      $scores['traffic'] += $entrance['sticks'] * $scoring['stick'];
    }
    if ($entrance['pageviews'] > ($entrance['entrances'] - $entrance['sticks'])) {
      $scores['traffic'] += ($entrance['pageviews'] - $entrance['entrances'] - $entrance['sticks']) * $scoring['additional_pages'];
    }
  }
  if (isset($data['pageview'])) {

  }

  foreach ($scores AS $i => $score) {
    if (substr($i, 0, 1) == '_') {
      continue;
    }
    $scores[$i] = $score / $divideby;
    $scores['_all'] += $scores[$i];
  }

  $score_components = $scores;
  return $scores['_all'];
}

/**
 * Scores data feed item in page or page-attr context
 * @param $data
 * @param int $divideby
 * @param array $score_components
 * @param string $mode
 * @return mixed
 */
function intel_score_page_aggregation($data, $divideby = 1, &$score_components = array(), $mode = '', $method = 'site', $options = array()) {
  $scoring = intel_get_scorings();
  $scores = array(
    '_all' => array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
    // stats for values generated directly by the item on non
    'onpage' => array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
    // stats for values generated during sessions where the item is the entrance
    'entrance' => array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
    // stats generated downstream from page hit
    'assist' =>  array(
      '_all' => 0,
      'events' => 0, // N/A
      'goals' => 0,
      'traffic' => 0, // N/A
    ),
  );
  // Note that entrance and assist stats include onpage data

  $ac = .1; // assist coef
  $page = $data['pageview'];
  $entrance = array(
    'entrances' => 0,
  );
  if ($mode == 'social') {
    if (!empty($data['socialNetwork']['_all']['entrance'])) {
      $entrance = $data['socialNetwork']['_all']['entrance'];
    }
  }
  elseif ($mode == 'seo') {
    if (!empty($data['organicSearch']['_all']['entrance'])) {
      $entrance = $data['organicSearch']['_all']['entrance'];
    }
  }
  else {
    if (!empty($data['entrance'])) {
      $entrance = $data['entrance'];
    }
  }
  ///////////////////////
  // entrance scoring

  if ($entrance['entrances']) {
    // the value of any goal achieved on any page in sessions started
    // on this page or segement but not directly by the page
    $scores['entrance']['goals'] += $entrance['goalValueAll'];

    // the value of any valued events generated on any page in sessions started
    // on this page or segement
    if (isset($entrance['events']['_all'])) {
      $scores['entrance']['events'] += $entrance['events']['_all']['value'];
    }
    // traffic scoring based on traffic generated by this page or segement
    $scores['entrance']['traffic'] += $entrance['entrances'] * $scoring['entrance'];
    //$scores['_all']['traffic'] += $entrance['entrances'] * $scoring['entrance'];
    if (!empty($entrance['sticks'])) {
      $scores['entrance']['traffic'] += $entrance['sticks'] * $scoring['stick'];
      // when using Session stick events, stick numbers should be removed from events
      $scores['entrance']['events'] -= $entrance['sticks'] * $scoring['stick'];
    }
    if ($entrance['pageviews']['traffic'] > 2) {
      $scores['entrance']['traffic'] += ($entrance['pageviews'] - $entrance['entrances'] - $entrance['sticks']) * $scoring['additional_pages'];
    }
  }

  ///////////////////////
  // page hit scoring

  // the value of any goals achieved on the page or a page in the segment
  if (!empty($page['goalValueAll'])) {
    $scores['onpage']['goals'] += $page['goalValueAll'];
  }

  // the value of any goal achieved on this page/segement or downstream
  if (!empty($page['pageValueAll'])) {
    $scores['assist']['goals'] += ($page['pageValueAll'] - $page['goalValueAll']);
  }

  // the value of events triggered on this page/segment
  if (isset($page['events']['_all'])) {
    $scores['onpage']['events'] += $page['events']['_all']['value'];
  }
  // traffic scoring based on hit on this page/segment
  $scores['onpage']['traffic'] += ($page['pageviews'] - $entrance['entrances']) * $scoring['additional_pages'];

  // tally up scores from components
  $method = !empty($options['method']) ? !empty($options['method']) :'';
  if ($method == 'direct') {

  }
  else {
    $scores['_all']['goals'] = (1 - $ac) * $scores['entrance']['goals'] + $ac * ($scores['assist']['goals']);
    $scores['_all']['events'] = (1 - $ac) * $scores['onpage']['events'] + $ac * ($scores['entrance']['events']);
    $scores['_all']['traffic'] = $scores['entrance']['traffic'] + $scores['onpage']['traffic'];
  }


  // do divide bys and total up alls
  foreach ($scores AS $i => $a) {
    foreach ($a AS $j => $b) {
      if (substr($j, 0, 1) == '_') {
        continue;
      }
      $scores[$i][$j] = $scores[$i][$j] / $divideby;
      $scores[$i]['_all'] += $scores[$i][$j];
    }
  }

  $score_components = $scores;
  return $scores['_all']['_all'];
}

/**
 * Scores data feed item in page or page-attr context
 * @param $data
 * @param int $divideby
 * @param array $score_components
 * @param string $mode
 * @return mixed
 */
function intel_score_item($data, $divideby = 1, &$score_components = array(), $mode = '', $method = '', $options = array()) {
  $scoring = intel_get_scorings();
  $scores = array(
    '_all' => array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
    // stats for values generated directly by the item on non
    'onpage' => array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
    // stats for values generated during sessions where the item is the entrance
    'entrance' => array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
    // stats generated downstream from page hit
    'assist' =>  array(
      '_all' => 0,
      'events' => 0,
      'goals' => 0,
      'traffic' => 0,
    ),
  );
  // Note that entrance and assist stats include onpage data

  $ac = .1; // assist coef
  $page = array(
    'pageviews' => 0,
    'goalsValueAll' => 0,
    'pageValueAll' => 0,
  );
  if (isset($data['pageview'])) {
    $page = $data['pageview'];
  }

  $entrance = array();

  if ($mode == 'social') {
    if (!empty($data['socialNetwork']['_all']['entrance'])) {
      $entrance = $data['socialNetwork']['_all']['entrance'];
    }
  }
  elseif ($mode == 'seo') {
    if (!empty($data['organicSearch']['_all']['entrance'])) {
      $entrance = $data['organicSearch']['_all']['entrance'];
    }
  }
  else {
    if (!empty($data['entrance'])) {
      $entrance = $data['entrance'];
    }
  }
  $entrance += array(
    'entrances' => 0,
    'pageviews' => 0,
    'goalsValueAll' => 0,
  );
  ///////////////////////
  // entrance scoring

  if ($entrance['entrances']) {
    // the value of any goal achieved on any page in sessions started
    // on this page or segement but not directly by the page
    $scores['entrance']['goals'] += $entrance['goalValueAll'];

    // the value of any valued events generated on any page in sessions started
    // on this page or segement
    if (isset($entrance['events']['_totals'])) {
      $scores['entrance']['events'] += $entrance['events']['_totals']['value'];
    }
    elseif (isset($entrance['events']['_all'])) {
      $scores['entrance']['events'] += $entrance['events']['_all']['value'];
    }
    // traffic scoring based on traffic generated by this page or segement
    $scores['entrance']['traffic'] += $entrance['entrances'] * $scoring['entrance'];
    //$scores['_all']['traffic'] += $entrance['entrances'] * $scoring['entrance'];
    if (!empty($entrance['sticks'])) {
      $scores['entrance']['traffic'] += $entrance['sticks'] * $scoring['stick'];
      // when using Session stick events, stick numbers should be removed from events
      //$scores['entrance']['events'] -= $entrance['sticks'] * $scoring['stick'];
    }
    if ($entrance['pageviews'] > 2) {
      $scores['entrance']['traffic'] += ($entrance['pageviews'] - $entrance['entrances'] - $entrance['sticks']) * $scoring['additional_pages'];
    }
  }

  ///////////////////////
  // page hit scoring

  // the value of any goals achieved on the page or a page in the segment
  if (!empty($page['goalValueAll'])) {
    $scores['onpage']['goals'] += $page['goalValueAll'];
  }

  // the value of any goal achieved on this page/segement or downstream
  if (!empty($page['pageValueAll'])) {
    $scores['assist']['goals'] += ($page['pageValueAll'] - $page['goalValueAll']);
  }

  // the value of events triggered on this page/segment
  if (isset($page['events']['_totals'])) {
    $scores['onpage']['events'] += $page['events']['_totals']['value'];
  }
  elseif (isset($page['events']['_all'])) {
    $scores['onpage']['events'] += $page['events']['_all']['value'];
  }

  // traffic scoring based on hit on this page/segment
  $scores['onpage']['traffic'] += ($page['pageviews'] - $entrance['entrances']) * $scoring['additional_pages'];
//dsm($scores);
  // tally up scores from components
  if ($method == 'direct') {

  }
  elseif ($method == 'page') {
    // for assist, subtract entrance goals as to not double count them.
    $scores['_all']['goals'] = (1 - $ac) * $scores['entrance']['goals'] + $ac * ($scores['assist']['goals'] - $scores['entrance']['goals']);
    $scores['_all']['events'] = (1 - $ac) * $scores['onpage']['events'] + $ac * ($scores['entrance']['events']);
    $scores['_all']['traffic'] = $scores['entrance']['traffic'] + $scores['onpage']['traffic'];
  }
  elseif ($method == 'visitor') {
    // use for visitors
    $scores['_all']['goals'] = $scores['onpage']['goals'];
    $scores['_all']['events'] = $scores['onpage']['events'];
    // if visitor is filtered, entrance may not be included in filtered item
    // if entrance score is 0, use pageview for traffic
    $scores['_all']['traffic'] = !empty($scores['entrance']['traffic']) ? $scores['entrance']['traffic'] : $scores['onpage']['traffic'];
  }
  elseif ($method == 'site') {
    // use for visitors
    $scores['_all']['goals'] = $scores['onpage']['goals'];
    $scores['_all']['events'] = $scores['onpage']['events'];
    // if visitor is filtered, entrance may not be included in filtered item
    // if entrance score is 0, use pageview for traffic
    //$scores['_all']['traffic'] = $scores['entrance']['traffic'] + $scores['onpage']['traffic'];
    $scores['_all']['traffic'] = $scores['entrance']['traffic'];
  }
  else {
    // use entrance, trafficsources
    $scores['_all']['goals'] = $scores['entrance']['goals'];
    $scores['_all']['events'] = $scores['entrance']['events'];
    $scores['_all']['traffic'] = $scores['entrance']['traffic'];
  }


  // do divide bys and total up alls
  foreach ($scores AS $i => $a) {
    foreach ($a AS $j => $b) {
      if (substr($j, 0, 1) == '_') {
        continue;
      }
      $scores[$i][$j] = $scores[$i][$j] / $divideby;
      $scores[$i]['_all'] += $scores[$i][$j];
    }
  }

  $score_components = $scores;
  return $scores['_all']['_all'];
}

function _intel_get_report_dates_from_ops($ops = 'l30d', &$cache_options = array(), $return_hash = FALSE) {
  $start = '';
  $end = '';
  if (!empty($_GET['timeops'])) {
    $ops = $_GET['timeops'];
  }

  $a = explode(',', $ops);
  if (count($a) == 2) {
    $start = $a[0];
    $end = $a[1];
  }
  elseif ($ops == 'today') {
    $start = 'midnight';
    $end = 'now';
    $cache_options = array('round_start' => 1, 'round_end' => 1);
  }
  elseif (($ops == 'l24') || ($ops == 'l24fn') || ($ops == 'l24h') || ($ops == 'l24hfn')) {
    $start = '-24 hours';
    $end = 'now';
    $cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'yesterday') {
    $start = '-1 day midnight';
    $end = "midnight - 1 second";
  }
  elseif ($ops == 'p1d') {
    $start = "-2 days midnight";
    $end = "-1 days midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'l7dfn' || $ops == 'l1wfn') {
    $start = "-6 days midnight";
    $end = "now";
    $cache_options = array('round_end' => 1);
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'l7d' || $ops == 'l1w') {
    $start = "-7 days midnight";
    $end = "midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'p7d' || $ops == 'p1w') {
    $start = "-14 days midnight";
    $end = "-7 days midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'l28dfn' || $ops == 'l4wfn') {
    $start = "-27 days midnight";
    $end = "now";
    $cache_options = array('round_end' => 1);
  }
  elseif ($ops == 'l28d' || $ops == 'l4w') {
    $start = "-28 days midnight";
    $end = "midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'p28d' || $ops == 'p4w') {
    $start = "-56 days midnight";
    $end = "-28 days midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'l30dfn') {
    $start = "-29 days midnight";
    $end = "now";
    $cache_options = array('round_end' => 1);
  }
  elseif ($ops == 'l30d') {
    $start = "-30 days midnight";
    $end = "midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'p30d') {
    $start = "-60 days midnight";
    $end = "-30 days midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'l91dfn' || $ops == 'l13wfn') {
    $start = "-90 days midnight";
    $end = "now";
    $cache_options = array('round_end' => 1);
  }
  elseif ($ops == 'l91d' || $ops == 'l13w') {
    $start = "-91 days midnight";
    $end = "midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'p91d' || $ops == 'p13w') {
    $start = "-182 days midnight";
    $end = "-91 days midnight - 1 second";
    //$cache_options = array('refresh' => 1);
  }
  elseif ($ops == 'thismonth') {
    $start = "midnight first day of this month";
    $end = "midnight - 1 second";
    //$end = "midnight first day of next month - 1 second";
  }
  elseif ($ops == 'lastmonth') {
    $start = "midnight first day of last month";
    $end = "midnight first day of this month - 1 second";
  }
  elseif ($ops == 'monthtodate') {
    $start = "midnight first day of this month";
    $end = "now";
    $cache_options = array('round_end' => 1);
  }
  elseif ($ops == 'thisweek') {
    $start = "midnight this week";
    $end = "midnight - 1 second";
    //$end = "midnight first day of next month - 1 second";
  }
  elseif ($ops == 'lastweek') {
    $start = "midnight last week";
    $end = "midnight first day of this week - 1 second";
  }
  elseif ($ops == 'weektodate') {
    $start = "midnight this week";
    $end = "now";
    $cache_options = array('round_end' => 1);
  }
  else  {  // "l30d" last 30 days from yesterday
    $start = "-30 days midnight";
    $end = "midnight - 1 second";
  }

  return _intel_get_report_dates($start, $end);
}

function _intel_get_report_dates($start_default = "-31 days", $end_default = "-1 day", $return_hash = FALSE) {
  if (!empty($_GET['dates'])) {
    $a = explode(":", $_GET['dates']);
    $_GET['start_date'] = $a[0];
    $_GET['end_date'] = $a[1];
  }
  $start_date = (!empty($_GET['start_date'])) ? strtotime($_GET['start_date']) : strtotime($start_default);
  $end_date = (!empty($_GET['end_date'])) ? strtotime($_GET['end_date']) : strtotime($end_default);
  $number_of_days = round(($end_date - $start_date)/60/60/24);
  if (!$return_hash) {
    return array(
      $start_date,
      $end_date,
      $number_of_days,
    );
  }
  else {
    return array(
      'start_date' => $start_date,
      'end_date' => $end_date,
      'number_of_days' => $number_of_days,
    );
  }
}

function intel_ga_data_source() {
  static $source;
  if (empty($source)) {
    $source = get_option('intel_ga_data_source', '');
  }
  return $source;
}

function intel_ga_api_data($request = array(), $cache_options = array()) {
  $feed = array();
  $source = intel_ga_data_source();
  if ($source == 'gadwp') {
    $feed = intel_ga_api_data_gadwp($request, $cache_options);
    //$feed = intel_ga_api_data_internal($request, $cache_options);
  }
  //$feed = google_analytics_reports_api_report_data($request, $cache_options);
  return $feed;
}

function intel_ga_api_data_gadwp($request = array(), $cache_options = array()) {
  $gadwp = GADWP();
  $gapi = new GADWP_GAPI_Controller();
  $projectId = get_option('intel_ga_view');
  $from = date("Y-m-d", $request['start_date']);
  $to = date("Y-m-d", $request['end_date']);
  //$from = $request['start_date'];
  //$to = $request['end_date'];
  $metrics = implode(',', $request['metrics']);
  $options = array(
    'dimensions' => implode(',', $request['dimensions']),
  );
  $managequota = 'u' . get_current_user_id() . 's' . get_current_blog_id();
  $options = array(
    'dimensions' => implode(',', $request['dimensions']),
    'quotaUser' => $managequota . 'p' . $projectId
  );
  if (!empty($request['sort_metric']) ) {
    $options['sort'] = $request['sort_metric'];
  }
  if (!empty($request['filters']) ) {
    $options['filters'] = $request['filters'];
  }
  if (!empty($request['segment']) ) {
    $options['segment'] = $request['segment'];
  }
  if (!empty($request['max_results']) ) {
    $options['max-results'] = $request['max_results'];
  }
  /*
  $seed = $projectId . $from . $metrics;
  if (!empty($options['dimensions'])) {
    $seed .= $options['dimensions'];
  }
  if (!empty($options['filters'])) {
    $seed .= $options['filters'];
  }
  if (!empty($options['segment'])) {
    $seed .= $options['segment'];
  }
  */

  // set caching for endates = 'now';
  $request_cache = $request;
  $cache_time = 900;

  if (!empty($cache_options['round_start'])) {
    $request_cache['start_date'] = $cache_time * floor($request_cache['start_date'] / $cache_time);

  }
  if (!empty($cache_options['round_end'])) {
    $request_cache['end_date'] = $cache_time * floor($request_cache['end_date'] / $cache_time);
  }
  $serial = 'intel_' . md5(serialize(array_merge($request_cache, array())));

  //$serial = 'intel_' . $gapi->get_serial( $seed );
  $feed = intel_gadwp_handle_corereports( $projectId, $from, $to, $metrics, $options, $serial, $gapi, $cache_options );

  if (!is_object($feed)) {
    $ret = array(
      'results' => NULL,
      'error' => $feed,
    );
    return $ret;
  }
  $feed->results = (object)array(
    'rows' => array(),
    'totalsForAllResults' => array(),
  );
  foreach ($feed->totalsForAllResults as $k => $v) {
    $k = str_replace('ga:', '', $k);
    $feed->results->totalsForAllResults[$k] = $v;
  }
  if (!empty($feed->rows)) {
    $ch = $feed->getColumnHeaders();
    if (!is_array($ch)) {
      intel_d($feed);//
    }
    $colHeaders = array();
    foreach ($ch as $v) {
      $colHeaders[] = str_replace('ga:', '', $v->name);
    }
    foreach ($feed->rows as $i => $row0) {
      $row = array();
      foreach ($row0 as $ii => $v) {
        $row[$colHeaders[$ii]] = $v;
      }
      $feed->results->rows[$i] = $row;
    }
  }

  return $feed;
}

/**
 * Mimics private handle_corereports function in gadwp
 * @param $projectId
 * @param $from
 * @param $to
 * @param $metrics
 * @param $options
 * @param $serial
 * @param $gapi
 * @return bool|int|mixed
 */
function intel_gadwp_handle_corereports( $projectId, $from, $to, $metrics, $options, $serial, $gapi, $cache_options = array() ) {
  try {

    if ( $from == "today" ) {
      $timeouts = 0;
    } else {
      $timeouts = 1;
    }

    if (!empty($cache_options['refresh'])) {
      GADWP_Tools::delete_cache( $serial );
      $data = $gapi->service->data_ga->get( 'ga:' . $projectId, $from, $to, $metrics, $options );
      GADWP_Tools::set_cache( $serial, $data, $gapi->get_timeouts( $timeouts ) );
    }
    else {
      $transient = FALSE;
      $transient = GADWP_Tools::get_cache($serial);
      if ($transient === FALSE) {
        //if ($gapi->gapi_errors_handler()) {
        //  return -23;
        //}
        $data = $gapi->service->data_ga->get('ga:' . $projectId, $from, $to, $metrics, $options);
        GADWP_Tools::set_cache($serial, $data, $gapi->get_timeouts($timeouts));
        //$gapi->gadwp->config->options['api_backoff'] = 0;
        //$gapi->gadwp->config->set_plugin_options();
      }
      else {
        $data = $transient;
      }
    }
  } catch ( Google_Service_Exception $e ) {
    GADWP_Tools::set_cache( 'last_error', date( 'Y-m-d H:i:s' ) . ': ' . esc_html( "(" . $e->getCode() . ") " . $e->getMessage() ), $gapi->error_timeout );
    GADWP_Tools::set_cache( 'gapi_errors', array( $e->getCode(), (array) $e->getErrors() ), $gapi->error_timeout );
    return $e->getCode();
  } catch ( Exception $e ) {
    GADWP_Tools::set_cache( 'last_error', date( 'Y-m-d H:i:s' ) . ': ' . esc_html( $e ), $gapi->error_timeout );
    return $e->getCode();
  }

  if ( $data->getRows() > 0 ) {
    return $data;
  } else {
    return - 21;
  }
}


function intel_ga_feed_request_callback($request, $args) {
  $cache_options = $args['cache_options'];
  $request['sort_metric'] = $request['sort'];
  unset($request['sort']);
  $feed = intel_ga_api_data($request, $cache_options);
  return $feed;
}

function intel_report_add_js_callback($script, $type = 'file') {
  if ($type == 'inline') {
    drupal_add_js($script, array('type' => 'inline', 'scope' => 'header'));
  }
  else {
    $a = explode('//', $script);
    if ((count($a) == 2) && (substr($a[0], 0, 4) == 'http')) {
      drupal_add_js($script, array('type' => 'external', 'scope' => 'header'));
    }
    else {
      drupal_add_js(libraries_get_path('intel') . '/' . $script);
    }
  }
}

/**
 * Selects data rows based on if feed is ver 2 or ver 3
 * @param $feed
 * @return array
 */
function intel_get_ga_feed_rows($feed) {
  if (isset($feed->results->rows) && is_array($feed->results->rows)) {
    return $feed->results->rows;
  }
  elseif (isset($feed->results) && is_array($feed->results)) {
    return $feed->results;
  }
  else {
    return array();
  }
  $rows = (is_array($feed->results)) ? $feed->results : $feed->results->rows;
  return $rows;
}

/**
 * Selects data rows based on if feed is ver 2 or ver 3
 * @param $feed
 * @return array
 */
function intel_get_ga_feed_totals($feed) {
  if (isset($feed->results->totals) && is_array($feed->results->totals)) {
    return $feed->results->totals;
  }
  elseif (isset($feed->results->totalsForAllResults) && is_array($feed->results->totalsForAllResults)) {
    return $feed->results->totalsForAllResults;
  }
  elseif (isset($feed->totals) && is_array($feed->totals)) {
    return $feed->totals;
  }
  elseif (isset($feed->totalsForAllResults) && is_array($feed->totalsForAllResults)) {
    return $feed->totalsForAllResults;
  }
  else {
    return array();
  }
}

function intel_get_session_id($vtk, $session_count, $date_hour_minute) {
  return $vtk . '-' . $session_count;
}

/**
 * TODO move to library
 * @param $vtk
 */
function intel_fetch_analytics_visitor_meta_data($vtkids) {
  intel_include_library_file('ga/class.ga_model.php');

  $visitor = array(
    'location' => array(),
    'environment' => array(),
    'lasthit' => 0,
  );

  list($start_date, $end_date, $number_of_days) = _intel_get_report_dates("-1 year", "Today");
  $cache = array(
    'refresh' => 1,
  );

  //$segment = 'dynamic::ga:customVarValue5==' . implode(',dynamic::ga:customVarValue5==', $vtkids);
  //$segment = 'users::condition::ga:customVarValue5==' . implode(',ga:customVarValue5==', $vtkids); // users has a limit of 90 days time frame
  //$segment = 'sessions::condition::ga:customVarValue5==' . implode(',ga:customVarValue5==', $vtkids);
  $segment = 'sessions::condition::ga:dimension5==' . implode(',ga:dimension5==', $vtkids);

  $request = array(
    'dimensions' => array('ga:browser', 'ga:browserVersion', 'ga:operatingSystem', 'ga:operatingSystemVersion', 'ga:language', 'ga:screenResolution', 'ga:dimension4'),
    'metrics' => array('ga:entrances'),
    'sort_metric' => '-ga:dimension4',
    'start_date' => $start_date,
    'end_date' => $end_date,
    'segment' => $segment,
    'max_results' => 1,
  );

  $data = intel_ga_api_data($request, $cache);
  if (!empty($_GET['debug'])) {
    dpm('ga_request'); dpm($request);//
    dpm('ga_data'); dpm($data);//
  }
  $rows = intel_get_ga_feed_rows($data);
  if (!empty($rows) && is_array($rows)) {
    foreach ($rows AS $row) {
      $ts = (int)$row['dimension4'];
      if ($ts > $visitor['lasthit']) {
        $visitor['lasthit'] = $ts;
      }
      $visitor['environment'] = array();
      $visitor['environment']['browser'] = $row['browser'];
      $visitor['environment']['browserVersion'] = $row['browserVersion'];
      $visitor['environment']['operatingSystem'] = $row['operatingSystem'];
      $visitor['environment']['operatingSystemVersion'] = $row['operatingSystemVersion'];
      $visitor['environment']['language'] = $row['language'];
      $visitor['environment']['screenResolution'] = $row['screenResolution'];
    }
  }

  $request['dimensions'] = array('ga:isMobile', 'ga:mobileDeviceBranding', 'ga:mobileDeviceModel', 'ga:mobileDeviceInfo', 'ga:dimension4');
  $request['metrics'] = array('ga:entrances');

  $data = intel_ga_api_data($request, $cache);

  $rows = intel_get_ga_feed_rows($data);
  if (!empty($rows) && is_array($rows)) {
    foreach ($rows AS $row) {
      $ts = (int)$row['dimension4'];
      $visitor['environment']['isMobile'] = $row['isMobile'];
      $visitor['environment']['mobileDeviceBranding'] = $row['mobileDeviceBranding'];
      $visitor['environment']['mobileDeviceModel'] = $row['mobileDeviceModel'];
      $visitor['environment']['mobileDeviceInfo'] = $row['mobileDeviceInfo'];
    }
  }

  $request['dimensions'] = array('ga:country', 'ga:region', 'ga:city', 'ga:metro', 'ga:latitude', 'ga:longitude', 'ga:dimension4');
  $request['metrics'] = array('ga:entrances');

  $data = intel_ga_api_data($request, $cache);

  $rows = intel_get_ga_feed_rows($data);
  if (!empty($rows) && is_array($rows)) {
    foreach ($rows AS $row) {
      $ts = (int)$row['dimension4'];
      if ($ts > $visitor['lasthit']) {
        $visitor['lasthit'] = $ts;
      }
      $visitor['location']['country'] = $row['country'];
      $visitor['location']['region'] = $row['region'];
      $visitor['location']['city'] = $row['city'];
      $visitor['location']['metro'] = $row['metro'];
      $visitor['location']['latitude'] = $row['latitude'];
      $visitor['location']['longitude'] = $row['longitude'];
    }
  }

  $visitor['visits'] = array(
    '_all' => array(
      'score' => 0,
      'entrance' => array(
        'entrances' => 0,
        'pageviews' => 0,
        'timeOnPage' => 0,
        'score' => 0,
      ),
    ),
    '_totals' => array(
      'entrance' => array(
        'entrances' => 0,
        'pageviews' => 0,
        'timeOnPage' => 0,
        'sticks' => 0,
        'goalValueAll' => 0,
        'events' => array(),
        'score' => 0,
      ),
    ),
  );

  // get totals
  $request['dimensions'] = array();
  $request['metrics'] = array('ga:entrances', 'ga:pageviews', 'ga:bounces', 'ga:timeOnPage', 'ga:goalValueAll');
  $request['sort_metric'] = '';
  $request['max_results'] = 1000;

  $data = intel_ga_api_data($request, $cache);
  //dsm($request); dsm($data);
  $rows = intel_get_ga_feed_rows($data);
  if (!empty($rows) && is_array($rows)) {
    foreach ($rows AS $row) {
      $visitor['visits']['_totals']['entrance']['entrances'] += intval($row['entrances']);
      $visitor['visits']['_totals']['entrance']['pageviews'] += intval($row['pageviews']);
      $visitor['visits']['_totals']['entrance']['sticks'] += (intval($row['entrances']) - intval($row['bounces']));
      $visitor['visits']['_totals']['entrance']['timeOnPage'] += intval($row['timeOnPage']);
      $visitor['visits']['_totals']['entrance']['goalValueAll'] += floatval($row['goalValueAll']);
    }
  }

  $request['metrics'] = array('ga:totalEvents', 'ga:uniqueEvents', 'ga:eventValue', 'ga:metric2');
  $request['filters'] = 'ga:eventCategory=~^*!$';
  $data = intel_ga_api_data($request, $cache);
  //dsm($request); dsm($data);
  $rows = intel_get_ga_feed_rows($data);
  if (!empty($rows) && is_array($rows)) {
    foreach ($rows AS $row) {
      $visitor['visits']['_totals']['entrance']['events'] = array(
        '_totals' => array(
          'value' => !empty($row['metric2']) ? floatval($row['metric2']) : floatval($row['eventValue']),
          'totalValuedEvents' => intval($row['totalEvents']),
          'uniqueValuedEvents' => intval($row['uniqueEvents']),
        ),
      );
    }
  }

  $request['dimensions'] = array('ga:dimension5', 'ga:sessionCount', 'ga:medium', 'ga:source', 'ga:referralPath', 'ga:keyword', 'ga:dimension4');
  $request['metrics'] = array('ga:entrances');
  $request['filters'] = 'ga:entrances>0';
  $request['sort_metric'] = '-ga:dimension4';  // note: cant sort by sessionCount as it is treated as a string not an int
  $request['max_results'] = 10;

  $data = intel_ga_api_data($request, $cache);
//dsm($request); dsm($data);
  $visitCountMin = 0;
  $visitCountMax = 0;
  $tsMin = REQUEST_TIME;
  $rows = intel_get_ga_feed_rows($data);
  if (!empty($rows) && is_array($rows)) {
    foreach ($rows AS $row) {
      $i = $row['dimension5'] . '-' . $row['dimension4'];
      $visitor['visits'][$i] = array(
        'trafficsource' => array(),
      );
      $visitor['visits'][$i]['sessionCount'] = $row['sessionCount'];
      $visitor['visits'][$i]['time'] = $vts =  (int)$row['dimension4'];
      $visitor['visits'][$i]['trafficsource']['medium'] = $row['medium'];
      $visitor['visits'][$i]['trafficsource']['source'] = $row['source'];
      $visitor['visits'][$i]['trafficsource']['referralPath'] = $row['referralPath'];
      $visitor['visits'][$i]['trafficsource']['keyword'] = $row['keyword'];

      //$visitor['visits'][$i]['trafficsource']['socialNetwork'] = $row['socialNetwork'];
      //$visitor['visits'][$ts]['trafficsource']['campaign'] = $row['campaign'];
      if ($vts < $tsMin) {
        $tsMin = $vts;
      }
    }
  }

  $request['dimensions'] = array('ga:landingPagePath','ga:dimension5', 'ga:sessionCount', 'ga:socialNetwork', 'ga:campaign', 'ga:dimension4');
  $request['metrics'] = array('ga:entrances', 'ga:pageviews', 'ga:bounces', 'ga:timeOnPage', 'ga:goalValueAll');
  $request['sort_metric'] = 'ga:dimension4';
  $request['filters'] = 'ga:pageviews>0;' . LevelTen\Intel\GAModel::formatGtRegexFilter('ga:dimension4', ($tsMin-1));
  $request['max_results'] = 1000;

  /*
  $request['dimensions'] = array('ga:dimension5', 'ga:sessionCount', 'ga:socialNetwork', 'ga:campaign');
  $request['metrics'] = array('ga:pageviews', 'ga:bounces', 'ga:timeOnPage', 'ga:goalValueAll');
  $request['sort_metric'] = '';
  $request['filters'] = LevelTen\Intel\GAModel::formatGtRegexFilter('ga:dimension4', ($tsMin-1));
  */

  $data = intel_ga_api_data($request, $cache);
  $rows = intel_get_ga_feed_rows($data);

  $sessions = array();

  if (!empty($rows) && is_array($rows)) {
    $i = '';
    foreach ($rows AS $row) {
      //$i = $row['dimension5'] . '-' . $row['sessionCount'];
      $entrances = intval($row['entrances']);
      if ($entrances) {
        $visitCountMax++;
        $i = $row['dimension5'] . '-' . $row['dimension4'];

        $sessions[] = array(
          'ts' => intval($row['dimension4']),
          'i' => $i,
        );


        $visitor['visits'][$i]['entrance'] = array(
          'entrances' => 0,
          'pageviews' => 0,
          'sticks' => 0,
          'timeOnPage' => 0,
          'goalValueAll' => 0,
        );
        $visitor['visits'][$i]['trafficsource']['socialNetwork'] = $row['socialNetwork'];
        $visitor['visits'][$i]['trafficsource']['campaign'] = $row['campaign'];
      }

      if ($i) {
        $visitor['visits'][$i]['entrance']['entrances'] += $entrances;
        $visitor['visits'][$i]['entrance']['pageviews'] += intval($row['pageviews']);
        $visitor['visits'][$i]['entrance']['sticks'] += ($entrances - intval($row['bounces']));
        $visitor['visits'][$i]['entrance']['timeOnPage'] += intval($row['timeOnPage']);
        $visitor['visits'][$i]['entrance']['goalValueAll'] += floatval($row['goalValueAll']);
      }

      /*
      $visitor['visits'][$i]['entrance'] = array();
      $visitor['visits'][$i]['entrance']['entrances'] = 1;
      $visitor['visits'][$i]['entrance']['pageviews'] = intval($row['pageviews']);
      $visitor['visits'][$i]['entrance']['sticks'] = (1 - intval($row['bounces']));
      $visitor['visits'][$i]['entrance']['timeOnPage'] = intval($row['timeOnPage']);
      $visitor['visits'][$i]['entrance']['goalValueAll'] = intval($row['goalValueAll']);
      $visitor['visits'][$i]['trafficsource']['socialNetwork'] = $row['socialNetwork'];
      $visitor['visits'][$i]['trafficsource']['campaign'] = $row['campaign'];
      */

      $visitor['visits']['_all']['entrance']['entrances'] += $entrances;
      $visitor['visits']['_all']['entrance']['pageviews'] += intval($row['pageviews']);
      $visitor['visits']['_all']['entrance']['timeOnPage'] += intval($row['timeOnPage']);
    }
  }
  $visitor['visits']['_all']['sessionCount'] = $visitCountMax;

  // get valued events
  $request['dimensions'] = array('ga:dimension5', 'ga:sessionCount', 'ga:dimension4');
  $request['metrics'] = array('ga:entrances', 'ga:sessions', 'ga:totalEvents', 'ga:uniqueEvents', 'ga:eventValue', 'ga:metric2');
  $request['sort_metric'] = '';
  $request['filters'] = 'ga:eventCategory=~^*!$' . ';' . LevelTen\Intel\GAModel::formatGtRegexFilter('ga:dimension4', ($tsMin-1));
  /*
  $request['dimensions'] = array('ga:dimension5', 'ga:sessionCount');
  $request['metrics'] = array('ga:totalEvents', 'ga:uniqueEvents', 'ga:eventValue', 'ga:metric2');
  $request['sort_metric'] = '';
  $request['filters'] = 'ga:eventCategory=~^*!$' . ';' . LevelTen\Intel\GAModel::formatGtRegexFilter('ga:dimension4', ($tsMin-1));
  */
  $data = intel_ga_api_data($request, $cache);
  //dsm($request); dsm($data);
  $rows = intel_get_ga_feed_rows($data);
  $session_i = 0;
  if (!empty($rows) && is_array($rows)) {
    $ses = 0;
    $ses1 = 0;
    foreach ($rows AS $row) {
      //$i = (int)$row['sessionCount'];
      $ts = intval($row['dimension4']);
      if (!$ses || ($ses1 && $ts >= $ses1['ts'])) {
        $ses = $sessions[$session_i];
        $ses1 = isset($sessions[$session_i + 1]) ? $sessions[$session_i + 1] : 0;
        $i = $ses['i'];
        $visitor['visits'][$i]['entrance']['events'] = array(
          '_all' => array(
            'value' => 0,
            'totalValuedEvents' => 0,
            'uniqueValuedEvents' => 0
          ),
        );
        $session_i++;
      }

      $visitor['visits'][$i]['entrance']['events']['_all']['value'] += isset($row['metric2']) ? floatval($row['metric2']) : floatval($row['eventValue']);
      $visitor['visits'][$i]['entrance']['events']['_all']['totalValuedEvents'] += intval($row['totalEvents']);
      $visitor['visits'][$i]['entrance']['events']['_all']['uniqueValuedEvents'] += intval($row['uniqueEvents']);
      /*
      $visitor['visits'][$i]['entrance']['events'] = array(
        '_all' => array(
          'value' => !empty($row['metric2']) ? floatval($row['metric2']) : floatval($row['eventValue']),
          'totalValuedEvents' => intval($row['totalEvents']),
          'uniqueValuedEvents' => intval($row['uniqueEvents']),
        ),
      );
      */
    }
  }

  $score_components = '';
  $score = 0;
  foreach ($visitor['visits'] AS $index => $visit) {
    if (substr($index, 0, 1) == '_') {
      continue;
    }
    //$visitor['visits'][$index]['score'] = intel_score_visit_aggregation($visitor['visits'][$index], 1, $score_components);
    $visitor['visits'][$index]['score'] = intel_score_item($visitor['visits'][$index], 1, $score_components, '', 'entrance');
    $visitor['visits']['_all']['score'] += $visitor['visits'][$index]['score'];
  }
  $visitor['visits']['_totals']['score'] = intel_score_item($visitor['visits']['_totals'], 1, $score_components, '', 'entrance');

  if ($visitor['lasthit']) {
    return $visitor;
  }
  else {
    return FALSE;
  }
}

function intel_fetch_ga_profiles() {
  $ga_profiles = &Intel_Df::drupal_static(__FUNCTION__, array());

  if (!empty($ga_profiles)) {
    return $ga_profiles;
  }

  $gadwp = GADWP();
  if ( null === $gadwp->gapi_controller ) {
    $gadwp->gapi_controller = new GADWP_GAPI_Controller();
  }

  $startindex = 1;
  $totalresults = 65535; // use something big

  while ( $startindex < $totalresults ) {

    $result = $gadwp->gapi_controller->service->management_profiles->listManagementProfiles( '~all', '~all', array( 'start-index' => $startindex ) );

    $items = $result->getItems();

    $totalresults = $result->getTotalResults();

    if ( $totalresults > 0 ) {
      foreach ( $items as $profile ) {
        $timetz = new DateTimeZone( $profile->getTimezone() );
        $localtime = new DateTime( 'now', $timetz );
        $timeshift = strtotime( $localtime->format( 'Y-m-d H:i:s' ) ) - time();
        $id = $profile->getId();
        $item = array(
          'accountId' => $profile->getAccountId(),
          'internalPropertyId' => $profile->getInternalWebPropertyId(),
          'viewId' => $profile->getId(),
          'propertyId' => $profile->getwebPropertyId(),
          'viewName' => $profile->getName(),
          'url' => $profile->getwebsiteUrl(),
          'timezone' => $profile->getTimezone(),
          'timeshift' => $timeshift,
          'defaultPage' => $profile->getDefaultPage(),
        );

        $ga_profiles[$id] = $item;
        //$ga_dash_profile_list[] = array( $profile->getName(), $profile->getId(), $profile->getwebPropertyId(), $profile->getwebsiteUrl(), $timeshift, $profile->getTimezone(), $profile->getDefaultPage() );
        $startindex++;
      }
    }
  }

  return $ga_profiles;
}

function intel_fetch_ga_goals($save = 0) {
  $ga_goals = &Intel_Df::drupal_static(__FUNCTION__, array());

  if (!empty($ga_goals)) {
    return $ga_goals;
  }

  $ga_profile = get_option('intel_ga_profile', array());

  $gadwp = GADWP();
  if ( null === $gadwp->gapi_controller ) {
    $gadwp->gapi_controller = new GADWP_GAPI_Controller();
  }


  $startindex = 1;
  $maxresults = 20;
  $totalresults = 65535; // use something big

  while ( $startindex < $totalresults ) {
    $result = $gadwp->gapi_controller->service->management_goals->listManagementGoals($ga_profile['accountId'], $ga_profile['propertyId'], $ga_profile['viewId'], array('start-index' => $startindex, 'max-results' => $maxresults));

    $items = $result->getItems();

    $totalresults = $result->getTotalResults();

    if ($totalresults > 0) {
      foreach ($items as $goal) {
        $id = $goal->getId();
        $type = strtolower($goal->getType());
        $type_label = $type;
        $labels = array(
          'url_destination' => Intel_Df::t('Destination'),
          'event' => Intel_Df::t('Event'),
          'visit_time_on_site' => Intel_Df::t('Duration'),
          'visit_num_pages' => Intel_Df::t('Pages/session'),
        );
        $item = array(
          'id' => $id,
          'name' => $goal->getName(),
          'type' => $type,
          'typeLabel' => $labels[$type],
          'active' => $goal->getActive(),
          'value' => $goal->getValue(),
          'details' => array(),
        );
        if ($type == 'event') {
          $details = $goal->getEventDetails();
          $item['details'] = array(
            'useEventValue' => $details->getUseEventValue(),
            'conditions' => array(),
          );
          $conditions = $details->getEventConditions();
          foreach ($conditions as $condition) {
            $ctype = array(
              'type' => strtolower($condition->getType()),
            );
            if ($ctype['type'] == 'value') {
              $ctype['comparisonType'] = strtolower($condition->getComparisonType());
              $ctype['value'] = $condition->getComparisonValue();
            }
            else {
              $ctype['matchType'] = strtolower($condition->getMatchType());
              $ctype['expression'] = $condition->getExpression();
            }
            $item['details']['conditions'][$ctype['type']] = $ctype;
          }
        }
        elseif ($type == 'url_destination') {
          $details = $goal->getUrlDestinationDetails();
          $item['details'] = array(
            'url' => $details->getUrl(),
            'matchType' => strtolower($details->getMatchType()),
            'caseSensitive' => $details->getCaseSensitive(),
            'firstStepRequired' => $details->getFirstStepRequired(),
          );
        }
        elseif ($type == 'visit_time_on_site') {
          $details = $goal->getUrlDestinationDetails();
          d($details);
          $item['details'] = array(
            'url' => $details->getUrl(),
            'matchType' => strtolower($details->getMatchType()),
            'caseSensitive' => $details->getCaseSensitive(),
            'firstStepRequired' => $details->getFirstStepRequired(),
          );
        }
        $ga_goals[$id] = $item;
        $startindex++;
      }
    }
  }

  if ($save) {
    update_option('intel_ga_goals', $ga_goals);
    $ga_goals = intel_fetch_ga_goals();
    $op_meta['ga_goals_updated'] = time();
    update_option('intel_option_meta', $op_meta);
  }

  return $ga_goals;
}

function intel_goal_type_titles() {
  return array(
    'INTL' => Intel_Df::t('Intelligence'),
    'INTEL' => Intel_Df::t('Intelligence'),
    'EVENT' => Intel_Df::t('Event'),
    'VISIT_TIME_ON_SITE' => Intel_Df::t('Duration'),
    'VISIT_NUM_PAGES' => Intel_Df::t('Pages/session'),
    'URL_DESTINATION' => Intel_Df::t('Destination'),
  );
}

function intel_fetch_ga_goals_page() {

  $ga_goals = intel_fetch_ga_goals();
  $op_meta = get_option('intel_option_meta', array());

  $op_meta['ga_goals_updated'] = time();
  update_option('intel_ga_goals', $ga_goals);
  update_option('intel_option_meta', $op_meta);
}

function intel_get_ga_admin_goals_url() {
  $ga_profile = get_option('intel_ga_profile');
  return "https://analytics.google.com/analytics/web/#management/Settings/a{$ga_profile['accountId']}w{$ga_profile['internalPropertyId']}p{$ga_profile['viewId']}/%3Fm.page%3DGoals";
  //$url = "https://analytics.google.com/analytics/web/#management/Settings/a5541069w22533750p78133449/%3Fm.page%3DGoals";
}


function x_intel_fetch_ga_custom_dimensions($save = 0) {
  $ga_dimensions = &Intel_Df::drupal_static(__FUNCTION__, array());

  if (!empty($ga_dimensions)) {
    return $ga_dimensions;
  }

  $ga_profile = get_option('intel_ga_profile', array());

  $gadwp = GADWP();
  if ( null === $gadwp->gapi_controller ) {
    $gadwp->gapi_controller = new GADWP_GAPI_Controller();
  }

  intel_gapi_attach_management_custom_dimension($gadwp);

  $result = $gadwp->gapi_controller->service->management_customDimensions->listManagementCustomDimensions($ga_profile['accountId'], $ga_profile['propertyId']);

  $items = $result->getItems();

  $totalresults = $result->getTotalResults();

  $ga_dimensions = array();
  foreach ($items as $dim) {
    $item = array();
    $id = $dim->getId();
    $i = substr($id, 12);
    $ga_dimensions[$i] = array(
      //'id' => $id,
      'name' => $dim->getName(),
      'scope' => strtolower($dim->getScope()),
      'active' => $dim->getActive(),
    );
  }


  if ($save) {
    update_option('intel_ga_dimensions', $ga_dimensions);
    $ga_goals = intel_fetch_ga_goals();
    $op_meta['ga_dimensions_updated'] = time();
    update_option('intel_option_meta', $op_meta);
  }

  return $ga_dimensions;
}

function x_intel_gapi_attach_management_custom_dimension(&$gadwp) {
  /*
   * GADWP's GA library is too old to support custom dimensions.
   * So this is a hack to integrate custom dimension support.
   */

  $gadwp->gapi_controller->service->management_customDimensions = new Google_Service_Analytics_Resource_ManagementCustomDimensions(
    $gadwp->gapi_controller->service,
    $gadwp->gapi_controller->service->serviceName,
    'customDimensions',
    array(
      'methods' => array(
        'get' => array(
          'path' => 'management/accounts/{accountId}/webproperties/{webPropertyId}/customDimensions/{customDimensionId}',
          'httpMethod' => 'GET',
          'parameters' => array(
            'accountId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'webPropertyId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'customDimensionId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
          ),
        ),'insert' => array(
          'path' => 'management/accounts/{accountId}/webproperties/{webPropertyId}/customDimensions',
          'httpMethod' => 'POST',
          'parameters' => array(
            'accountId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'webPropertyId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
          ),
        ),'list' => array(
          'path' => 'management/accounts/{accountId}/webproperties/{webPropertyId}/customDimensions',
          'httpMethod' => 'GET',
          'parameters' => array(
            'accountId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'webPropertyId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'max-results' => array(
              'location' => 'query',
              'type' => 'integer',
            ),
            'start-index' => array(
              'location' => 'query',
              'type' => 'integer',
            ),
          ),
        ),'patch' => array(
          'path' => 'management/accounts/{accountId}/webproperties/{webPropertyId}/customDimensions/{customDimensionId}',
          'httpMethod' => 'PATCH',
          'parameters' => array(
            'accountId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'webPropertyId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'customDimensionId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'ignoreCustomDataSourceLinks' => array(
              'location' => 'query',
              'type' => 'boolean',
            ),
          ),
        ),'update' => array(
          'path' => 'management/accounts/{accountId}/webproperties/{webPropertyId}/customDimensions/{customDimensionId}',
          'httpMethod' => 'PUT',
          'parameters' => array(
            'accountId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'webPropertyId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'customDimensionId' => array(
              'location' => 'path',
              'type' => 'string',
              'required' => true,
            ),
            'ignoreCustomDataSourceLinks' => array(
              'location' => 'query',
              'type' => 'boolean',
            ),
          ),
        ),
      )
    )
  );
}

function intel_get_ga_admin_custom_dimensions_url() {
  $ga_profile = get_option('intel_ga_profile');
  return "https://analytics.google.com/analytics/web/#management/Settings/" . intel_get_ga_profile_slug() . "/%3Fm.page%3DCustomDimensions";
  //$url = "https://analytics.google.com/analytics/web/#management/Settings/a5541069w22533750p78133449/%3Fm.page%3DGoals";
}

function intel_get_ga_profile_slug() {
  $ga_profile = get_option('intel_ga_profile');
  return "a{$ga_profile['accountId']}w{$ga_profile['internalPropertyId']}p{$ga_profile['viewId']}";
}

function intel_get_ga_admin_url($page = '', $options = array()) {
  $url = "https://analytics.google.com/analytics/web/#management/Settings/" . intel_get_ga_profile_slug() . '/';
  if ($page == 'custom_dimensions') {
    $url .= '%3Fm.page%3DCustomDimensions';
  }
  if ($page == 'goal_list') {
    $url .= '%3Fm.page%3DGoals%26m-content-goalList.rowShow%3D20';
  }
  return $url;
}

function intel_ga_custom_dimension_load($name = '', $options = array()) {

  $op_meta = get_option('intel_option_meta', array());

  // refresh if goal have never been updated
  if (empty($op_meta['ga_custom_dimensions_updated'])) {
    $options['refresh'] = 1;
  }

  // cache for one month unless requested
  if (!isset($options['refresh'])) {
    $options['refresh'] = 2592000;
  }

  // load goals from ga
  if (!empty($options['refresh'])) {
    $time = is_numeric($options['refresh']) ? $options['refresh'] : 0;
    $ga_dims_updated = !empty($op_meta['ga_custom_dimensions_updated']) ? $op_meta['ga_custom_dimensions_updated'] : 0;
    if ((time() - $ga_dims_updated) > $time) {

      include_once INTEL_DIR . 'includes/intel.imapi.inc';

      try {
        $ga_dims = intel_imapi_ga_custom_dimension_get();
        update_option('intel_ga_custom_dimensions', $ga_dims);
        $op_meta['ga_custom_dimensions_updated'] = time();
        update_option('intel_option_meta', $op_meta);
      }
      catch (Exception $e) {
        Intel_Df::drupal_set_message($e->getMessage(), 'error');
        $ga_dims = array();
      }
    }
  }

  if (!isset($ga_dims)) {
    $ga_dims = get_option('intel_ga_custom_dimensions', array());
  }

  if (!empty($name)) {
    return !empty($ga_dims[$name]) ? $ga_dims[$name] : 0;
  }
  return $ga_dims;
}

function intel_ga_custom_metric_load($name = '', $options = array()) {

  $op_meta = get_option('intel_option_meta', array());

  // refresh if goal have never been updated
  if (empty($op_meta['ga_custom_metrics_updated'])) {
    $options['refresh'] = 1;
  }

  // cache for one month unless requested
  if (!isset($options['refresh'])) {
    $options['refresh'] = 2592000;
  }

  // load goals from ga
  if (!empty($options['refresh'])) {
    $time = is_numeric($options['refresh']) ? $options['refresh'] : 0;
    $ga_metrics_updated = !empty($op_meta['ga_custom_metrics_updated']) ? $op_meta['ga_custom_metrics_updated'] : 0;
    if ((time() - $ga_metrics_updated) > $time) {

      include_once INTEL_DIR . 'includes/intel.imapi.inc';

      try {
        $ga_metrics = intel_imapi_ga_custom_metric_get();
        update_option('intel_ga_custom_metrics', $ga_metrics);
        $op_meta['ga_custom_metrics_updated'] = time();
        update_option('intel_option_meta', $op_meta);
      }
      catch (Exception $e) {
        Intel_Df::drupal_set_message($e->getMessage(), 'error');
        $ga_dims = array();
      }
    }
  }

  if (!isset($ga_metrics)) {
    $ga_metrics = get_option('intel_ga_custom_metrics', array());
  }

  if (!empty($name)) {
    return !empty($ga_metrics[$name]) ? $ga_metrics[$name] : 0;
  }
  return $ga_metrics;
}



