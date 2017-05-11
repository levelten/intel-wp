<?php

/**
 * Fired during plugin activation
 *
 * @link       getlevelten.com/blog/tom
 * @since      1.0.0
 *
 * @package    Intel
 * @subpackage Intel/includes
 */

/**
 * Fired during plugin activation.
 *
 * This class defines all code necessary to run during the plugin's activation.
 *
 * @since      1.0.0
 * @package    Intel
 * @subpackage Intel/includes
 * @author     Tom McCracken <tomm@getlevelten.com>
 */
class Intel_Activator {

	/**
	 * Short Description. (use period)
	 *
	 * Long Description.
	 *
	 * @since    1.0.0
	 */
	public static function activate() {
		self::setup_database();
		self::setup_cron();
	}

	public static function setup_database() {
		global $wpdb;

		require_once( ABSPATH . 'wp-admin/includes/upgrade.php' );

		$charset_collate = $wpdb->get_charset_collate();

		// create intl_visitor table
		$table_name = $wpdb->prefix . "intel_visitor";

		$sql = "CREATE TABLE $table_name (
		  vid int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
			created int(10) UNSIGNED NOT NULL DEFAULT '0',
			updated int(10) UNSIGNED NOT NULL DEFAULT '0',
			last_activity int(10) UNSIGNED NOT NULL DEFAULT '0',
			name varchar(128) NOT NULL,
			contact_created int(10) UNSIGNED NOT NULL DEFAULT '0',
			data_updated int(10) UNSIGNED NOT NULL DEFAULT '0',
			data longtext NOT NULL,
			ext_updated int(10) UNSIGNED NOT NULL DEFAULT '0',
			ext_data longtext NOT NULL,
			PRIMARY KEY (vid)
		) $charset_collate;";

		dbDelta( $sql );

		// create intl_visitor_identifier table
		$table_name = $wpdb->prefix . "intel_visitor_identifier";

		$sql = "CREATE TABLE $table_name (
  		vid int(10) UNSIGNED NOT NULL,
			type varchar(32) NOT NULL,
			delta smallint(6) NOT NULL DEFAULT '0',
			value varchar(255) NOT NULL,
			KEY vid (vid),
			KEY type (type(10)),
			KEY value (value(10))
		) $charset_collate;";

		dbDelta( $sql );

		// create intl_submission table
		$table_name = $wpdb->prefix . "intel_submission";

		$sql = "CREATE TABLE $table_name (
			sid int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
			vid int(10) UNSIGNED NOT NULL,
			submitted int(11) NOT NULL,
			type varchar(32) NOT NULL,
			fid varchar(64) NOT NULL,
			fsid varchar(128) NOT NULL,
			submission_uri varchar(255) NOT NULL,
			response_page_uri varchar(255) NOT NULL,
			response_page_id varchar(32) NOT NULL,
			form_page_uri varchar(255) NOT NULL,
			form_page_id varchar(32) NOT NULL,
			cta_page_uri varchar(255) NOT NULL,
			cta_page_id varchar(255) NOT NULL,
			cta_id varchar(255) NOT NULL,
			data longtext NOT NULL,
			PRIMARY KEY (sid),
			KEY vid (vid),
			KEY fid (fid),
			KEY fsid (fsid),
			KEY form_page_id (form_page_id),
			KEY form_page_uri (form_page_uri),
			KEY response_page_id (response_page_id),
			KEY response_page_uri (response_page_uri),
			KEY cta_id (cta_id),
			KEY cta_page_id (cta_page_id),
			KEY cta_page_uri (cta_page_uri)
		) $charset_collate;";

		dbDelta( $sql );
	}

	public static function setup_cron() {
		// setup intel_cron_hook
		$timestamp = wp_next_scheduled( 'intel_cron_hook' );
Intel_Df::watchdog('setup_cron cron_hook ts', $timestamp);
		if ($timestamp == FALSE) {
			wp_schedule_event( time(), 'intel_cron_interval', 'intel_cron_hook' );
		}

		// setup intel_cron_queue_hook
		$timestamp = wp_next_scheduled( 'intel_cron_queue_hook' );
Intel_Df::watchdog('setup_cron cron_queue_hook ts', $timestamp);
		if ($timestamp == FALSE) {
			wp_schedule_event( time(), 'intel_cron_queue_interval', 'intel_cron_queue_hook' );
		}
	}

}
