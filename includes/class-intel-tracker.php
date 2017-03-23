<?php

/**
 * The file that defines the core plugin class
 *
 * A class definition that includes attributes and functions used across both the
 * public-facing side of the site and the admin area.
 *
 * @link       getlevelten.com/blog/tom
 * @since      1.0.0
 *
 * @package    Intl
 * @subpackage Intl/includes
 */

/**
 * The core plugin class.
 *
 * This is used to define internationalization, admin-specific hooks, and
 * public-facing site hooks.
 *
 * Also maintains the unique identifier of this plugin as well as the current
 * version of the plugin.
 *
 * @since      1.0.0
 * @package    Intl
 * @subpackage Intl/includes
 * @author     Tom McCracken <tomm@getlevelten.com>
 */
class Intel_Tracker {

	private static $instance;

	protected $config;

	protected $pushes;

	/**
	 * Define the core functionality of the plugin.
	 *
	 * Set the plugin name and the plugin version that can be used throughout the plugin.
	 * Load the dependencies, define the locale, and set the hooks for the admin area and
	 * the public-facing side of the site.
	 *
	 * @since    1.0.0
	 */
	private function __construct() {

		// TODO change this
		$api_level = 'pro';

		$a = explode('//', WP_SITEURL);
		$systemHost = $a[1];
		$cmsHostpath = $a[1] . '/';
		//$pageTitle = is_admin() ? '' : wp_title('', 0);
		$pageTitle = '';

		$this->config = array(
			'debug' => 0,
			// cmsHostpath, modulePath & apiPath are not standard io settings. They are used
			// exclusivly by intel module js.
			'cmsHostpath' => $cmsHostpath,
			'modulePath' => "wp-content/plugins/intl/",
			'libPath' => 'TODO',
			'systemPath' => 'TODO',
			'systemHost' => $systemHost,
			'systemBasepath' => 'TODO',
			'srl' => 'TODO',
			'pageTitle' => $pageTitle,
			'trackAnalytics' => 1, // this is set in intel_js_alter if ga script exists
			'trackAdhocCtas' => ($api_level == 'pro') ? 'track-cta' : '',
			'trackAdhocEvents' => 'track-event',
			'trackForms' => array(),
			'trackRealtime' => 0,
			'isLandingpage' => 0,
			'scorings' => array(),
			//'scorings' => intel_get_scorings('js_setting'), //TODO
			'storage' => array(
				'page' => array(
					'analytics' => array(),
				),
				'session' => array(
					'analytics' => array(),
				),
				'visitor' => array(
					'analytics' => array(),
				),
			),
		);

		$this->pushes = array();
	}

	public static function getInstance() {
		if (empty(self::$instance)) {
			self::$instance = new Intel_Tracker();
		}
		return self::$instance;
	}

	public function get_intel_pushes() {
		return intel_get_flush_page_intel_pushes();
	}

	public function get_pushes_script() {
	  $pushes = self::get_intel_pushes();
		$out = '<script>' . "\n";
		foreach ($pushes as $key => $value) {
			$out .= "  io('$key', " . json_encode($value) . ");\n";
		}
		$out .= '</script>' . "\n";
		return $out;
  }

	public function setConfig($prop, $value) {
		$this->config[$prop] = $value;
	}

	public function getConfig($prop = '', $default = null) {
		if (!$prop) {
			return $this->config;
		}
		else if (exists($this->config[$prop])) {
			return $this->config[$prop];
		}
		else {
			return $default;
		}
	}


	public function addPush($push, $index = '') {
		if (!empty($push['method'])) {
			$method = $push['method'];
			unset($push['method']);
		}
		else {
			$method = array_shift($push);
		}

		if ($method == 'set') {
			$index = $push[0];
			$value = $push[1];
		}
		else if ($method == 'event') {
			$index = count($this->pushes[$method]);
			$value = $push[0];
		}
		else {
			$index = count($this->pushes[$method]);
			$value = $push;
		}

		$this->pushes[$method][$index] = $value;
	}

	public function getPushes($flush = false) {
		$ret = $this->pushes;
		if ($flush) {
			$this->pushes = array();
		}
		return $ret;
	}

	/**
	 * Generates tracking code
	 */
	public function tracking_code() {
		require_once INTEL_DIR . 'includes/intel.page_alter.inc';
		$page = array();
		intel_page_alter($page);

		$js_settings = intel()->get_js_settings();

		if (intel_is_debug()) {
			intel_d($js_settings);
		}

		$io_name = 'io';

		$script = "var intel_settings = " . json_encode($js_settings) . ";\n";
		$script .= intel_get_js_embed('l10i', 'local');
		$script .= "$io_name('set', 'config', intel_settings.intel.config);\n";
		if (isset($js_settings['intel']['pushes']) && is_array($js_settings['intel']['pushes'])) {
			foreach ($js_settings['intel']['pushes'] as $cm => $push) {
				if ($cm == 'setUserId') {
					$script .= $io_name . '("' . $cm . '","' . $push[0][0];
					if (!empty($push[0][1])) {
						$script .= '","' . $push[0][1];
					}
					$script .= '");' . "\n";
				} else {
					$script .= "$io_name('$cm', intel_settings.intel.pushes.$cm);\n";
				}
			}
		}


		//$script .= "$io_name('set', intel_settings.intel.pushes.set);\n";
		//if (!empty($js_settings['intel']['pushes']['events'])) {
		//	$script .= "$io_name('event', intel_settings.intel.pushes.event);\n";
		//}

		print '<script>' . $script . '</script>';
		return;

		/*
		// mimics logic in intel_js_alter()
		$intel_js_settings = intel()->get_js_settings();
		$


		$io_name = 'io';
		$pushstr = $io_name . '("set", "config", );' . "\n";
		//$str = '_l10iq.push(["set", "config", ' . drupal_json_encode($options['config']) . ']);' . "\n";
		if (isset($options['pushes']) && is_array($options['pushes'])) {
			foreach ($options['pushes'] as $cm => $push) {
				if ($cm == 'setUserId') {
					$pushstr .= $io_name . '("' . $cm . '","' . $push[0][0];
					if (!empty($push[0][1])) {
						$pushstr .= '","' . $push[0][1];
					}
					$pushstr .= '");' . "\n";
				} else {
					$pushstr .= $io_name . '("' . $cm . '",' . drupal_json_encode($push) . ');' . "\n";
				}
				//$str .= '_l10iq.push(["' . $cm . '",' . drupal_json_encode($push) . ']);' . "\n";
			}
		}

		$io_name = 'io';
		print '<script>';
    print "var intel_settings = " . json_encode(intel()->get_js_settings()) . ";\n";
		print "$io_name('set', 'config', intel_settings.intel.config);\n";
		print '</script>';
		return;

		$ga_tid = get_option( 'intel_ga_tid' );
		$l10iapi_url = get_option('intel_l10iapi_url', '');
		if (empty($l10iapi_url)) {
			$l10iapi_url = INTEL_L10IAPI_URL;
		}
		$l10iapi_js_ver = get_option('intel_l10iapi_js_ver', '');
		if (empty($l10iapi_js_ver)) {
			$l10iapi_js_ver = INTEL_L10IAPI_JS_VER;
		}
		$debug_mode = get_option('intel_debug_mode', 0);

		$l10iapi_js_file = $l10iapi_js_ver . '/' . (($debug_mode) ? 'l10i.js' : 'l10i.min.js');

		require_once plugin_dir_path( dirname( __FILE__ ) ) . 'public/partials/intel-tracking-code.php';
		*/
	}

}
