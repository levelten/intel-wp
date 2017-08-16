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

  include_once INTEL_DIR . 'includes/intel.demo.inc';

  $demo_mode = get_option('intel_demo_mode', '');
  $demo_settings = get_option('intel_demo_settings', intel_demo_settings_default());
  $form_state['demo_settings'] = $demo_settings;

  $form['general']['intel_demo_mode'] = array(
    '#type' => 'checkbox',
    '#title' => Intel_Df::t('Enable demo'),
    '#default_value' => !empty($demo_mode) ? $demo_mode : '',
    '#description' => Intel_Df::t('Enables spoof demo pages to be displayed on site. These pages are created programmatically and do not alter your database.'),
  );

  $form['posts'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Edit Posts'),
    //'#description' => Intel_Df::t('Warning: do not use these settings unless you really know what you are doing.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $posts = intel_demo_post_load();

  $posts_settings = !empty($demo_settings['posts']) ? $demo_settings['posts'] : array();
  foreach ($posts as $id => $post) {
    $post_settings = !empty($posts_settings[$id]) ? $posts_settings[$id] : array();
    $fieldset_key = 'posts_' . $id;
    $form['posts'][$fieldset_key] = array(
      '#type' => 'fieldset',
      '#title' => $post->post_title,
      //'#description' => Intel_Df::t('Warning: do not use these settings unless you really know what you are doing.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['posts'][$fieldset_key][$fieldset_key . '__post_content'] = array(
      '#type' => 'textarea',
      '#title' => Intel_Df::t('Content'),
      '#default_value' => !empty($post_settings['post_content']) ? $post_settings['post_content'] : $post->post_content,
      '#html' => 1,
      //'#description' => Intel_Df::t('Enter any goals that can be triggered by a form submission. Enter one goal per line as name,ga_goal_id (e.g. <em>Contact form,1</em>). Note in order for goals to track, they must also be setup properly in Google Analytics.'),
    );
  }

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => Intel_Df::t('Advanced'),
    //'#description' => Intel_Df::t('Warning: do not use these settings unless you really know what you are doing.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $options = array(
    '' => Intel_Df::t('Content and excerpt'),
    'content' => Intel_Df::t('Content only'),
    'excerpt' => Intel_Df::t('Excerpt only'),
    'title' => Intel_Df::t('Title only (neither content or excerpt)'),
  );
  $form['advanced']['post_list_content_fields'] = array(
    '#type' => 'select',
    '#title' => Intel_Df::t('Post list content fields'),
    '#options' => $options,
    '#default_value' => !empty($demo_settings['post_list_content_fields']) ? $demo_settings['post_list_content_fields'] : '',
    '#description' => Intel_Df::t('Set to the content fields to set on the post list page, intelligence/demo/blog.'),
  );

  $form['advanced']['css_injection'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('CSS injection'),
    '#default_value' => !empty($demo_settings['css_injection']) ? $demo_settings['css_injection'] : '',
    '#description' => Intel_Df::t('Custom CSS to inject into head on demo pages.'),
    '#html' => 1,
  );

  $form['advanced']['js_injection'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('JavaScript injection'),
    '#default_value' => !empty($demo_settings['js_injection']) ? $demo_settings['js_injection'] : '',
    '#description' => Intel_Df::t('Custom JavaScript to inject into head on demo pages.'),
    '#html' => 1,
  );




/*
  $form['advanced']['submission_goals'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('Submission goals'),
    '#default_value' => $value,
    '#description' => Intel_Df::t('Enter any goals that can be triggered by a form submission. Enter one goal per line as name,ga_goal_id (e.g. <em>Contact form,1</em>). Note in order for goals to track, they must also be setup properly in Google Analytics.'),
  );
*/

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

  if (!isset($demo_settings['posts'])) {
    $demo_settings['posts'] = array();
  }

  foreach ($values as $k => $v) {
    if ($k == 'intel_demo_mode') {
      update_option('intel_demo_mode', $v);
    }
    else {
      $a = explode('_', $k);
      if ($a[0] == 'posts') {
        if (empty($demo_settings['posts'][$a[1]] )) {
          $demo_settings['posts'][$a[1]] = array();
        }
        $b = explode('__', $k);
        if ($b[1] == 'post_content') {
          $v = stripslashes($v);
        }
        $demo_settings['posts'][$a[1]][$b[1]] = $v;
      }
      else {
        $demo_settings[$k] = $v;
      }

      if ($k == 'js_injection' || $k == 'css_injection') {
        $demo_settings[$k] = stripslashes($v);
      }

    }
  }

  update_option('intel_demo_settings', $demo_settings);

  Intel_Df::drupal_set_message(Intel_Df::t('Demo settings have been saved.'));

  //Intel_Df::drupal_goto('admin/config/intel/settings/general');
  //return 'admin/config/intel/settings/general';
}