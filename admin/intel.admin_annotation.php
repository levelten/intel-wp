<?php
/**
 * @file
 * Administration of submission data
 *
 * @author Tom McCracken <tomm@getlevelten.com>
 */

include_once INTEL_DIR . 'includes/class-intel-form.php';

function intel_admin_annotation_list_page() {

  global $wpdb;

  $lift = array(
    -53.7,
    13.50,
    26.6,
    0.1,
  );

  $data = array(
    100,
    0,
  );
  $sql = "
		  SELECT *
		  FROM {$wpdb->prefix}intel_annotation
      ORDER BY implemented DESC
      LIMIT %d OFFSET %d
		";

  $timezone_info = intel_get_timezone_info();

  intel_d(intel_annotation_display_time_offset());

  $results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

  $header = array(
    Intel_Df::t('Initiated'),
    Intel_Df::t('Type'),
    Intel_Df::t('Summary'),
    Intel_Df::t('Value ') . '&Delta;',
    Intel_Df::t('Ops'),
  );
  $rows = array();

  $options = array();
  $custom_default_value = '';
  $link_options = array(
    'query' => Intel_Df::drupal_get_destination(),
  );
  $link_options = array();
  $i = 0;
  foreach ($results as $row) {
    $ops = array();
    $ops[] = Intel_Df::l(Intel_Df::t('view'), 'annotation/' . $row->aid, $link_options);
    $ops[] = Intel_Df::l(Intel_Df::t('edit'), 'annotation/' . $row->aid . '/edit', $link_options);
    if (!empty($event['custom'])) {
      $ops[] = Intel_Df::l(Intel_Df::t('delete'), 'annotation/' . $row->aid . '/delete', $link_options);
    }
    else {
      //$ops[] = Intel_Df::t('NA');
    }
    $rows[] = array(
      date("Y-m-d H:i", $row->implemented + intel_annotation_display_time_offset()),
      $row->type,
      $row->message,
      ($lift[$i] > 0 ? '+' : '') . $lift[$i] . '%',
      //$row['type'],
      //$row['message'],
      implode(' ', $ops),
    );
    $i++;
  }

  $vars = array(
    'header' => $header,
    'rows' => $rows,
  );

  $output = Intel_Df::theme('table', $vars);

  return $output;
}

function intel_annotation_page($annotation) {

  if (intel_is_debug()) {
    intel_d($annotation);//
  }

  $output = '';

  $timezone_info = intel_get_timezone_info();

  //Intel_Df::drupal_set_title(Intel_Df::t('Annotation @title', array('@title' => $annotation->message)));
  //$form = Intel_Form::drupal_get_form('intel_admin_annotation_form', $annotation, 1);

  //$output = Intel_Df::render($form);

  $output .= '<div class="card">';
  $output .= '<div class="card-block">';
  $output .= '<div class="row">';
  $output .= '<div class="col-md-4">';

  $output .= '<div>';
  $output .= '<dt>Implemented</dt>';
  $output .= '<dd>';
  $output .= Intel_Df::t('GA') . ': ' . date("m/d/Y H:i", $annotation->implemented + $timezone_info['ga_offset']) . ' (' . $timezone_info['ga_timezone_abv'] . ")\n<br />";
  $output .= Intel_Df::t('WP') . ': ' . date("m/d/Y H:i", $annotation->implemented  + $timezone_info['cms_offset']) . ' (' . $timezone_info['cms_timezone_abv'] . ')</dd>';
  $output .= '</div>';

  $output .= '<div>';
  $output .= '<dt>Type</dt>';
  $output .= '<dd>' . $annotation->type . '</dd>';
  $output .= '</div>';

  $output .= '</div>';
  $output .= '<div class="col-md-8">';

  $output .= '<div>';
  $output .= '<dt>Description</dt>';
  $output .= '<dd>' . $annotation->message . '</dd>';
  $output .= '</div>';

  $output .= '</div>';
  $output .= '</div>';
  $output .= '</div>';
  $output .= '</div>';


  $header = array(
    '',
    array(
      'data' => 'Sessions',
      'class' => array('text-right'),
    ),
    array(
      'data' => 'Attraction value',
      'class' => array('text-right'),
    ),
    array(
      'data' => 'Engagement value',
      'class' => array('text-right'),
    ),
    array(
      'data' => 'Conversions',
      'class' => array('text-right'),
    ),
    array(
      'data' => 'Conversion value',
      'class' => array('text-right'),
    ),
    array(
      'data' => 'Total value',
      'class' => array('text-right'),
    ),
  );

  $data = array();
  $data[] = array(
    'sessions' => 5684,
    'vsessions' => 4523,
    'vevents' => 450,
    'goals' => 142,
  );
  $data[] = array(
    'sessions' => 5845,
    'vsessions' => 4543,
    'vevents' => 498,
    'goals' => 162,
  );

  $timedelta = 60 * 60 * 8;
  $secinweek = 60 * 60 * 24 * 7;

  $keys = array('sessions', 'avalue', 'evalue', 'goals', 'cvalue', 'value');

  $rows = array();
  for ($i = 0; $i < count($data); $i++) {
    $data[$i]['avalue'] = .105 * $data[$i]['vsessions'];
    $data[$i]['evalue'] = 3.21 * $data[$i]['vevents'];
    $data[$i]['cvalue'] = 117 * $data[$i]['goals'];
    $data[$i]['value']  = $data[$i]['avalue'] + $data[$i]['evalue'] + $data[$i]['cvalue'];
    $row = array();
    if ($i == 0) {
      $row[] = 'Before: ' . date("m/d/Y H:i", $annotation->timestamp) . ' - ' . date("m/d/Y H:i", $annotation->timestamp + $timedelta);
    }
    else {
      $row[] = 'After: ' . date("m/d/Y H:i", $annotation->timestamp - $secinweek) . ' - ' . date("m/d/Y H:i", $annotation->timestamp + $timedelta - $secinweek);
    }
    foreach ($keys as $k) {
      $row[] = array(
        'data' => number_format($data[$i][$k]),
        'class' => array('text-right'),
      );
    }
    $rows[] = $row;
  }

  $row = array(
    '<strong>Change</strong>',
  );
  $row2 = array(
    '% Change',
  );

  foreach ($keys as $k) {
    $v = $data[1][$k] - $data[0][$k];
    $v2 = 100 * ($v) / $data[0][$k];
    $row[] = array(
      'data' => '<strong>' . ($v > 0 ? '+' : '') . number_format($v, 0) . '</strong>',
      'class' => array('text-right'),
    );
    $row2[] = array(
      'data' => ($v > 0 ? '+' : '') . number_format($v2, 2) . '%',
      'class' => array('text-right'),
    );

  }
  $rows[] = $row;
  $rows[] = $row2;

  $vars = array(
    'header' => $header,
    'rows' => $rows,
  );

  $output .= '<div class="card">';
  $output .= '<h4 class="card-header">Analytics</h4>';
  $output .= Intel_Df::theme('table', $vars);
  $output .= '</div>';

  //return $form;
  return $output;
}

function intel_admin_annotation_add_page() {

  Intel_Df::drupal_set_title(Intel_Df::t('Add new annotation'));
  //drupal_set_title(t('Add visitor attribute'));
  $form = Intel_Form::drupal_get_form('intel_admin_annotation_form');
  //return $form;
  return Intel_Df::render($form);
}

function intel_admin_annotation_edit_page($annotation) {
  Intel_Df::drupal_set_title(Intel_Df::t('Edit @title annotation', array('@title' => date("Y-m-d H:i", $annotation->timestamp))));
  $form = Intel_Form::drupal_get_form('intel_admin_annotation_form', $annotation);
  //return $form;


  return Intel_Df::render($form);
}



function intel_admin_annotation_form($form, &$form_state, $annotation = NULL, $view = 0) {

  $add = 0;
  if (empty($annotation)) {
    $annotation = intel_annotation_construct();
    $add = 1;
  }

  $form_state['add'] = $add;
  $form_state['annotation'] = $annotation;

  $name = 'timestamp';
  $form[$name] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Launched'),
    '#default_value' => date("Y-m-d H:i", $annotation->timestamp),
    '#description' => Intel_Df::t('Date and time change was initiated.'),
  );
  if ($view) {
    $form[$name]['#type'] = 'item';
    $form[$name]['#markup'] = $form[$name]['#default_value'];
  }

  $name = 'type';
  $form[$name] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Type'),
    '#default_value' => $annotation->type ? $annotation->type : 'custom',
    '#description' => Intel_Df::t('Classification of annotation.'),
  );
  if ($view) {
    $form[$name]['#type'] = 'item';
    $form[$name]['#markup'] = $form[$name]['#default_value'];
  }

  $name = 'message';
  $form[$name] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('Message'),
    '#default_value' => $annotation->message,
    '#description' => Intel_Df::t('Discription of the change.'),
  );
  if ($view) {
    $form[$name]['#type'] = 'item';
    $form[$name]['#markup'] = $form[$name]['#default_value'];
  }

  if (!$view) {
    $form['save'] = array(
      '#type' => 'submit',
      '#value' => $add ? Intel_Df::t('Add annotation') : Intel_Df::t('Save annotation'),
    );
  }

  return $form;
}

function intel_admin_annotation_form_validate(&$form, &$form_state) {
  $values = &$form_state['values'];

  $ts = strtotime($values['timestamp']);

  if (!is_numeric($ts)) {
    $msg = Intel_Df::t('Timestamp is invalid. Please provide a timestamp in a valid format.');
    form_set_error('timestamp', $msg);
  }
  else {
    $values['timestamp'] = $ts;
  }
}

function intel_admin_annotation_form_submit(&$form, &$form_state) {
  $values = $form_state['values'];

  $annotation = $form_state['annotation'];

  foreach ($values as $k => $v) {
    if (isset($annotation->{$k})) {
      $annotation->{$k} = $v;
    }
  }

  $annotation->updated = REQUEST_TIME;

  intel_annotation_save($annotation);

  if (!empty($form_state['add'])) {
    $msg = Intel_Df::t('Intel annotation %title has been added.', array(
      '%title' => $annotation->timestamp,
    ));
  }
  else {
    $msg = Intel_Df::t('Intel annotation %title has been updated.', array(
      '%title' => $annotation->timestamp,
    ));
  }
  Intel_Df::drupal_set_message($msg);
  Intel_Df::drupal_goto('annotation/' . $annotation->aid);
}

function intel_admin_annotation_delete_page($event) {
  Intel_Df::drupal_set_title(Intel_DF::t('Are you sure you want to delete @title?', array('@title' => $event['title'])));
  $form = Intel_Form::drupal_get_form('intel_admin_annotation_delete_form', $event);
  //return $form;
  return Intel_Df::render($form);
}

function intel_admin_annotation_delete_form($form, &$form_state, $event) {
  $form_state['event'] = $event;
  $form['operation'] = array('#type' => 'hidden', '#value' => 'delete');
  $form['#submit'][] = 'intel_admin_annotation_delete_form_submit';
  $confirm_question = Intel_Df::t('Are you sure you want to delete the event %title?', array('%title' => $event['title']));
  return Intel_Form::confirm_form($form,
    $confirm_question,
    'admin/config/intel/settings/annotation/' . $event['key'] . '/edit',
    Intel_Df::t('This action cannot be undone.'),
    Intel_Df::t('Delete'),
    Intel_Df::t('Cancel'));
}

function intel_admin_annotation_delete_form_submit($form, &$form_state) {
  $event = $form_state['event'];
  $key = $event['key'];


  $events = get_option('intel_annotations_custom', array());
  unset($events[$key]);
  update_option('intel_annotations_custom', $events);

  $msg = Intel_Df::t('Intel event %title has been deleted.', array(
    '%title' => $event['title'],
  ));
  Intel_Df::drupal_set_message($msg);
  Intel_Df::drupal_goto('admin/config/intel/settings/annotation');
}

function intel_annotation_display_time_offset() {
  $timezone_info = intel_get_timezone_info();

  return $timezone_info['ga_offset'];
}