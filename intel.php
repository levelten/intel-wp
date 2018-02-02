<?php

/**
 * Intelligence bootstrap file
 *
 * This file is read by WordPress to generate the plugin information in the plugin
 * admin area. This file also includes all of the dependencies used by the plugin,
 * registers the activation and deactivation functions, and defines a function
 * that starts the plugin.
 *
 * @link              getlevelten.com/blog/tom
 * @since             1.0.0
 * @package           Intelligence
 *
 * @wordpress-plugin
 * Plugin Name:       Intelligence
 * Plugin URI:        http://intelligencewp.com
 * Description:       Provides behavior and visitor intelligence.
 * Version:           1.2.7.0-dev
 * Minimum PHP:       5.3
 * Author:            LevelTen
 * Author URI:        http://getlevelten.com
 * License:           GPL-2.0+
 * License URI:       http://www.gnu.org/licenses/gpl-2.0.txt
 * Text Domain:       intel
 * Domain Path:       /languages
 * GitHub Plugin URI: https://github.com/levelten/wp-intelligence
 */


// If this file is called directly, abort.
if ( ! defined( 'WPINC' ) ) {
	die;
}

define('INTEL_VER', '1.2.7.0-dev');

/*******************************************
/* Begin Freemius setup
 */
if (0) {
// Create a helper function for easy SDK access.
	function intel_fs() {
		global $intel_fs;

		if ( ! isset( $intel_fs ) ) {
			// Include Freemius SDK.
			require_once dirname(__FILE__) . '/freemius/start.php';

			$intel_fs = fs_dynamic_init( array(
				'id'                  => '1675',
				'slug'                => 'intelligence',
				'type'                => 'plugin',
				'public_key'          => 'pk_cd3b6d95db54c50e50ccbf77112de',
				'is_premium'          => false,
				'has_addons'          => false,
				'has_paid_plans'      => false,
				'menu'                => array(
					'slug'           => 'intel_admin',
					'first-path'     => 'admin.php?page=intel_config&q=admin/config/intel/settings/setup',
					'account'        => false,
					'support'        => false,
				),
			) );
		}

		return $intel_fs;
	}

// Init Freemius.
	intel_fs();

// Signal that parent SDK was initiated.
	do_action( '_loaded' );

// Signal that SDK was initiated.
	do_action( 'intel_fs_loaded' );
}

/*******************************************
/* End Freemius setup
 */

/**
 * The code that runs during plugin activation.
 * This action is documented in includes/class-intel-activator.php
 */
function activate_intel() {
	require_once plugin_dir_path( __FILE__ ) . 'includes/class-intel-activator.php';
	Intel_Activator::activate();
	intel_activate_plugin('intel');
}
register_activation_hook( __FILE__, 'activate_intel' );

/**
 * The code that runs during plugin deactivation.
 * This action is documented in includes/class-intel-deactivator.php
 */
function deactivate_intel() {
	require_once plugin_dir_path( __FILE__ ) . 'includes/class-intel-deactivator.php';
	Intel_Deactivator::deactivate();
}
register_deactivation_hook( __FILE__, 'deactivate_intel' );

function _intel_uninstall() {
	require_once plugin_dir_path( __FILE__ ) . 'intel.install';
	intel_uninstall();
}
register_uninstall_hook( __FILE__, '_intel_uninstall' );





//register_uninstall_hook('uninstall.php', 'intel_uninstall');

/**
 * required shims
 */
require plugin_dir_path( __FILE__ ) . 'includes/class-intel-df.php';
function intel_df() {
	return Intel_Df::getInstance();
}

/**
 * The core plugin class that is used to define internationalization,
 * admin-specific hooks, and public-facing site hooks.
 */
require plugin_dir_path( __FILE__ ) . 'includes/class-intel.php';

/**
 * Begins execution of the plugin.
 *
 * Since everything within the plugin is registered via hooks,
 * then kicking off the plugin from this point in the file does
 * not affect the page life cycle.
 *
 * @since    1.0.0
 */
function intel() {
	return Intel::getInstance();
}

/*
 * Start GADWP
 */
global $intel;
$intel = intel();