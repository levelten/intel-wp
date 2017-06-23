<?php

/**
 * The public-facing functionality of the plugin.
 *
 * @link       getlevelten.com/blog/tom
 * @since      1.0.0
 *
 * @package    Intel
 * @subpackage Intel/public
 */

/**
 * The public-facing functionality of the plugin.
 *
 * Defines the plugin name, version, and two examples hooks for how to
 * enqueue the admin-specific stylesheet and JavaScript.
 *
 * @package    Intel
 * @subpackage Intel/public
 * @author     Tom McCracken <tomm@getlevelten.com>
 */
class Intel_Public {

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
	 * Initialize the class and set its properties.
	 *
	 * @since    1.0.0
	 * @param      string    $plugin_name       The name of the plugin.
	 * @param      string    $version    The version of this plugin.
	 */
	public function __construct( $plugin_name, $version ) {

		$this->plugin_name = $plugin_name;
		$this->version = $version;

	}

	/**
	 * Register the stylesheets for the public-facing side of the site.
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

		wp_enqueue_style( $this->plugin_name, plugin_dir_url( __FILE__ ) . 'css/intel-public.css', array(), $this->version, 'all' );

	}

	/**
	 * Register the JavaScript for the public-facing side of the site.
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

		wp_enqueue_script( $this->plugin_name, plugin_dir_url( __FILE__ ) . 'js/intel-public.js', array( 'jquery' ), $this->version, false );

	}

	/**
	 * Adds Intelligence link to admin bar
	 * @param $wp_admin_bar
	 */
	public function admin_bar_menu($wp_admin_bar) {

		// check permissions to access reports
		if (!Intel_Df::user_access('view all intel reports')) {
			return;
		}

		// only include toolbar link on front end
		if (is_admin()) {
			return;
		}


		$l_options = array(
			'query' => array(
				'report_params' => 'f0=pagePath:' . Intel_Df::current_pagepath(),
			),
		);
		$args = array(
			'id'    => 'intel',
			'title' => '<span class="icon ab-icon dashicons-before dashicons-analytics"></span>' . Intel_Df::t('Intelligence'),
			//'href'  => Intel_Df::url('admin/reports/intel/scorecard', $l_options),
			'meta'  => array( 'class' => 'intel-toolbar-item' ),
		);
		$wp_admin_bar->add_node( $args );

		$args = array(
			'parent' => 'intel',
			'id'    => 'intel-content-scorecard',
			'title' => Intel_Df::t('Report: Content scorecard'),
			'href'  => Intel_Df::url('admin/reports/intel/scorecard', $l_options),
			'meta'  => array( 'class' => 'intel-toolbar-subitem' ),
		);
		$wp_admin_bar->add_node( $args );

		$args = array(
			'parent' => 'intel',
			'id'    => 'intel-content-trafficsource',
			'title' => Intel_Df::t('Report: Content traffic source'),
			'href'  => Intel_Df::url('admin/reports/intel/trafficsource', $l_options),
			'meta'  => array( 'class' => 'intel-toolbar-subitem' ),
		);
		$wp_admin_bar->add_node( $args );

		$args = array(
			'parent' => 'intel',
			'id'    => 'intel-content-visitor',
			'title' => Intel_Df::t('Report: Content visitors'),
			'href'  => Intel_Df::url('admin/reports/intel/visitor', $l_options),
			'meta'  => array( 'class' => 'intel-toolbar-subitem' ),
		);
		$wp_admin_bar->add_node( $args );

		if (intel_is_intel_script_enabled('admin') && Intel_Df::user_access('admin intel')) {
			if (!empty($_GET['io-admin'])) {
				$query = $_GET;
				unset($query['io-admin']);
				$l_options_admin = Intel_Df::l_options_add_query($query);
				$title_mode = Intel_Df::t('disable');
			}
			else {

				$l_options_admin = Intel_Df::l_options_add_query(array('io-admin' => 1));
				$title_mode = Intel_Df::t('enable');
			}

			$args = array(
				'parent' => 'intel',
				'id'    => 'intel-front-end-admin',
				'title' => Intel_Df::t('Admin: Event explorer') . ' ' . $title_mode,
				'href'  => Intel_Df::url(Intel_Df::current_path(), $l_options_admin),
				'meta'  => array( 'class' => 'intel-toolbar-subitem' ),
			);
			$wp_admin_bar->add_node( $args );
		}

	}

}
