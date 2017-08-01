<?php
/**
 * @file
 * Admin configuration management
 */

include_once INTEL_DIR . 'includes/class-intel-form.php';

/**
 * Displays the form for the standard settings tab.
 *
 * @return array
 * A structured array for use with Forms API.
 */
function intel_admin_demo_settings($form, &$form_state) {
  //global $base_url;

  include_once INTEL_DIR . 'includes/intel.ga.inc';

  $demo_settings = get_option('intel_demo_settings', array());
  $form_state['demo_settings'] = $demo_settings;

  $form['general']['status'] = array(
    '#type' => 'checkbox',
    '#title' => Intel_Df::t('Enable demo'),
    '#default_value' => !empty($demo_settings['status']) ? $demo_settings['status'] : '',
    '#description' => Intel_Df::t('Enables phonecall tracking features.'),
  );

  $form['post_display'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Content display'),
    //'#description' => Intel_Df::t('Warning: do not use these settings unless you really know what you are doing.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $options = array(
    '' => Intel_Df::t('Content and excerpt'),
    'content' => Intel_Df::t('Content only'),
    'excerpt' => Intel_Df::t('Excerpt only'),
    'title' => Intel_Df::t('Title only (neither content or excerpt)'),
  );
  $form['post_display']['post_list_content_fields'] = array(
    '#type' => 'select',
    '#title' => Intel_Df::t('Post list content fields'),
    '#options' => $options,
    '#default_value' => !empty($demo_settings['post_list_content_fields']) ? $demo_settings['post_list_content_fields'] : '',
    '#description' => Intel_Df::t('Set to the content fields to set on the post list page, intelligence/demo/blog.'),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => Intel_Df::t('Save settings'),
  );
  return $form;
  //return system_settings_form($form);
}

function intel_admin_demo_settings_validate($form, &$form_state) {

}

function intel_admin_demo_settings_submit($form, &$form_state) {
  $values = $form_state['values'];

  $demo_settings = $form_state['demo_settings'];

  foreach ($values as $k => $v) {
    $demo_settings[$k] = $v;
  }

  update_option('intel_demo_settings', $demo_settings);

  Intel_Df::drupal_set_message(Intel_Df::t('Demo settings have been saved.'));

  //Intel_Df::drupal_goto('admin/config/intel/settings/general');
  //return 'admin/config/intel/settings/general';
}