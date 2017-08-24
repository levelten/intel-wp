<?php
/**
 * @file
 * Support for Intelligence demo
 *
 * @author Tom McCracken <tomm@getlevelten.com>
 */

function intel_demo_settings_default() {
  $settings = array();
  $settings['css_injection'] = "
img.attachment-post-thumbnail {
  display: none !important;
}
  ";
  return $settings;
}

function intel_demo_user_load($id = NULL) {
  $users = array();

  $users['-1'] = (object) array(
    'display_name' => 'Demo User 1'
  );

  $users['-2'] = (object) array(
    'display_name' => 'Demo User 2'
  );

  $users['-3'] = (object) array(
    'display_name' => 'Demo User 3'
  );

  if ($id) {
    return !empty($users[$id]) ? $users[$id] : FALSE;
  }

  return $users;
}

function intel_demo_count_user_posts($userid, $post_type = '') {
  $posts = intel_demo_post_load();
  $count = 0;
  foreach ($posts as $post) {
    if (!$post_type || $post->post_type == $post_type) {
      $count += ($post->post_author == $userid);
    }
  }
  return $count;
}

function intel_demo_term_load($id = NULL) {
  $terms = Intel_Df::drupal_static( __FUNCTION__, array());

  if (empty($terms)) {
    $term = array();
    $term['-1'] = array(
      'term_id' => -1,
      'name' => 'demo tag 1',
      'taxonomy' => 'post_tag',
      'count' => 0,
    );

    $term['-2'] = array(
      'term_id' => -2,
      'name' => 'demo tag 2',
      'taxonomy' => 'post_tag',
      'count' => 0,
    );

    $term['-3'] = array(
      'term_id' => -3,
      'name' => 'demo tag 3',
      'taxonomy' => 'post_tag',
      'count' => 0,
    );

    $term['-11'] = array(
      'term_id' => -11,
      'name' => 'demo category 1',
      'taxonomy' => 'category',
      'count' => 0,
    );

    $term['-12'] = array(
      'term_id' => -12,
      'name' => 'demo category 2',
      'taxonomy' => 'category',
      'count' => 0,
    );

    $posts = intel_demo_post_load();
    foreach ($posts as $post) {
      if (!empty($post->intel_demo['terms'])) {
        foreach ($post->intel_demo['terms'] as $tid) {
          $term["$tid"]['count']++;
        }
      }
    }

    foreach ($term as $i => $t) {
      $terms[$i] = (object)$t;
    }
  }


  if ($id) {
    return !empty($terms[$id]) ? $terms[$id] : FALSE;
  }

  return $terms;
}

function intel_demo_post_load($id = NULL, $options = array()) {
  $posts = Intel_Df::drupal_static( __FUNCTION__, array());

  $demo_settings = get_option('intel_demo_settings', intel_demo_settings_default());
  if (empty($posts)) {
    $post = array();

    $ipsums = array(
      'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer pulvinar molestie massa non aliquam. Nulla dolor metus, elementum vitae neque non, congue placerat sapien. Pellentesque mollis tortor in diam elementum accumsan. Fusce elementum sapien non massa imperdiet, a iaculis ante accumsan. Donec faucibus tempor velit, nec egestas libero placerat in. Nam vitae congue nisi. Integer mi elit, cursus et sapien vitae, hendrerit gravida quam. Mauris sit amet ante venenatis, posuere lectus vitae, fermentum enim. Vivamus fermentum eros mi, et elementum lorem lobortis et. Aenean consectetur mauris ac varius vehicula. Vestibulum condimentum ultrices sem, ac aliquet sapien congue et. Aenean eu felis a ligula mattis feugiat.',
      'Aenean elementum elit sed nibh sollicitudin efficitur. Maecenas tincidunt ornare accumsan. Duis eros mi, posuere ut justo at, eleifend aliquam erat. Proin in lectus eros. Ut non ullamcorper dolor. Aenean tincidunt ligula suscipit porta ullamcorper. Vivamus in placerat mi. Nam tempus lacus sit amet nisl auctor accumsan. Maecenas laoreet massa in aliquam ullamcorper. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Quisque pellentesque lacinia felis sit amet ornare. Sed a felis diam. Etiam egestas sed augue at maximus. Ut ultrices magna hendrerit porta dapibus. Morbi fermentum at nisl sed ultrices.',
      'Donec rhoncus nunc vitae turpis hendrerit, ac mattis massa ultrices. Interdum et malesuada fames ac ante ipsum primis in faucibus. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur non nisl ante. Ut eu tempor mauris. Sed non massa dapibus, euismod enim at, porttitor est. Donec luctus lobortis rutrum. Cras rhoncus diam at efficitur laoreet. Phasellus rhoncus elementum risus eu fermentum. Nam sit amet ornare turpis. Suspendisse placerat, risus vel dignissim sollicitudin, tortor lorem imperdiet purus, hendrerit ultrices justo felis ut magna. Mauris dapibus gravida nibh, sed semper ante congue ac.',
      'Curabitur pharetra odio sed felis hendrerit elementum. Phasellus bibendum eget risus a mattis. Maecenas egestas maximus lacus ac tincidunt. Vivamus dapibus ut est at bibendum. Suspendisse nec justo diam. Donec fermentum, lorem ut faucibus aliquet, felis erat condimentum leo, aliquet vestibulum tellus sapien eget enim. Vivamus sit amet orci urna. Pellentesque tempus magna leo, et consequat massa maximus non. Duis quis lacinia mauris, suscipit tincidunt libero. Cras tristique molestie mauris. Duis condimentum dui sapien, non aliquam arcu mattis vitae. Nam in nulla sed est finibus ultricies. Fusce accumsan diam vel eros eleifend, nec gravida leo mollis.',
      'Nullam diam orci, maximus sed diam at, sodales vestibulum purus. Aenean tincidunt lorem blandit porta efficitur. Curabitur lacus lacus, aliquam eu facilisis ut, mollis ut orci. In finibus nisi vitae dui aliquet placerat. Proin at urna eget magna egestas interdum sit amet eu risus. Mauris dapibus feugiat convallis. Nunc et ultrices erat. Ut blandit consequat diam feugiat ultricies. Nam lobortis mauris in libero vestibulum ultricies. Proin sagittis faucibus eros in condimentum. Nullam vel urna sit amet leo faucibus tincidunt quis at enim. Mauris vestibulum sem ac odio suscipit, ut dignissim orci blandit.',
    );

    $ipsums_s = array(
      'Lorem ipsum dolor sit amet, consectetur adipiscing elit.',
      'Integer pulvinar molestie massa non aliquam.',
      'Nulla dolor metus, elementum vitae neque non, congue placerat sapien.',
      'Pellentesque mollis tortor in diam elementum accumsan.',
    );

    $i = -1;
    $post["$i"] = array(
      'ID' => $i,
      'post_type' => 'page',
      'post_title' => Intel_Df::t('Demo Home'),
      'post_content' => 'Page content',
    );

    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_type' => 'page',
      'post_title' => 'Demo Page',
      'post_content' => 'Page content',
    );


    $content =  '<div class="contact-wrapper">' . "\n";
    $content .= '  <div class="col-left">' . "\n";
    $content .= '    <h3>Inquiry Form</h3>' . "\n";
    $content .= '      [intel_form name="intel_demo_contact_form"]' . "\n";
    $content .= '    </div>' . "\n";
    $content .= '    <div class="col-right">' . "\n";
    $content .= '      <h3>Contact Info</h3>' . "\n";
    $content .= '      <strong>' . Intel_Df::t('Email') . ':</strong><br>' . Intel_Df::l('info@example.com', 'mailto:info@example.com') . '<br><br>' . "\n";
    $content .= '      <strong>' . Intel_Df::t('Telephone') . ':</strong><br> ' . Intel_Df::l('214.555.1212', 'tel:214.555.1212') . '<br><br>' . "\n";
    $content .= '      <strong>' . Intel_Df::t('Address') . ':</strong><br> ' . Intel_Df::t('123 Easy Street') . '<br>' . Intel_Df::t('Dallas, TX 75201') . '<br><br>' . "\n";
    $content .= '    </div>' . "\n";
    $content .= '</div>' . "\n";
    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_type' => 'page',
      'post_title' => 'Demo Contact',
      'post_content' => $content,
    );

    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_author' => -2,
      'post_type' => 'post',
      'post_name' => 'intelligence/demo/blog/alpha',
      'guid' => 'intelligence/demo/blog/alpha',
      'post_title' => Intel_Df::t('Demo Post A'),
      'post_content' => $ipsums[0] . "\n\n" . $ipsums[1] . "\n\n" . $ipsums[2] . "\n\n" . $ipsums[3],
      'post_excerpt' => $ipsums[0],
      'comment_status' => 'open',
      'intel_demo' => array(
        'terms' => array(-1, -11, -12)
      ),
    );

    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_author' => -3,
      'post_type' => 'post',
      'post_name' => 'intelligence/demo/blog/beta',
      'post_title' => Intel_Df::t('Demo Post B'),
      'post_content' => $ipsums[4] . "\n\n" . '[embed]https://www.youtube.com/watch?v=A__S2YudnFI[/embed]' . "\n\n" . $ipsums[3] . "\n\n" . $ipsums[2]  . "\n\n" . $ipsums[1],
      'post_excerpt' => $ipsums[4],
      //'comment_status' => 'open',
      'intel_demo' => array(
        'terms' => array(-1, -2, -11)
      ),
    );

    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_author' => -3,
      'post_type' => 'post',
      'post_name' => 'intelligence/demo/blog/charlie',
      'post_title' => Intel_Df::t('Demo Post C'),
      'post_content' => $ipsums[1] . "\n\n" . $ipsums[2] . "\n\n" . $ipsums[3]  . "\n\n" . $ipsums[4] . "\n\n" . $ipsums[0] . "\n\n" . $ipsums[1] . "\n\n" . $ipsums[2],
      'post_excerpt' => $ipsums[1],
      //'comment_status' => 'open',
      'intel_demo' => array(
        'terms' => array(-1, -2, -3, -12)
      ),
    );

    $content =  '<div class="contact-wrapper">' . "\n";
    $content .= '  <div class="col-left" style="margin-bottom: 5em;">' . "\n";
    $content .= "    <p class=\"lead\">{$ipsums_s[0]}</p> \n\n {$ipsums[3]}";
    $content .= '  </div>' . "\n";
    $content .= '  <div class="col-right">' . "\n";
    $content .= '    <h3>Get it now!</h3>' . "\n";
    $content .= '    [intel_form name="intel_demo_offer_form"]' . "\n";
    $content .= '  </div>' . "\n";
    $content .= '</div>' . "\n";
    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_type' => 'page',
      'post_title' => 'Demo Offer A',
      'post_content' => $content,
    );

    $content =  '<div class="contact-wrapper" style="text-align: center; margin-bottom: 20em;">' . "\n";
    $content .= " <p class=\"lead\">{$ipsums_s[1]}</p> \n\n";
    $l_options = array(
      'html' => 1,
    );
    $l_options = Intel_Df::l_options_add_class('icon-link', $l_options);
    $content .= Intel_Df::l( '<i class="fa fa-arrow-circle-down" aria-hidden="true" style="font-size: 5em;"></i>', '/wp-content/plugins/intelligence/images/setup_intel_action.png', $l_options);
    $content .= "\n\n";
    $content .= '</div>' . "\n";
    $i--;
    $post["$i"] = array(
      'ID' => $i,
      'post_type' => 'page',
      'post_title' => 'Demo Offer A Response',
      'post_content' => $content,
    );

    $post_defaults = array(
      'ID' => -99,
      'post_author' => -1,
      'post_date' => current_time('mysql'),
      'post_date_gmt' => current_time('mysql', 1),
      'post_content' => '',
      'post_title' => Intel_Df::t("Page Title"),
      'post_excerpt' => "",
      'post_status' => 'static',
      'comment_status' => 'closed',
      'ping_status' => 'open',
      'post_name' => 'intelligence/demo',
      'guid' => get_bloginfo('wpurl') . '/' . 'intelligence/demo',
      'post_type' => 'page',
      'comment_count' => 0,
      //'post_modified' => current_time('mysql'),
      //'post_modified_gmt' => current_time('mysql', 1),
      'filter' => 'raw',
    );


    /*

    $post['-4']['post_content'] = $post['-4']['post_excerpt'] = $ipsums[0];
    $post['-5']['post_content'] = $post['-5']['post_excerpt'] = $ipsums[4];
    $post['-6']['post_content'] = $post['-6']['post_excerpt'] = $ipsums[1];

    $post['-4']['post_content'] .= "\n\n" . $ipsums[1] . "\n\n" . $ipsums[2] . "\n\n" . $ipsums[3];
    $post['-5']['post_content'] .= "\n\n" . '[embed]https://www.youtube.com/watch?v=A__S2YudnFI[/embed]' . "\n\n" . $ipsums[3] . "\n\n" . $ipsums[2]  . "\n\n" . $ipsums[1];
    $post['-6']['post_content'] .= "\n\n" . $ipsums[2] . "\n\n" . $ipsums[3]  . "\n\n" . $ipsums[4] . "\n\n" . $ipsums[0] . "\n\n" . $ipsums[1] . "\n\n" . $ipsums[2];

    */

    foreach ($post as $i => $p) {
      $posts[$i] = ($p + $post_defaults);
      if (!empty($demo_settings['posts'][$i]) && is_array($demo_settings['posts'][$i])) {
        $posts[$i] = $demo_settings['posts'][$i] + $posts[$i];
      }
      $posts[$i] = (object)$posts[$i];
    }
  }

  if ($id) {
    return !empty($posts[$id]) ? $posts[$id] : FALSE;
  }

  return $posts;
};

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


  $i = -1;
  $urls = array(
    'intelligence/demo' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/page' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/contact' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/blog' => array(
      'entity' => array(
        'type' => 'post',
        'id' => array($i, $i-1, $i-2),
      ),
      'wp_query' => array(
        'is_page' => false,
        'is_singular' => false,
      ),
    ),
    'intelligence/demo/blog/alpha' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/blog/beta' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/blog/charlie' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/offer/alpha' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),
    'intelligence/demo/offer/alpha-response' => array(
      'entity' => array(
        'type' => 'post',
        'id' => $i--,
      ),
    ),

  );

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

    $demo_settings = get_option('intel_demo_settings', intel_demo_settings_default());

    wp_enqueue_script( 'intel_demo', INTEL_URL . 'js/intel.demo.js', array( 'jquery' ), intel()->get_version(), false );
    wp_enqueue_style( 'intel_demo', INTEL_URL . 'css/intel.demo.css', array(), intel()->get_version(), 'all');
    wp_enqueue_style( 'intel_wpb-fa', 'https://maxcdn.bootstrapcdn.com/font-awesome/4.4.0/css/font-awesome.min.css' );

    if (!empty($demo_settings['css_injection'])) {
      wp_add_inline_style( 'intel_demo', $demo_settings['css_injection'] );
    }
    if (!empty($demo_settings['js_injection'])) {
      wp_add_inline_script( 'intel_demo', $demo_settings['js_injection'] );
    }

    // suppress warnings on demo pages;
    error_reporting(E_ERROR);


    $wp_query_data = !empty($urls[$intel_demo_page_url]['wp_query']) ? $urls[$intel_demo_page_url]['wp_query'] : array();
    $posts_data = array();
    if ($urls[$intel_demo_page_url]['entity']) {
      $ids = $urls[$intel_demo_page_url]['entity']['id'];
      if (!is_array($ids)) {
        $ids = array($ids);
      }
      foreach ($ids as $id) {
        $posts_data[] = intel_demo_post_load($id);
      }
    }

    // build menu
    $menu = '';
    foreach ($menu_items as $i) {
      if ($menu) {
        $menu .= ' | ';
      }
      $menu .= Intel_Df::l($i['text'], $i['path'], !empty($i['options']) ? $i['options'] : array());
    }

    $demo_terms = intel_demo_term_load();

    $posts = array();
    $post_count = count($posts_data);
    foreach ($posts_data as $i => $post_data) {
      // create a fake virtual page
      $post = new stdClass;
      $post_data = (array)$post_data;
      foreach ($post_data as $k => $v) {
        $post->{$k} = $v;
      }
      //$_post = new WP_Post( $post );
      $_post = $post;

      //$excerpt = get_the_excerpt( $_post );
      //$excerpt = apply_filters( 'get_the_excerpt', $post->post_excerpt, $post );

      $content = '';
      $excerpt = '';
      if ($post_count > 1) {
        $wp_query->query_vars['title'] = 'bob';
      }
      if ($i == 0) {
        $content .= '<div class="intel-demo-menu">' . $menu . '</div>';
        $excerpt .= '<div class="intel-demo-menu">' . $menu . '</div>';
      }

      if ($_post->post_type == 'post') {
        if ($post_count == 1) {
          $content .= '<div class="intel-demo-featured-image"><img src="http://via.placeholder.com/1280x720&text=Featured+Image"></div>';
        }
        else {
          $excerpt .= '<div class="intel-demo-featured-image"><img src="http://via.placeholder.com/1280x360&text=Featured+Image"></div>';
        }
      }

      if ($_post->post_type == 'post' && $post_count == 1) {
        $content .= '<div class="intel-demo-social-share social-wrapper social-share">' . intel_demo_social_share_buttons($post) . '</div>';
      }

      if ($_post->post_type == 'post' && $post_count == 1) {
        $terms = array(
          'category' => array(),
          'post_tag' => array(),
        );

        if (!empty($_post->intel_demo['terms'])) {
          foreach ($_post->intel_demo['terms'] as $tid) {
            if (!empty($demo_terms[$tid])) {
              $t = $demo_terms[$tid];
              $terms[$t->taxonomy][] = $t->name;
            }
          }
        }
        $content .= '<div class="intel-demo-terms terms-wrapper">';
        $content .= '<div class="intel-demo-term-category term-wrapper term-category">' . Intel_Df::t('Categories') . ': ';
        if (!empty($terms['category'])) {
          $content .= implode(', ', $terms['category']);
        }
        else {
          $content .= Intel_Df::t('(none)');
        }
        $content .= '</div>';
        $content .= '<div class="intel-demo-term-tag term-wrapper term-tag">' . Intel_Df::t('Tags') . ': ';
        if (!empty($terms['post_tag'])) {
          $content .= implode(', ', $terms['post_tag']);
        }
        else {
          $content .= Intel_Df::t('(none)');
        }
        $content .= '</div></div>';

      }

      $content .= '<div class="intel-demo-body">' . $_post->post_content . '</div>';
      $excerpt .= '<div class="intel-demo-body">' . $_post->post_excerpt . '</div>';

      if ($_post->post_type == 'post') {
        if ($post_count == 1) {
          $l_options = array(
            'html' => 1,
            'attributes' => array(
              'title' => Intel_Df::t('Call To Action'),
            ),
          );
          $l_options = Intel_Df::l_options_add_class(array('track-cta'), $l_options);
          //$content .= '<div class="intel-demo-cta">' . Intel_Df::l('<img src="http://via.placeholder.com/1280x360&text=Call+To+Action" width="1280" height="360">', 'intelligence/demo/contact', $l_options) . '</div>';
          $content .= '<div class="intel-demo-cta">' . Intel_Df::l('<img src="http://via.placeholder.com/1280x360&text=Call+To+Action" width="1280" height="360">', 'intelligence/demo/offer/alpha', $l_options) . '</div>';
        }
      }


      $content .= '<div class="intel-demo-footer">';

      if ($post_count == 1) {
        $content .= '<div class="intel-demo-social-profile social-wrapper social-profile">' . intel_demo_social_profile_buttons($post) . '</div>';
      }

      $content .= '</div>';

      // if post list page, set content/excerpt fields based on settings
      if ($post_count > 1) {
        $_post->post_content = '';
        $_post->post_excerpt = '';
        if (empty($demo_settings['post_list_content_fields']) || $demo_settings['post_list_content_fields'] == 'content') {
          $_post->post_content = '<div class="intel-demo-post-content">' . $excerpt . '</div>';
        }
        if (empty($demo_settings['post_list_content_fields']) || $demo_settings['post_list_content_fields'] == 'excerpt') {
          $_post->post_excerpt = '<div class="intel-demo-post-content post-excerpt">' . $excerpt . '</div>';
        }
      }
      else {
        $_post->post_content = '<div class="intel-demo-post-content">' . $content . '</div>';
        $_post->excerpt = '';
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

/* couldnt get this to work
function intel_demo_wp_title($title, $sep, $seplocation = NULL) {
  intel_d($title);
  intel_d($sep);
  intel_d($seplocation);
  return $title;
}
//add_filter('wp_title', 'intel_demo_wp_title', 10, 3);
*/

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

  $vars = intel_demo_form_submit_intel_vars($form, $form_state);

  // process submission data
  intel_process_form_submission($vars);

  $msg = Intel_Df::t('Thank you for contacting us. We will get back to you shortly.');
  Intel_Df::drupal_set_message($msg, 'status');

  // since the form is embedded with a shortcode, the page has already built and
  // page alter already been sent
  // print io commands directly to page

  //$script = intel()->tracker->get_pushes_script();
  //print "\n$script";
}

function intel_demo_form_submit_intel_vars($form, &$form_state) {
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

  return $vars;
}

function intel_demo_offer_form($form, &$form_state) {
  $form = intel_demo_contact_form($form, $form_state);
  unset($form['message']);

  return $form;
}

function intel_demo_offer_form_submit($form, &$form_state) {

  $vars = intel_demo_form_submit_intel_vars($form, $form_state);

  // process submission data
  intel_process_form_submission($vars);

  $url = Intel_Df::url(Intel_Df::current_path() . '-response');
  //$msg = Intel_Df::t('Thank you for your offer submission.');
  //Intel_Df::drupal_set_message($msg, 'status');

  intel_save_flush_page_intel_pushes();

  print '<script> window.location = "' . $url . '";</script>';

  //Intel_Df::drupal_goto(Intel_Df::current_path() . '-response');
}