<?php
/**
 * @file
 * Administration of visitors
 * 
 * @author Tom McCracken <tomm@getlevelten.com>
 */


function intel_util() {
  return 'OK';
}

/**
 * Testing function
 */
function intel_util_temp() {

  global $wpdb;

  $sql = "
    SELECT post_id
    FROM {$wpdb->prefix}postmeta AS pm
    LEFT JOIN {$wpdb->prefix}posts AS p ON pm.post_id = p.ID
    WHERE p.ID IS NULL
    GROUP BY post_id
  ";
  $results = $wpdb->get_results( $sql );
  foreach ($results as $row) {
    $wpdb->delete("{$wpdb->prefix}postmeta", array('post_id' => $row->post_id), array('%d'));
  }
  intel_d($results);

  return 'OK 4';

  $intel_events = intel_get_intel_event_info();
  d($intel_events);

  $scorings = intel_get_scorings();
  d($scorings);

  $vars = intel_form_submission_vars_default();
  $submission = &$vars['submission'];

  $submission->type = 'testform';
  $submission->type = 'testform';
  $submission->fid = 'test-machine-name';  // $submission->fid
  $submission->fsid = rand(1, 1000); // $submission->fsid
  $submission->form_title = 'Test form';
  $submission->form_page_uri = ':post:1';
  $submission->form_page_id = 1;
  $submission->response_page_uri = ':post:1';
  $submission->response_page_id = 1;

  $vars['track_id'] = 'form_submission__contact';

  // primary

  d($vars);
  $_GET['debug'] = 1;
  intel_process_form_submission($vars);

  return 'OK';


/*
  $urls = array(
    'http://www.alpha.com',
    '//www.beta.com',
    'http://www.alpha.com/blog/alpha',
    '//www.beta.com/blog/beta',
    'http://www.alpha.com/blog/alpha?id=4',
    '//www.beta.com/blog/beta?id=4&view=full',
    ':post:1',
    'http://getlevelten.compost:1',
    'urn:post:1',
    'urn::post:1',
  );

  foreach ($urls as $url) {
    $parse = intel_parse_url($url);
    d($url);
    d($parse);
  }

  return 'OK';
*/

  $entity_info = intel()->entity_info('intel_submission');

  d($entity_info);

  return 'OK';




  return;
  require_once INTEL_DIR . 'includes/class-intel-gapi.php';

  $intel = intel();
  if ( null === $intel->gapi ) {
    $intel->gapi = new Intel_GAPI_Controller();
  }
  return '';

  $goal_categories = intel_get_goal_categories();
  d($goal_categories);

  $event_goal_info = intel_get_event_goal_info();
  d($event_goal_info);

  return $out;
  $action = !empty($_GET['action']) ? $_GET['action'] : 'load';


  if ($action == 'load') {
    $visitor = intel_visitor_load_or_create('user');
  }
  if ($action == 'create') {
    $visitor = intel_visitor_load_or_create('user');

    $visitor_properties = array(
      'data.givenName' => 'Abe',
      'data.familyName' => 'Tester',
      'data.name' => 'Abe Tester',
      'data.email' => !empty($_GET['email']) ? $_GET['email'] : 'abe@example.com',
      'data.twitter:username' => 'levelten_tom',
    );

    $property_options = array(
      'source' => 'form',
    );
    if(!empty($visitor_properties) && is_array($visitor_properties)) {
      foreach ($visitor_properties as $k => $fv) {
        $kt = $k;
        if (strpos($k, 'data.') === 0) {
          $kt = substr($k, 5);
        }
        $a = explode(':', $kt);
        $prop_name = $a[0];
        $var_name = !empty($a[1]) ? $a[1] : '@value';
        $value = $visitor->getProp($kt);
        $value[$var_name] = $fv;
        $visitor->setProp($prop_name, $value, $property_options);
      }
    }

    $visitor->save();
  }
  if ($action == 'delete') {
    $visitor = intel_visitor_load_or_create('user');
    if (!empty($visitor->vid)) {
      intel_visitor_delete($visitor->vid);
    }
  }


  d($visitor);
  return 'OK';

  include_once 'includes/intel.visitor_properties.inc';

  jsonld_init();

  $contexts = array();

  $contexts['Person'] = intel_get_itemprop_context('Person');

  print_r($contexts['Person']);
  //echo json_encode($contexts['Person'], JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
  echo "\n-------------------\n";

  /*
  $contexts['Person'] = (object)array(
    // Aliases
    //'id' => '@id',  // alias id to @id
    'type' => '@type',
    'url' => '@id',
    // Namespaces
    'io' => 'http://oepro3p.localhost/intel/itemprop/context/',
    'foaf' => 'http://xmlns.com/foaf/0.1/',
    'schema' => 'http://schema.org/', // add schema namespace
    // Properties
    'name' => 'http://schema.org/name',
    'description' => 'http://schema.org/description',
    'image' => 'http://schema.org/image',
    'familyName' => 'http://schema.org/familyName',
    'givenName' => 'http://schema.org/givenName',
    'nickname' => 'foaf:nick',
    'preferredName' => 'http://schema.org/name',
    'socialProfile' => 'http://schema.org/sameAs',
    // just testing a custom definition
    'birthday' => (object)array(
      '@id' => 'http://schema.org/birthDate',
      '@type' => 'xsd:date',
    ),
    //'facebook' => array(
    //  '@id' => 'http://schema.org/sameAs',
      //'@type' => 'URL',
    //),
  );

  print_r($contexts['Person']);
  //echo json_encode($contexts['Person'], JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
  echo "\n-------------------\n";
  */


  $visitor = (object)array(
    '@context' =>       $contexts['Person'],
    '@type' =>          'Person',
    'name' =>           'Tommy Tester',
    'description' =>    'He is a tester!',
    'image' =>          'http://getlevelten.com/sites/default/files/tom_1.png',
    'url' =>            'http://getlevelten.com/about-us/tom-mccracken',
    'familyName' =>     'Tester',
    'givenName' =>      'Tommy',
    'preferredName' =>  'Tom Dude',
    'foaf:nick' => 'Tom Dude Nick',
    'nickname' => 'Nick 2',
    'schema:gender' =>   'Male',
    'birthday' => '1969-07-02',
    'schema:knows' => (object)array(
      'name' => 'Boris',
    ),
    //'facebook' =>       (object)array(
    //  '@value' => 'http://facebook.com/tommy',
    //  '@type'  =>
    //),
    //'socialProfile' =>  '',
  );

  //$visitor->{'schema:knows'} = 'http://facebook.com/boris';
  //$visitor->{'schema:knows'} = array('http://facebook.com/boris', 'http://facebook.com/carrie');

  $visitor->image = (object)array(
    //'caption' => 'Image caption',
    '@value' => 'http://getlevelten.com/sites/default/files/tom_1.png',
    //'name' => 'Image name',
    'caption' => 'Image caption',
  );

  $js = '<script id="intel-ld" type="application/ld+json">' . json_encode($visitor, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES) . '</script>';

  $markup = array(
    '#type' => 'markup',
    '#markup' => $js,
  );

  drupal_add_html_head($markup, 'intel');

  //drupal_add_js($js, 'inline');

  echo json_encode($visitor, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
  echo "\n-------------------\n";

  $expanded = jsonld_expand($visitor);

  echo json_encode($expanded, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  echo "\n-------------------\n";

  $compacted = jsonld_compact($expanded, $contexts['Person']);

  echo json_encode($compacted, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  return '';

  $context = (object)array(
    "name" => "http://schema.org/name",
    "homepage" => (object)array("@id" => "http://schema.org/url", "@type" => "@id"),
    "image" => (object)array("@id" => "http://schema.org/image", "@type" => "@id")
  );

  $doc = (object)array(
    "http://schema.org/name" => "Manu Sporny",
    "http://schema.org/url" => (object)array("@id" => "http://manu.sporny.org/"),
    "http://schema.org/image" => (object)array("@id" => "http://manu.sporny.org/images/manu.png")
  );

// compact a document according to a particular context
// see: http://json-ld.org/spec/latest/json-ld/#compacted-document-form
  $compacted = jsonld_compact($doc, $context);

  echo json_encode($compacted, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  $doc = (object)array(
    "name" => "Manu Sporny",
    "url" => "http://manu.sporny.org/",
    "image" => "http://manu.sporny.org/images/manu.png",
  );


  $doc->{'@context'} = $context;

  echo json_encode($doc, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  $expanded = jsonld_expand($doc);

  echo json_encode($expanded, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  //$p = new JsonLdDrupalProcessor();
  //return $p->fromRDF($input);


  return '';
}

/**
 * Form Test function
 */
function intel_util_temp_form($form, &$form_state) {

  $defaults = get_option('intel_test_form', array());

  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Fieldset'),
    '#description' => Intel_Df::t('Fieldset description'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['fs']['textfield'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Textfield'),
    '#default_value' => !empty($defaults['test']) ? $defaults['test'] : '',
    '#description' => Intel_Df::t('Textfield description.'),
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
  intel_d($values);
}

function intel_util_init_options() {
  update_option('intel_apikey', "e84cac6f1c6640dbd143755675fb05b0");
  update_option('intel_api_level', 'pro');
  update_option('intel_custom_embed_script', '');
  update_option('intel_debug_ga_debug', 0);
  update_option('intel_debug_mode', 0);
  update_option('intel_domain_name', '');
  update_option('intel_extended_mode', 0);
  update_option('intel_ga_tid', "UA-5541069-16");
  update_option('intel_ga_view', "78133449");
  update_option('intel_ga_data_source', "gadwp");
  update_option('intel_icon_urls', array());
  update_option('intel_intel_scripts_enabled', array());
  update_option('intel_js_monitor_script', '');
  update_option('intel_l10iapi_connector', '');
  update_option('intel_l10iapi_custom_params', array());
  update_option('intel_l10iapi_js_embed_style', '');
  update_option('intel_gmap_apikey', 'AIzaSyDsafj-vVtKc-MzEPmSB1GCJb9Jpqgsg5M');
  return 'OK - options initialized';
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

function intel_util_reset_site() {
  if (empty($_GET['pw']) || $_GET['pw'] != 'please') {
    return __('What is the password?', 'intel');
  }
  if (empty($_GET['flags'])) {
    return __('No flags set', 'intel');
  }
  $flags = $_GET['flags'];
  if ($flags = '_') {
    $flags = 'c,v,f';
  }
  $flags = explode(',', $flags);
  global $wpdb;
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

    if (is_plugin_active( 'flamingo/flamingo.php' )) {
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

