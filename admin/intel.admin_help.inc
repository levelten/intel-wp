<?php
/**
 * @file
 * Administration of visitors
 * 
 * @author Tom McCracken <tomm@getlevelten.com>
 */


function intel_help() {
  $output = '';

  $output .= '<article>';
  $output .= '<div class="card"><div class="card-block">';

  $output .= '<p class="lead">';
  $output .= Intel_Df::t('Intelligence is designed to extend Google Analytics to track meaningful visitor interactions on your website.');
  $output .= ' ' . Intel_Df::t('This tutorial will walk you through the essentials of extending Google Analytics using Intelligence to create results oriented analytics.');
  $output .= '</p>';

  $output .= '<h3>' . Intel_Df::t('Extending Google Analytics') . '</h3>';
  $output .= '<p>';
  $output .= Intel_Df::t('The default installation of Google Analytics tracks everything based on pageviews.');
  $output .= ' ' . Intel_Df::t('Pageviews provide a lot of default metrics such as sessions, page hits and time on site.');
  $output .= ' ' . Intel_Df::t('However, the default installation does not track more meaningful interactions such as when someone clicks on an important link or submits a form.');
  $output .= '</p>';

  $output .= '<p>';
  $output .= Intel_Df::t('GA provides several advanced features such as events and goals that can be used to provide more insightful data.');
  $output .= ' ' . Intel_Df::t('Intelligence helps streamline implementing these advanced features on your site.');
  $output .= ' ' . Intel_Df::t('The key is to understand how Intelligence events can be leverage to track Google Analytics events and goals.');
  $output .= '</p>';

  $output .= '<p>';
  $output .= ' ' . Intel_Df::t('For this tutorial we will want to view the events we are sending to Google Anatlyics.');
  $output .= ' ' . Intel_Df::t('You can view events in near realtime in GA reports.');
  $output .= '<br><br><label>' . Intel_Df::t('Launch the Google Analytics realtime events report:') . '</label>';
  $l_options = Intel_Df::l_options_add_target('ga');
  $l_options = Intel_Df::l_options_add_class('btn btn-info', $l_options);
  $url = "https://analytics.google.com/analytics/web/#realtime/rt-event/" . intel_get_ga_profile_slug() . "/%3Fmetric.type%3D5/";
  $output .= '<br>' . Intel_Df::l( Intel_Df::t('Google Analytics realtime events report'), $url, $l_options);
  $output .= '</p>';

  $output .= '<h3>' . Intel_Df::t('Pre-defined events') . '</h3>';
  $output .= '<p>';
  $output .= Intel_Df::t('Intelligence provides several pre-defined events that automatically start tracking after installation.');
  $output .= ' ' . Intel_Df::t('You may already see some of these events in the realtime events report.');
  $output .= ' ' . Intel_Df::t('Lets look at how these work.');
  $output .= '</p>';

  $output .= '<div class="alert alert-info">';
  $output .= '<label>' . Intel_Df::t('Try it: triggering link events') . '</label>';
  $items = array();
  $l_options = Intel_Df::l_options_add_target('wp_admin');
  $items[] = Intel_Df::t('Add a new page to your site to use as a sandbox for this tutorial by going to !link', array(
    '!link' => Intel_Df::l( Intel_Df::t('Admin > Pages > Add new'), 'wp-admin/post-new.php?post_type=page', $l_options)
  ));
  $items[] = Intel_Df::t('Set the page title to "Intelligence Sandbox" and the Permalink slug to "intelligence-sandbox".');
  $input = '<textarea rows="6" style="width: 100%">';
  $l_options = Intel_Df::l_options_add_target('_blank');
  $input  .= "<strong>" . Intel_Df::t('File download link') . ":</strong>\n";
  $input  .= Intel_Df::l( Intel_Df::t('Download now'), '/wp-content/plugins/intelligence/images/setup_intel_action.png', $l_options);
  $input  .= "\n\n<strong>" . Intel_Df::t('External link') . ":</strong>\n";
  $input  .= Intel_Df::l( Intel_Df::t('Visit Intelligence for WordPress'), 'http://intelligencewp.com', $l_options);
  $input  .= "\n\n<strong>" . Intel_Df::t('Email link') . ":</strong>\n";
  $input  .= Intel_Df::l( Intel_Df::t('Email us at info@example.com'), 'mailto:info@example.com', $l_options);
  $input  .= "\n\n<strong>" . Intel_Df::t('Telephone link') . ":</strong>\n";
  $input  .= Intel_Df::l( Intel_Df::t('Call Us at 214-555-1212'), 'tel:214-555-1212', $l_options);
  $input  .= '</textarea>';
  $items[] = Intel_Df::t('Copy and past the following content into the page content:') . $input;
  $items[] = Intel_Df::t('View the page and click on the links. Notice the events comming through in the GA realtime events report.');

  $output .= Intel_Df::theme('item_list', array('type' => 'ol', 'items' => $items));
  $output .= '</div>';

  $output .= '<p>';
  $output .= Intel_Df::t('The pre-defined events are provided by Intelligence add-ons and scripts.');
  $output .= ' ' . Intel_Df::t('Developers can add their own in plugins and themes using the Intelligence event API.');
  $output .= ' ' . Intel_Df::t('The events triggered in the above exercise were provided by the LinkTracker script.');
  $output .= ' ' . Intel_Df::t('You can enable more pre-defined events by installing add-ons or enabling scripts.');
  $l_options = Intel_Df::l_options_add_target('wp_admin_intel_events');
  $output .= ' ' . Intel_Df::t('You can view the available events in the Intelligence events admin at !link.', array(
    '!link' => Intel_Df::l( Intel_Df::t('Admin > Intelligence > Settings > Events'), 'admin/config/intel/settings/intel_event', $l_options)
  ));
  $output .= '</p>';

  $output .= '<h3>' . Intel_Df::t('Custom events') . '</h3>';
  $output .= '<p>';
  $output .= Intel_Df::t('If your not a developer, or just prefer to use the admin, you can create custom Intelligence events using WordPress\' admin.');
  $output .= '</p>';

  $output .= '<div class="alert alert-info">';
  $output .= '<label>' . Intel_Df::t('Try it: create a custom pageview event') . '</label><br>';
  $output .= Intel_Df::t('In this exercise we will trigger an event when someone views the Intelligence sandbox page.');
  $items = array();
  $l_options = Intel_Df::l_options_add_target('wp_admin');
  $items[] = Intel_Df::t('Add a new Intelligence event using by going to !link.', array(
    '!link' => Intel_Df::l( Intel_Df::t('Admin > Intelligence > Settings > Events > Add new'), 'admin/config/intel/settings/intel_event/add', $l_options)
  ));
  $l_options = Intel_Df::l_options_add_query(array(
    'title' => Intel_Df::t('Tutorial sandbox view'),
    'key' => Intel_Df::t('tutorial_sandbox_view'),
    'enable_pages' => Intel_Df::t('intelligence-sandbox'),
  ), $l_options);
  $item2 = array(
    'data' => Intel_Df::t('Input the following into the fields on the Add event form or !link:', array(
      '!link' => Intel_Df::l( Intel_Df::t('autofill the form fields'), 'admin/config/intel/settings/intel_event/add', $l_options)
    )),
    'children' => array(
      '<label>' . Intel_Df::t('General > Title') . ':</label> <pre>' . Intel_Df::t('Tutorial sandbox view') . '</pre>',
      '<label>' . Intel_Df::t('General > Id') . ':</label> <pre>' . Intel_Df::t('tutorial_sandbox_view') . '</pre>',
      '<label>' . Intel_Df::t('Availability > Page list') . ':</label> <pre>' . Intel_Df::t('intelligence-sandbox') . '</pre>',
    ),
  );
  $items[] = $item2;
  $items[] = Intel_Df::t('Click the "Save event" button at the bottom of the form to create your event.');
  $l_options = Intel_Df::l_options_add_target('intel_sb_page');
  $items[] = Intel_Df::t('Trigger the event by going to the !link.', array(
    '!link' => Intel_Df::l( Intel_Df::t('Intelligence sandbox page'), 'intelligence-sandbox', $l_options)
  ));
  $output .= Intel_Df::theme('item_list', array('type' => 'ol', 'items' => $items));
  $output .= '</div>';

  $output .= '<p>';
  $output .= Intel_Df::t('You just created an event that was only enabled on the sandbox page.');
  $output .= ' ' . Intel_Df::t('This style of event is useful to track when visitors hit an important page on your site.');
  $output .= ' ' . Intel_Df::t('You can use the availablity settings to enable an event on a single page, multiple pages or the entire website.');
  $output .= '</p>';

  $output .= '<p>';
  $output .= ' ' . Intel_Df::t('Often times you will want to track when a visitor preforms a specific interaction with a specific element of a page like the link events at the start of the tutorial.');
  $output .= ' ' . Intel_Df::t('You can create events that monitor specific page elements then trigger when a particular event happens like clicking a link or mousing over an image.');
  $output .= ' ' . Intel_Df::t('To setup this style of event we simply add trigger fields to the event admin form.');
  $output .= ' ' . Intel_Df::t('This will cause the event to not trigger automatically when the page loads, but instead to bind to a select page element and wait for the designated trigger event.');
  $output .= '</p>';

  $output .= '<div class="alert alert-info">';
  $output .= '<label>' . Intel_Df::t('Try it: create a custom page interaction event') . '</label><br>';
  $items = array();
  $l_options = Intel_Df::l_options_add_target('wp_admin');
  $items[] = Intel_Df::t('Add a new Intelligence event using by going to !link.', array(
    '!link' => Intel_Df::l( Intel_Df::t('Admin > Intelligence > Settings > Events > Add new'), 'admin/config/intel/settings/intel_event/add', $l_options)
  ));
  $l_options = Intel_Df::l_options_add_query(array(
    'title' => Intel_Df::t('Tutorial link click'),
    'key' => Intel_Df::t('tutorial_link_click'),
    'selector' => Intel_Df::t('a.track-tutorial-link'),
    'on_event' => Intel_Df::t('click'),
    'enable_pages' => Intel_Df::t('intelligence-sandbox'),
  ), $l_options);
  $item2 = array(
    'data' => Intel_Df::t('Input the following into the fields on the Add event form or !link:', array(
      '!link' => Intel_Df::l( Intel_Df::t('autofill the form fields'), 'admin/config/intel/settings/intel_event/add', $l_options)
    )),
    'children' => array(
      '<label>' . Intel_Df::t('General > Title') . ':</label> <pre>' . Intel_Df::t('Tutorial link click') . '</pre>',
      '<label>' . Intel_Df::t('General > Id') . ':</label> <pre>' . Intel_Df::t('tutorial_link_click') . '</pre>',
      '<label>' . Intel_Df::t('Event trigger > Selector') . ':</label> <pre>' . Intel_Df::t('a.track-tutorial-link') . '</pre>',
      '<label>' . Intel_Df::t('Event trigger > On event') . ':</label> <pre>' . Intel_Df::t('click') . '</pre>',
      '<label>' . Intel_Df::t('Availability > Page list') . ':</label> <pre>' . Intel_Df::t('intelligence-sandbox') . '</pre>',
    ),
  );
  $items[] = $item2;
  $items[] = Intel_Df::t('Click the "Save event" button at the bottom of the form to create your event.');
  $l_options = Intel_Df::l_options_add_target('intel_sb_page');
  $items[] = Intel_Df::t('Trigger the event by going to the !link.', array(
    '!link' => Intel_Df::l( Intel_Df::t('Intelligence sandbox page'), 'intelligence-sandbox', $l_options)
  ));
  $output .= Intel_Df::theme('item_list', array('type' => 'ol', 'items' => $items));
  $output .= '</div>';


  $output .= '</div></div>';
  $output .= '</article>';

  return $output;
}