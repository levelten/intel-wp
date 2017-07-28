<?php
/**
 * @file
 * Support for Intelligence demo
 *
 * @author Tom McCracken <tomm@getlevelten.com>
 */

function intel_demo_posts_alter($posts){
  global $wp;
  global $wp_query;
  global $wp_rewrite;

  global $intel_demo_page_url; // used to stop double loading

  if (isset($intel_demo_page_url)) {
    return $posts;
  }

  $menu_items = array();
  $menu_items[] = array(
    'text' => 'Home',
    'path' => 'intelligence/demo/',
  );
  $menu_items[] = array(
    'text' => 'Page',
    'path' => 'intelligence/demo/page',
  );
  $menu_items[] = array(
    'text' => 'Blog',
    'path' => 'intelligence/demo/blog',
  );
  $menu_items[] = array(
    'text' => 'Contact',
    'path' => 'intelligence/demo/contact',
  );

  $urls = array(
    'intelligence/demo' => array(
      'posts' => array(
        array(
          'post_type' => 'page',
          'post_title' => Intel_Df::t('Home'),
          'post_content' => 'Page content',
        ),
      ),
    ),
    'intelligence/demo/page' => array(
      'posts' => array(
        array(
          'post_type' => 'page',
          'post_title' => 'Page',
          'post_content' => 'Page content',
        ),
      ),
    ),
    'intelligence/demo/contact' => array(
      'posts' => array(
        array(
          'post_type' => 'page',
          'post_title' => 'Contact',
          'post_content' => '[intel_form name="intel_demo_contact_form"]',
        ),
      ),
    ),
    'intelligence/demo/blog' => array(
      'posts' => array(),
      'wp_query' => array(
        'is_page' => false,
        'is_singular' => false,
      ),
    ),
    'intelligence/demo/blog/alpha' => array(
      'posts' => array(
        array(
          'post_type' => 'post',
          'post_name' => 'intelligence/demo/blog/alpha',
          'guid' => 'intelligence/demo/blog/alpha',
          'post_title' => Intel_Df::t('Post A'),
          'post_content' => 'Alpha content',
          'comment_status' => 'open',
        ),
      ),
    ),
    'intelligence/demo/blog/beta' => array(
      'posts' => array(
        array(
          'post_type' => 'post',
          'post_name' => 'intelligence/demo/blog/beta',
          'post_title' => Intel_Df::t('Post B'),
          'post_content' => 'Beta content',
          'comment_status' => 'open',
        ),
      ),
    ),
    'intelligence/demo/blog/charlie' => array(
      'posts' => array(
        array(
          'post_type' => 'post',
          'post_name' => 'intelligence/demo/blog/charlie',
          'post_title' => Intel_Df::t('Post C'),
          'post_content' => 'Beta content',
          'comment_status' => 'open',
        ),
      ),
    ),
  );

  $ipsums = array(
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer pulvinar molestie massa non aliquam. Nulla dolor metus, elementum vitae neque non, congue placerat sapien. Pellentesque mollis tortor in diam elementum accumsan. Fusce elementum sapien non massa imperdiet, a iaculis ante accumsan. Donec faucibus tempor velit, nec egestas libero placerat in. Nam vitae congue nisi. Integer mi elit, cursus et sapien vitae, hendrerit gravida quam. Mauris sit amet ante venenatis, posuere lectus vitae, fermentum enim. Vivamus fermentum eros mi, et elementum lorem lobortis et. Aenean consectetur mauris ac varius vehicula. Vestibulum condimentum ultrices sem, ac aliquet sapien congue et. Aenean eu felis a ligula mattis feugiat.',
    'Aenean elementum elit sed nibh sollicitudin efficitur. Maecenas tincidunt ornare accumsan. Duis eros mi, posuere ut justo at, eleifend aliquam erat. Proin in lectus eros. Ut non ullamcorper dolor. Aenean tincidunt ligula suscipit porta ullamcorper. Vivamus in placerat mi. Nam tempus lacus sit amet nisl auctor accumsan. Maecenas laoreet massa in aliquam ullamcorper. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Quisque pellentesque lacinia felis sit amet ornare. Sed a felis diam. Etiam egestas sed augue at maximus. Ut ultrices magna hendrerit porta dapibus. Morbi fermentum at nisl sed ultrices.',
    'Donec rhoncus nunc vitae turpis hendrerit, ac mattis massa ultrices. Interdum et malesuada fames ac ante ipsum primis in faucibus. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur non nisl ante. Ut eu tempor mauris. Sed non massa dapibus, euismod enim at, porttitor est. Donec luctus lobortis rutrum. Cras rhoncus diam at efficitur laoreet. Phasellus rhoncus elementum risus eu fermentum. Nam sit amet ornare turpis. Suspendisse placerat, risus vel dignissim sollicitudin, tortor lorem imperdiet purus, hendrerit ultrices justo felis ut magna. Mauris dapibus gravida nibh, sed semper ante congue ac.',
    'Curabitur pharetra odio sed felis hendrerit elementum. Phasellus bibendum eget risus a mattis. Maecenas egestas maximus lacus ac tincidunt. Vivamus dapibus ut est at bibendum. Suspendisse nec justo diam. Donec fermentum, lorem ut faucibus aliquet, felis erat condimentum leo, aliquet vestibulum tellus sapien eget enim. Vivamus sit amet orci urna. Pellentesque tempus magna leo, et consequat massa maximus non. Duis quis lacinia mauris, suscipit tincidunt libero. Cras tristique molestie mauris. Duis condimentum dui sapien, non aliquam arcu mattis vitae. Nam in nulla sed est finibus ultricies. Fusce accumsan diam vel eros eleifend, nec gravida leo mollis.',
    'Nullam diam orci, maximus sed diam at, sodales vestibulum purus. Aenean tincidunt lorem blandit porta efficitur. Curabitur lacus lacus, aliquam eu facilisis ut, mollis ut orci. In finibus nisi vitae dui aliquet placerat. Proin at urna eget magna egestas interdum sit amet eu risus. Mauris dapibus feugiat convallis. Nunc et ultrices erat. Ut blandit consequat diam feugiat ultricies. Nam lobortis mauris in libero vestibulum ultricies. Proin sagittis faucibus eros in condimentum. Nullam vel urna sit amet leo faucibus tincidunt quis at enim. Mauris vestibulum sem ac odio suscipit, ut dignissim orci blandit.',
  );

  $urls['intelligence/demo/blog/alpha']['posts'][0]['post_content'] = $ipsums[0];
  $urls['intelligence/demo/blog/beta']['posts'][0]['post_content'] = $ipsums[4];
  $urls['intelligence/demo/blog/charlie']['posts'][0]['post_content'] = '[embed]https://www.youtube.com/watch?v=A__S2YudnFI[/embed]';

  $urls['intelligence/demo/blog']['posts'][] = $urls['intelligence/demo/blog/alpha']['posts'][0];
  $urls['intelligence/demo/blog']['posts'][] = $urls['intelligence/demo/blog/beta']['posts'][0];
  $urls['intelligence/demo/blog']['posts'][] = $urls['intelligence/demo/blog/charlie']['posts'][0];

  $urls['intelligence/demo/blog/alpha']['posts'][0]['post_content'] .= "\n\n" . $ipsums[1] . "\n\n" . $ipsums[2] . "\n\n" . $ipsums[3];
  $urls['intelligence/demo/blog/beta']['posts'][0]['post_content'] .= "\n\n" . $ipsums[3] . "\n\n" . $ipsums[2]  . "\n\n" . $ipsums[1] . "\n\n" . $ipsums[0] . "\n\n" . $ipsums[4] . "\n\n" . $ipsums[3];
  $urls['intelligence/demo/blog/charlie']['posts'][0]['post_content'] .= "\n\n" . $ipsums[2] . "\n\n" . $ipsums[3]  . "\n\n" . $ipsums[4];

  // determine current page url
  $page_url = '';
  if (!empty($wp->request)) {
    $page_url = strtolower($wp->request);
  }
  elseif (!empty($wp->query_vars['page_id'])) {
    $page_url = $wp->query_vars['page_id'];
  }
  if ( empty($intel_demo_page_url) && !empty($urls[$page_url]) ) {
    // stop interferring with other $posts arrays on this page (only works if the sidebar is rendered *after* the main page)
    $intel_demo_page_url = $page_url;

    //wp_enqueue_script( 'intel_demo', INTEL_URL . 'js/intel.demo.js', array( 'jquery' ), intel()->get_version(), false );
    wp_enqueue_style( 'intel_demo', INTEL_URL . 'css/intel.demo.css', array(), intel()->get_version(), 'all');
    wp_enqueue_style( 'intel_wpb-fa', 'https://maxcdn.bootstrapcdn.com/font-awesome/4.4.0/css/font-awesome.min.css' );

    $posts_data = !empty($urls[$intel_demo_page_url]['posts']) ? $urls[$intel_demo_page_url]['posts'] : array();
    $wp_query_data = !empty($urls[$intel_demo_page_url]['wp_query']) ? $urls[$intel_demo_page_url]['wp_query'] : array();

    $post_defaults = array(
      'ID' => -999,
      'post_author' => 1,
      'post_date' => current_time('mysql'),
      'post_date_gmt' => current_time('mysql', 1),
      'post_content' => '',
      'post_title' => Intel_Df::t("Page Title"),
      'post_excerpt' => "",
      'post_status' => 'static',
      'comment_status' => 'closed',
      'ping_status' => 'open',
      'post_name' => $intel_demo_page_url,
      'guid' => get_bloginfo('wpurl') . '/' . $intel_demo_page_url,
      'post_type' => 'page',
      'comment_count' => 0,
      //'post_modified' => current_time('mysql'),
      //'post_modified_gmt' => current_time('mysql', 1),
      'filter' => 'raw',
    );

    // build menu
    $menu = '';
    foreach ($menu_items as $i) {
      if ($menu) {
        $menu .= ' | ';
      }
      $menu .= Intel_Df::l($i['text'], $i['path'], !empty($i['options']) ? $i['options'] : array());
    }

    $posts = array();
    $post_count = count($posts_data);
    foreach ($posts_data as $i => $post_data) {

      $post_data += $post_defaults;


      // create a fake virtual page
      $post = new stdClass;
      foreach ($post_data as $k => $v) {
        $post->{$k} = $v;
      }
      //$_post = new WP_Post( $post );
      $_post = $post;

      //$excerpt = get_the_excerpt( $_post );
      //$excerpt = apply_filters( 'get_the_excerpt', $post->post_excerpt, $post );

      $content = '';
      if ($i == 0) {
        $content .= '<div class="intel-demo-menu">' . $menu . '</div>';
        //$_post->post_title = '<div class="intel-demo-menu-placeholder"></div>' . $_post->post_title;
      }

      if ($_post->post_type == 'post') {
        if ($post_count == 1) {
          $content .= '<div class="intel-demo-featured-image"><img src="http://via.placeholder.com/1280x720&text=Featured+Image"></div>';
        }
        else {
          $content .= '<div class="intel-demo-featured-image"><img src="http://via.placeholder.com/1280x360&text=Featured+Image"></div>';
        }
      }

      if ($_post->post_type == 'post' && $post_count == 1) {
        $content .= '<div class="intel-demo-social-share social-wrapper social-share">' . intel_demo_social_share_buttons($post) . '</div>';
      }

      $content .= '<div class="intel-demo-body">' . $_post->post_content . '</div>';

      if ($_post->post_type == 'post') {
        if ($post_count == 1) {
          $l_options = array(
            'html' => 1
          );
          $l_options = Intel_Df::l_options_add_class(array('track-cta'), $l_options);
          $content .= '<div class="intel-demo-cta">' . Intel_Df::l('<img src="http://via.placeholder.com/1280x360&text=Call+To+Action">', 'intelligence/demo/contact', $l_options) . '</div>';
        }
      }


      if ($post_count == 1) {
        $content .= '<div class="intel-demo-social-profile social-wrapper social-profile">' . intel_demo_social_profile_buttons($post) . '</div>';
      }

      $_post->post_content = '<div class="intel-demo-post-content">' . $content . '</div>';

      if (empty($_post->post_excerpt)) {
        $_post->post_excerpt = '<div class="intel-demo-post-content post-excerpt">' . $content . '</div>';
      }

      $posts[] = $_post;
    }


    // configure wp_query to make this page look real
    $wp_query_defaults = array(
      'is_page' => true,
      'is_singular' => true,
      'is_home' => false,
      'is_archive' => false,
      'is_category' => false,
      'is_404' => false,
    );

    $wp_query_data += $wp_query_defaults;

    foreach ($wp_query_data as $k => $v) {
      $wp_query->{$k} = $v;
    }

    unset($wp_query->query["error"]);
    $wp_query->query_vars["error"]="";

    // remove permalink structure so demo blog posts will not follow site pattern
    $wp_rewrite->permalink_structure = '';

    add_filter( 'pre_option_permalink_structure' , 'intel_demo_option_permalink_structure', -10, 2 );
  }

  return $posts;
}

function intel_demo_social_share_buttons($post) {
  $urls_info = array(
    'facebook' => array(
      'hostpath' => 'https://www.facebook.com/sharer.php',
      'icon_class' => 'fa fa-facebook-square',
      'query' => array(
        'url' => 'u',
      ),
    ),
    'googleplus' => array(
      'hostpath' => 'https://plus.google.com/share',
      'icon_class' => 'fa fa-google-plus-square',
      'query' => array(
        'url' => 'url',
      ),
    ),
    'linkedin' => array(
      'hostpath' => 'https://www.linkedin.com/shareArticle',
      'icon_class' => 'fa fa-linkedin-square',
      'query' => array(
        'url' => 'url',
        'title' => 'title',
      ),
    ),
    'pinterest' => array(
      'hostpath' => 'https://pinterest.com/pin/create/bookmarklet/',
      'icon_class' => 'fa fa-pinterest-square',
      'query' => array(
        'url' => 'url',
        'title' => 'description',
        'img' => 'media',
      ),
    ),
    'twitter' => array(
      'hostpath' => 'https://twitter.com/intent/tweet',
      'icon_class' => 'fa fa-twitter-square',
      'query' => array(
        'url' => 'url',
        'title' => 'text',
      ),
    ),
  );
  $platforms = array(
    'twitter',
    'facebook',
    'googleplus',
    'linkedin',
    'pinterest'
  );
  $output = '';
  $intel = intel();
  foreach ($platforms as $platform) {
    $url_info = $urls_info[$platform];
    $url = '';
    if (!empty($url_info['query']['url'])) {
      $url .= $url ? '&' : '';
      $url .= $url_info['query']['url'] . '=' . $intel->base_url . $intel->base_path . $post->post_name;
    }
    if (!empty($url_info['query']['title'])) {
      $url .= $url ? '&' : '';
      $url .= $url_info['query']['title'] . '=' . $post->post_title;
    }

    if ($url) {
      $url = '?' . $url;
    }

    $url = $url_info['hostpath'] . $url;

    $l_option = array(
      'html' => 1,
    );
    $l_option = Intel_Df::l_options_add_target('_blank', $l_option);
    $l_option = Intel_Df::l_options_add_class(array('social-link','social-share'), $l_option);
    $output .= Intel_Df::l('<i class="' . $url_info['icon_class'] . ' social-icon" aria-hidden="true"></i>', $url, $l_option);
  }

  $output = '<div class="social-share-text social-text">' . Intel_Df::t('Share Post') . ' &raquo; </div><div class="social-share-buttons social-buttons">' . $output . '</div>';
  //$output = '<div class="social-wrapper social-share"><div class="social-share-text">' . Intel_Df::t('Share Post') . ' >> </div>' . $output . '</div>';

  return $output;
}

function intel_demo_social_profile_buttons($post) {
  $urls_info = array(
    'facebook' => array(
      'hostpath' => 'http://www.facebook.com/levelten',
      'icon_class' => 'fa fa-facebook-square',
    ),
    'googleplus' => array(
      'hostpath' => 'https://plus.google.com/share',
      'icon_class' => 'fa fa-google-plus-square',
    ),
    'linkedin' => array(
      'hostpath' => 'https://www.linkedin.com/shareArticle',
      'icon_class' => 'fa fa-linkedin-square',
    ),
    'pinterest' => array(
      'hostpath' => 'https://pinterest.com/pin/create/bookmarklet/',
      'icon_class' => 'fa fa-pinterest-square',
    ),
    'twitter' => array(
      'hostpath' => '//twitter.com/levelten',
      'icon_class' => 'fa fa-twitter-square',
    ),
    'youtube' => array(
      'hostpath' => 'http://www.youtube.com/user/resultsorientedweb?sub_confirmation=1',
      'icon_class' => 'fa fa-youtube-square',
    ),
  );
  $platforms = array(
    'twitter',
    'facebook',
    'googleplus',
    'linkedin',
    'youtube',
  );
  $output = '';
  $intel = intel();
  foreach ($platforms as $platform) {
    $url_info = $urls_info[$platform];

    $url = $url_info['hostpath'];

    $l_option = array(
      'html' => 1,
    );
    $l_option = Intel_Df::l_options_add_target('_blank', $l_option);
    $l_option = Intel_Df::l_options_add_class(array('social-link', 'social-share'), $l_option);
    $output .= Intel_Df::l('<i class="' . $url_info['icon_class'] . ' social-icon" aria-hidden="true"></i>', $url, $l_option);
  }


  $output = '<div class="social-profile-text social-text">' . Intel_Df::t('Follow Us') . ' &raquo; </div><div class="social-profile-buttons social-buttons">' . $output . '</div>';
  //$output = '<div class="social-wrapper social-profile">' . Intel_Df::t('Follow Us') . ' >> ' . $output . '</div>';


  return $output;
}



/**
 * Overrides WordPress options set in $intel_wp_config_options global.
 *
 * @param $value
 * @param $name
 * @return array|mixed|object|string|void
 */
function intel_demo_option_permalink_structure($value, $name) {
  return '%postname%';
}

/*
function intel_demo_post_link( $post_link, $post, $leavename ) {
  intel_d($post_link);
  if ( 'post' == $post->post_type) {
    return $post_link . 'hi';
  }

  return $post_link;
}
add_filter( 'post_type_link', 'intel_demo_post_link', 10, 3 );
*/

/**
 * Form Test function
 */
function intel_demo_contact_form($form, &$form_state) {

  $account = wp_get_current_user();

  $form['givenName'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('First name'),
    '#default_value' => !empty($account->user_firstname) ? $account->user_firstname : Intel_Df::t('Tommy'),
    //'#description' => Intel_Df::t('Input family name.'),
    '#required' => 1,
  );

  $form['familyName'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Last name'),
    '#default_value' => !empty($account->user_lastname) ? $account->user_lastname : Intel_Df::t('Tester'),
    //'#default_value' => !empty($defaults['test']) ? $defaults['test'] : '',
    //'#description' => Intel_Df::t('Input family name.'),
  );

  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => Intel_Df::t('Email'),
    '#default_value' => !empty($account->user_email) ? $account->user_email : '',
    //'#default_value' => !empty($defaults['test']) ? $defaults['test'] : '',
    //'#description' => Intel_Df::t('Input family name.'),
    '#required' => 1,
  );

  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => Intel_Df::t('Message'),
    '#default_value' => !empty($account->user_email) ? $account->user_email : '',
    //'#default_value' => !empty($defaults['test']) ? $defaults['test'] : '',
    //'#description' => Intel_Df::t('Input family name.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => Intel_Df::t('Submit'),
    '#prefix' => '<br>',
  );

  return $form;
}

function intel_demo_contact_form_validate($form, &$form_state) {
  //$_SESSION['intel_weform_test']['time0'] = microtime (TRUE);
}

function intel_demo_contact_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  // get initialied var structure
  $vars = intel_form_submission_vars_default();

  // create pointer aliases
  $submission = &$vars['submission'];
  $track = &$vars['track'];

  // set visitor properties from webform values
  $vp_info = intel()->visitor_property_info();
  foreach ($values as $k => $v) {
    if (!empty($vp_info['data.' . $k])) {
      $vars['visitor_properties']['data.' . $k] = $v;
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

  $goals = intel_goal_load();
  if (!empty($goals['contact'])) {
    $track['name'] = 'form_submission__contact';
  }
  else {
    $track['name'] = 'form_submission';
  }

  // process submission data
  intel_process_form_submission($vars);

  $msg = Intel_Df::t('Thank you for contacting us. We will get back to you shortly.');
  Intel_Df::drupal_set_message($msg, 'status');

  // save l10i.js settings till page refresh
  intel_save_flush_page_intel_pushes();

  //$url = intel_cache_busting_url(Intel_Df::current_path());
  //Intel_Df::drupal_goto($url);

  //Intel_Df::drupal_goto('intelligence/demo/contact-submission');

  //unset($_SESSION['intel_weform_test']);
}