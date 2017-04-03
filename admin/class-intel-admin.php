<?php

/**
 * The admin-specific functionality of the plugin.
 *
 * @link       getlevelten.com/blog/tom
 * @since      1.0.0
 *
 * @package    Intel
 * @subpackage Intel/admin
 */

/**
 * The admin-specific functionality of the plugin.
 *
 * Defines the plugin name, version, and two examples hooks for how to
 * enqueue the admin-specific stylesheet and JavaScript.
 *
 * @package    Intel
 * @subpackage Intel/admin
 * @author     Tom McCracken <tomm@getlevelten.com>
 */
class Intel_Admin {

	/**
	 * The ID of this plugin.
	 *
	 * @since    1.0.0
	 * @access   private
	 * @var      string    $plugin_name    The ID of this plugin.
	 */
	private $plugin_name;

	/**
	 * The version of this plugin.
	 *
	 * @since    1.0.0
	 * @access   private
	 * @var      string    $version    The current version of this plugin.
	 */
	private $version;

	/**
	 * Holds query string
	 * @var
	 */
	public $q = '';

	public $args = array();

	/**
	 * Initialize the class and set its properties.
	 *
	 * @since    1.0.0
	 * @param      string    $plugin_name       The name of this plugin.
	 * @param      string    $version    The version of this plugin.
	 */
	public function __construct( $plugin_name, $version ) {

		$this->plugin_name = $plugin_name;
		$this->version = $version;

	}

	/**
	 * Register the stylesheets for the admin area.
	 *
	 * @since    1.0.0
	 */
	public function enqueue_styles() {

		/**
		 * This function is provided for demonstration purposes only.
		 *
		 * An instance of this class should be passed to the run() function
		 * defined in Intel_Loader as all of the hooks are defined
		 * in that particular class.
		 *
		 * The Intel_Loader will then create the relationship
		 * between the defined hooks and the functions defined in this
		 * class.
		 */

		wp_enqueue_style( $this->plugin_name, plugin_dir_url( __FILE__ ) . 'css/intel-admin.css', array(), $this->version, 'all' );

	}

	/**
	 * Register the JavaScript for the admin area.
	 *
	 * @since    1.0.0
	 */
	public function enqueue_scripts() {

		/**
		 * This function is provided for demonstration purposes only.
		 *
		 * An instance of this class should be passed to the run() function
		 * defined in Intel_Loader as all of the hooks are defined
		 * in that particular class.
		 *
		 * The Intel_Loader will then create the relationship
		 * between the defined hooks and the functions defined in this
		 * class.
		 */

		wp_enqueue_script( $this->plugin_name, INTEL_URL . 'admin/js/intel-admin.js', array( 'jquery' ), $this->version, false );

		wp_enqueue_script('intel_admin_js_bootstrap_hack', INTEL_URL . 'admin/js/intel-bootstrap-hack.js', false, $this->version, false);

		wp_enqueue_script('intel_admin_bootstrap', INTEL_URL . 'vendor/bootstrap/js/bootstrap.min.js', false, $this->version, false);


		$data = array();
		$data['intel_dir'] = INTEL_DIR;
		$data['intel_url'] = INTEL_URL;
		$data['intel_file'] = INTEL_FILE;
		wp_localize_script('intel_admin_js_bootstrap_hack', 'intel_admin_settings', $data);
	}

	// buffer page output incase we need to do a redirect
	public static function ob_callback($buffer) {
		return $buffer;
	}

	public function ob_start() {
		ob_start("Intel_Admin::ob_callback");
	}

	function ob_end(){
		ob_end_flush();
	}

	public function session_start() {
		// need to start session for messages to queue across pages
		if(!session_id()) {
			session_start();
		}
	}

	public function session_end() {
		session_destroy();
  }

	public function site_menu() {
		global $wp_version;
		if ( current_user_can( 'manage_options' ) ) {
			add_menu_page( __( "Intelligence", 'intel' ), __( "Intelligence", 'intel' ), 'manage_options', 'intel_reports', array( $this, 'menu_router' ), version_compare( $wp_version, '3.8.0', '>=' ) ? 'dashicons-analytics' : GADWP_URL . 'admin/images/gadash-icon.png' );
			add_submenu_page( 'intel_reports', __( "Reports", 'intel' ), __( "Reports", 'intel' ), 'manage_options', 'intel_reports', array( $this, 'menu_router' ) );
			add_submenu_page( 'intel_reports', __( "Contacts", 'intel' ), __( "Contacts", 'intel' ), 'manage_options', 'intel_visitor', array( $this, 'menu_router' ) );
			add_submenu_page( 'intel_reports', __( "Settings", 'intel' ), __( "Settings", 'intel' ), 'manage_options', 'intel_config', array( $this, 'menu_router' ) );
			//add_submenu_page( 'intel_settings', __( "General Settings", 'intel' ), __( "Settings", 'intel' ), 'manage_options', 'intel_settings', array( 'Intel_Settings', 'general_settings' ) );
			//if (intel_is_debug()) {
				add_submenu_page( 'intel_reports', __( "Utilities", 'intel' ), __( "Utilities", 'intel' ), 'manage_options', 'intel_util', array( $this, 'menu_router' ) );
			//}
  	}
	}

	public function menu_router() {
		$intel = intel();
		$menu_info = $intel->menu_info();

		//ksort($menu_info);
		//d($menu_info);

		$info = array();
		$tree = array();
		$breadcrumbs = array();
		$breadcrumbs[] = array(
			'text' => __('Intelligence', 'intel'),
			//'path' => Intel_Df::url('admin/intel'),
		);
		$navbar_exclude = array();

		$q = '';
		if ($_GET['page'] == 'intel') {
			$q = 'admin/dashboard';
		}
		if ($_GET['page'] == 'intel_visitor') {
			$q = 'admin/people/contacts';
			$breadcrumbs[] = array(
				'text' => __('Contacts', 'intel'),
				'path' => Intel_Df::url($q),
			);
		}
		if ($_GET['page'] == 'intel_reports') {
			$q = 'admin/reports/intel';
			//$navbar_exclude[$q] = 1;
			$breadcrumbs[] = array(
				'text' => __('Reports', 'intel'),
				'path' => Intel_Df::url($q),
			);
			$navbar_base_q = $navbar_base_qt = $q;
		}

		if ($_GET['page'] == 'intel_config') {
			$q = 'admin/config/intel/settings';
			$breadcrumbs[] = array(
				'text' => __('Settings', 'intel'),
				'path' => Intel_Df::url($q),
			);
		}
		if ($_GET['page'] == 'intel_util') {
			$q = 'admin/util';
			$breadcrumbs[] = array(
				'text' => __('Utilities', 'intel'),
				'path' => Intel_Df::url($q),
			);
			$navbar_base_q = $navbar_base_qt = $q;
		}
		if (isset($_GET['q'])) {
			$q = $_GET['q'];
		}
		else {
			$_GET['q'] = &$q;
		}
		$intel->q = $q;

		// set translated path
		$qt = $q;

		$entities = array(
			'submission',
			'visitor',
		);
		$this->args = $path_args = explode('/', $q);

		$path_args_t = array();


		if (empty($info)) {
			if (!empty($menu_info[$q])) {
				$info = $menu_info[$q];
			}
			else {
				if (in_array($path_args[0], $entities)) {
					$a = $path_args;
					$a[1] = '%intel_' . $path_args[0];
					$qt = implode('/', $a);
					if (!empty($menu_info[$qt])) {
						$info = $menu_info[$qt];
						if (!empty($path_args[1])) {
							$entity_type = substr($a[1], 1);
							$entity = $intel->get_entity_controller($entity_type)->loadOne($path_args[1]);
							if (empty($entity)) {
								$vars = array(
									'title' => __('404 Error', 'intel'),
									'markup' => __('Entity not found', 'intel'),
								);
								print Intel_Df::theme('intel_page', $vars);
								return;
							}
							$path_args_t[1] = $entity;
						}
						if ($path_args[0] == 'visitor') {
							$breadcrumbs[] = array(
								'text' => $entity->label(),
								'path' => Intel_Df::url($entity->uri()),
							);
						}
					}
				}
			}
		}

		if (empty($info)) {
			$vars = array(
				'title' => __('404 Error', 'intel'),
				'markup' => __('Page not found', 'intel'),
			);
			print Intel_Df::theme('intel_page', $vars);
			return;
		}

		/*
		$a = explode('/', $qt);
		$qt_arg_count = count($a);
		$qt_len = strlen($qt);
		foreach ($menu_info as $k => $mi) {
			if (isset($mi['type']) && ($mi['type'] == Intel_Df::MENU_DEFAULT_LOCAL_TASK) && substr($k, 0, $qt_len) == $qt) {
				$a = explode('/', $k);
				$a_cnt = count($a);
				if ($a_cnt == ($qt_arg_count + 1)) {
					$info = $mi + $info;
					$breadcrumbs[] = array(
						'text' => Intel_Df::t($info['title']),
						'path' => Intel_Df::url($q),
					);
					$q .= '/' . $a[$a_cnt - 1];
					$qt .= '/' . $a[$a_cnt - 1];

					break;
				}
			}
		}
		d($q);
		d($qt);
		*/

		$a = explode('/', $q);
		$q_arg_count = count($a);
		array_pop($a);
		$parent_q = implode('/', $a);

		$a = explode('/', $qt);
		array_pop($a);
		$parent_qt = implode('/', $a);

		$defs = array(
			'type' => Intel_Df::MENU_CALLBACK,
		);
		$info += $defs;

		if ($info['type'] & Intel_Df::MENU_LINKS_TO_PARENT ) {
			if (isset($menu_info[$parent_qt])) {
				$info += $menu_info[$parent_qt];
			}
		}

		// check permissions
		$func = !empty($info['access callback']) ? $info['access callback'] : 'user_access';
		if ($func == 'user_access') {
			$func = 'Intel_Df::' . $func;
		}
		$args = !empty($info['access arguments']) ? $info['access arguments'] : array();
		if (!call_user_func_array($func, $args)) {
			$vars = array(
				'title' => __('401 Error', 'intel'),
				'markup' => __('Not authorized', 'intel'),
			);
			//print Intel_Df::theme('intel_page', $vars);
			//return;
		}

		// process page arguments
		$page_args = !empty($info['page arguments']) ? $info['page arguments'] : array();

		foreach ($page_args as $k => $arg) {
			if (is_integer($arg)) {
				$page_args[$k] = !empty($path_args_t[$arg]) ? $path_args_t[$arg] : $path_args[$arg];
			}
		}

		// TODO WP handle permissions

		// set page title using menu info
		if (!empty($info['title'])) {
			$title = __($info['title'], 'intel');
			$intel->set_page_title($title);
		}

		if (!empty($info['file'])) {
			$fp = INTEL_DIR;
			$fn = $fp . $info['file'];
			include_once $fn;
		}

		$page_func = $info['page callback'];
		if ($page_func == 'drupal_get_form') {
			include_once ( INTEL_DIR . 'includes/class-intel-form.php' );
			$page_func = 'Intel_Form::drupal_get_form';
		}
		$vars['markup'] = call_user_func_array($page_func, $page_args);

		$base_q = $q;
		$base_qt = $qt;
		$menu_actions = array();
		if ($info['type'] & Intel_Df::MENU_IS_LOCAL_TASK ) {
			$base_q = $parent_q;
			$base_qt = $parent_qt;
		}
		if (empty($tree)) {
			$tree = self::build_submenu_tree(isset($navbar_base_qt) ? $navbar_base_qt : $base_qt, $q, $menu_info, $navbar_exclude, $menu_actions, $breadcrumbs);
		}

		$nb_vars = array(
			'brand' => 'Intelligence',
			'base_path' => isset($navbar_base_q) ? $navbar_base_q : $base_q,
			'tree' => $tree,
			'tree2' => $menu_actions,
		);
		$vars['navbar'] = Intel_Df::theme('intel_navbar', $nb_vars);

		$vars['breadcrumbs'] = Intel_Df::theme('intel_breadcrumbs', array('breadcrumbs' => $breadcrumbs));

		$vars['messages'] = Intel_Df::drupal_get_messages();

		print Intel_Df::theme('intel_page', $vars);
	}

	public function build_submenu_tree($base_qt, $q, $menu_info, $exclude = array(), &$actions = array(), &$breadcrumbs = array()) {
		$tree = array();
		$actions = array();
		$qt_len = strlen($base_qt);
		$a = explode('/', $base_qt);
		$b = explode('/', $q);
		$q_end_arr = array_slice($b, count($a));
		$q_end = implode('/', $q_end_arr);
		$bc_add = array();

		foreach ($menu_info as $k => $info) {
			if (!empty($exclude[$k])) {
				continue;
			}

			if (isset($info['type']) && ($info['type'] & Intel_Df::MENU_LOCAL_TASK)  && (substr($k, 0, $qt_len) == $base_qt) && ($k != $base_qt)) {

				// if default local task, get info from parent;
				if ($info['type'] & Intel_Df::MENU_DEFAULT_LOCAL_TASK) {
					$a = explode('/', $k);
					array_pop($a);
					$parent_k = implode('/', $a);
					if (!empty($menu_info[$parent_k])) {
						$info += $menu_info[$parent_k];
					}
				}

				// check permissions
				$func = !empty($info['access callback']) ? $info['access callback'] : 'user_access';
				if ($func == 'user_access') {
					$func = 'Intel_Df::' . $func;
				}
				$args = !empty($info['access arguments']) ? $info['access arguments'] : array();
				if (!call_user_func_array($func, $args)) {
					continue;
				}
				//d($k);

				// get elements after $qt;
				$defs = array(
					'type' => Intel_Df::MENU_CALLBACK,
				);
				$info += $defs;

				$qt_end = substr($k, $qt_len + 1);
				$qt_end_arr = explode('/', $qt_end);

				if (count($qt_end_arr) == 1) {
					if (!empty($q_end_arr[0]) && $qt_end_arr[0] == $q_end_arr[0]
						|| (empty($q_end_arr[0]) && ($info['type'] & Intel_Df::MENU_LINKS_TO_PARENT) )
					) {

						$info['active'] = 1;
						$bc_add[0] = array(
							'text' => $info['title'],
							'path' => Intel_Df::url($q),
						);
					}
					if ($info['type'] & Intel_Df::MENU_IS_LOCAL_ACTION) {
						$actions[$qt_end_arr[0]] = array(
							'#info' => $info,
						);
					}
					else {
						$tree[$qt_end_arr[0]] = array(
							'#info' => $info,
						);
					}
				}
				else if (count($qt_end_arr) == 2) {
					if (!isset($tree[$qt_end_arr[0]])) {
						$tree[$qt_end_arr[0]] = array();
					}
					if (
						!empty($q_end_arr[0]) && $qt_end_arr[0] == $q_end_arr[0]
						&& !empty($q_end_arr[1]) && $qt_end_arr[1] == $q_end_arr[1]
					) {
						$info['active'] = 1;
						$bc_add[1] = array(
							'text' => $info['title'],
							'path' => Intel_Df::url($q),
						);
					}
					$tree[$qt_end_arr[0]][$qt_end_arr[1]] = array(
						'#info' => $info,
					);
				}
			}
		}
		$breadcrumbs = array_merge ( $breadcrumbs, $bc_add );
		return $tree;
	}

	public function admin_bar_menu($wp_admin_bar) {
		$args = array(
			'id'    => 'my_page',
			'title' => 'My Page',
			'href'  => 'http://mysite.com/my-page/',
			'meta'  => array( 'class' => 'my-toolbar-page' )
		);
		$wp_admin_bar->add_node( $args );
	}

	public function contacts_column_headers() {
		$ch = array(
			'cb' => '<input type="checkbox" />',
			'name' => __('Name', 'intel'),
			'email' => __('Email', 'intel'),
			'contact_created' => __('Created', 'intel'),
			'last_activity' => __('Last activity', 'intel'),
			'score' => __('Score', 'intel'),
			//'entrances' => __('Visits', 'intel'),
			//'pageviews' => __('Pageviews', 'intel'),
			//'timeOnSite' => __('Time on site', 'intel'),
		);
		return $ch;
	}

	public function plugin_action_links($links) {
		$l = array();
		$l[] = Intel_Df::l(Intel_Df::t('Settings'), 'admin/config/intel/settings');
		$links = $l + $links;
		return $links;
	}

	public function admin_setup_notice() {
		$api_level = intel_api_level();
  	if (!empty($api_level)) {
			return;
		}
		// Don't show the connect notice anywhere but the plugins.php after activating
		$current = get_current_screen();
		if ( 'plugins' !== $current->parent_base ) {
			return;
		}

		$this->enqueue_scripts();
		$this->enqueue_styles();
		?>
		<div id="message" class="bootstrap-wrapper wrap">
			<div class="panel panel-info m-t-1">
				<h2 class="panel-heading m-t-0"><?php _e( 'Get Intelligence!', 'intel' ); ?></h2>
				<div class="panel-body">
					<p><?php _e( 'To complete the installation of Intelligence launch the setup wizard using the button below.', 'intel' ); ?></p>
					<p>
						<a href="<?php print Intel_Df::url('admin/config/intel/settings/setup'); ?>" class="btn btn-info">Launch Setup Wizard</a>
					</p>
				</div>
			</div>
		</div>

		<?php
	}

}
