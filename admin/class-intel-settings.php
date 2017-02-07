<?php

/**
 * The admin-specific functionality of the plugin.
 *
 * @link       getlevelten.com/blog/tom
 * @since      1.0.0
 *
 * @package    Intl
 * @subpackage Intl/admin
 */

/**
 * The admin-specific functionality of the plugin.
 *
 * Defines the plugin name, version, and two examples hooks for how to
 * enqueue the admin-specific stylesheet and JavaScript.
 *
 * @package    Intl
 * @subpackage Intl/admin
 * @author     Tom McCracken <tomm@getlevelten.com>
 */
class Intel_Settings {
	private static function update_options( $who ) {
		$intel = intel();
		$network_settings = false;
		$options = $intel->config->options; // Get current options
		if ( isset( $_POST['options']['ga_dash_hidden'] ) && isset( $_POST['options'] ) && ( isset( $_POST['gadash_security'] ) && wp_verify_nonce( $_POST['gadash_security'], 'gadash_form' ) ) && $who != 'Reset' ) {
			$new_options = $_POST['options'];
			if ( $who == 'tracking' ) {
				$options['ga_dash_anonim'] = 0;
				$options['ga_event_tracking'] = 0;
				$options['ga_enhanced_links'] = 0;
				$options['ga_dash_remarketing'] = 0;
				$options['ga_dash_adsense'] = 0;
				$options['ga_event_bouncerate'] = 0;
				$options['ga_crossdomain_tracking'] = 0;
				$options['ga_aff_tracking'] = 0;
				$options['ga_hash_tracking'] = 0;
				if ( isset( $_POST['options']['ga_tracking_code'] ) ) {
					$new_options['ga_tracking_code'] = trim( $new_options['ga_tracking_code'], "\t" );
				}
				if ( empty( $new_options['ga_track_exclude'] ) ) {
					$new_options['ga_track_exclude'] = array();
				}
			} else if ( $who == 'backend' ) {
				$options['switch_profile'] = 0;
				$options['backend_item_reports'] = 0;
				$options['dashboard_widget'] = 0;
				if ( empty( $new_options['ga_dash_access_back'] ) ) {
					$new_options['ga_dash_access_back'][] = 'administrator';
				}
			} else if ( $who == 'frontend' ) {
				$options['frontend_item_reports'] = 0;
				if ( empty( $new_options['ga_dash_access_front'] ) ) {
					$new_options['ga_dash_access_front'][] = 'administrator';
				}
			} else if ( $who == 'general' ) {
				$options['ga_dash_userapi'] = 0;
				if ( ! is_multisite() ) {
					$options['automatic_updates_minorversion'] = 0;
				}
			} else if ( $who == 'network' ) {
				$options['ga_dash_userapi'] = 0;
				$options['ga_dash_network'] = 0;
				$options['ga_dash_excludesa'] = 0;
				$options['automatic_updates_minorversion'] = 0;
				$network_settings = true;
			}
			$options = array_merge( $options, $new_options );
			$intel->config->options = $options;
			$intel->config->set_plugin_options( $network_settings );
		}
		return $options;
	}

	public static function general_settings() {
		$intel = intel();

		// check if GADWP plugin is active
		if (is_plugin_active('google-analytics-dashboard-for-wp/gadwp.php')) {
		  $gadwp = GADWP();
		  $gadwp_options = $gadwp->config->options;
		  print_r($gadwp->config->options);
		  //$gadwp_dir_path = plugin_dir_path('google-analytics-dashboard-for-wp/gadwp.php');
		  //print $gadwp_dir_path;
		  //include_once( ABSPATH . '/wp-content/plugins/' . $gadwp_dir_path . 'admin/settings.php');
      //$gadwp_options = GADWP_Settings::update_options( 'general' );
		}

    ?>

		<div class="wrap">
	    <?php echo "<h2>" . __( "Google Analytics Settings", 'intel' ) . "</h2>"; ?>
	    <hr>
    </div>

    <select id="ga_dash_tableid_jail">
		<?php
  		if ( ! empty( $options['ga_dash_profile_list'] ) ) {
	  		foreach ( $options['ga_dash_profile_list'] as $items ) {
		    	if ( $items[3] ) {
				  	echo '<option value="' . esc_attr( $items[1] ) . '" ' . selected( $items[1], $options['ga_dash_tableid_jail'], false );
						echo ' title="' . __( "View Name:", 'google-analytics-dashboard-for-wp' ) . ' ' . esc_attr( $items[0] ) . '">' . esc_html( GADWP_Tools::strip_protocol( $items[3] ) ) . ' &#8658; ' . esc_attr( $items[0] ) . '</option>';
					}
				}
			} else {
				echo '<option value="">' . __( "Property not found", 'google-analytics-dashboard-for-wp' ) . '</option>';
			}
		?>
    </select>

    <?php
	}

	public static function general_settings2() {
		$intel = intel();

		include ( INTL_DIR . 'admin/class-intel-gapi.php' );

		global $wp_version;

		if ( ! current_user_can( 'manage_options' ) ) {
			return;
		}
		$options = self::update_options( 'general' );

		printf( '<div id="gapi-warning" class="updated"><p>%1$s <a href="https://deconf.com/google-analytics-dashboard-wordpress/?utm_source=gadwp_config&utm_medium=link&utm_content=general_screen&utm_campaign=gadwp">%2$s</a></p></div>', __( 'Loading the required libraries. If this results in a blank screen or a fatal error, try this solution:', 'google-analytics-dashboard-for-wp' ), __( 'Library conflicts between WordPress plugins', 'google-analytics-dashboard-for-wp' ) );
		if ( null === $intel->gapi_controller ) {
			$intel->gapi_controller = new Intl_GAPI_Controller();
		}

		echo '<script type="text/javascript">jQuery("#gapi-warning").hide()</script>';
		if ( isset( $_POST['ga_dash_code'] ) ) {
			if ( ! stripos( 'x' . $_POST['ga_dash_code'], 'UA-', 1 ) == 1 ) {
				try {
					$intel->gapi_controller->client->authenticate( $_POST['ga_dash_code'] );
					$intel->config->options['ga_dash_token'] = $intel->gapi_controller->client->getAccessToken();
					$intel->config->options['automatic_updates_minorversion'] = 1;
					$intel->config->set_plugin_options();
					$options = self::update_options( 'general' );
					$message = "<div class='updated'><p>" . __( "Plugin authorization succeeded.", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
					GADWP_Tools::delete_cache( 'gapi_errors' );
					GADWP_Tools::delete_cache( 'last_error' );
					if ( $intel->config->options['ga_dash_token'] && $intel->gapi_controller->client->getAccessToken() ) {
						if ( ! empty( $intel->config->options['ga_dash_profile_list'] ) ) {
							$profiles = $intel->config->options['ga_dash_profile_list'];
						} else {
							$profiles = $intel->gapi_controller->refresh_profiles();
						}
						if ( $profiles ) {
							$intel->config->options['ga_dash_profile_list'] = $profiles;
							if ( ! $intel->config->options['ga_dash_tableid_jail'] ) {
								$profile = GADWP_Tools::guess_default_domain( $profiles );
								$intel->config->options['ga_dash_tableid_jail'] = $profile;
								// $gadwp->config->options['ga_dash_tableid'] = $profile;
							}
							$intel->config->set_plugin_options();
							$options = self::update_options( 'general' );
						}
					}
				} catch ( Google_IO_Exception $e ) {
					GADWP_Tools::set_cache( 'last_error', date( 'Y-m-d H:i:s' ) . ': ' . esc_html( $e ), $intel->gapi_controller->error_timeout );
					return false;
				} catch ( Google_Service_Exception $e ) {
					GADWP_Tools::set_cache( 'last_error', date( 'Y-m-d H:i:s' ) . ': ' . esc_html( "(" . $e->getCode() . ") " . $e->getMessage() ), $intel->gapi_controller->error_timeout );
					GADWP_Tools::set_cache( 'gapi_errors', $e->getErrors(), $intel->gapi_controller->error_timeout );
					return $e->getCode();
				} catch ( Exception $e ) {
					GADWP_Tools::set_cache( 'last_error', date( 'Y-m-d H:i:s' ) . ': ' . esc_html( $e ) . "\nResponseHttpCode:" . $e->getCode(), $intel->gapi_controller->error_timeout );
					$intel->gapi_controller->reset_token( false );
				}
			} else {
				$message = "<div class='error'><p>" . __( "The access code is <strong>NOT</strong> your <strong>Tracking ID</strong> (UA-XXXXX-X). Try again, and use the red link to get your access code", 'google-analytics-dashboard-for-wp' ) . ".</p></div>";
			}
		}
		if ( isset( $_POST['Clear'] ) ) {
			if ( isset( $_POST['gadash_security'] ) && wp_verify_nonce( $_POST['gadash_security'], 'gadash_form' ) ) {
				GADWP_Tools::clear_cache();
				$message = "<div class='updated'><p>" . __( "Cleared Cache.", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			} else {
				$message = "<div class='error'><p>" . __( "Cheating Huh?", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			}
		}
		if ( isset( $_POST['Reset'] ) ) {
			if ( isset( $_POST['gadash_security'] ) && wp_verify_nonce( $_POST['gadash_security'], 'gadash_form' ) ) {
				$intel->gapi_controller->reset_token( true );
				GADWP_Tools::clear_cache();
				$message = "<div class='updated'><p>" . __( "Token Reseted and Revoked.", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
				$options = self::update_options( 'Reset' );
			} else {
				$message = "<div class='error'><p>" . __( "Cheating Huh?", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			}
		}
		if ( isset( $_POST['Reset_Err'] ) ) {
			if ( isset( $_POST['gadash_security'] ) && wp_verify_nonce( $_POST['gadash_security'], 'gadash_form' ) ) {
				GADWP_Tools::delete_cache( 'last_error' );
				GADWP_Tools::delete_cache( 'gapi_errors' );
				$message = "<div class='updated'><p>" . __( "All errors reseted.", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			} else {
				$message = "<div class='error'><p>" . __( "Cheating Huh?", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			}
		}
		if ( isset( $_POST['options']['ga_dash_hidden'] ) && ! isset( $_POST['Clear'] ) && ! isset( $_POST['Reset'] ) && ! isset( $_POST['Reset_Err'] ) ) {
			$message = "<div class='updated'><p>" . __( "Settings saved.", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			if ( ! ( isset( $_POST['gadash_security'] ) && wp_verify_nonce( $_POST['gadash_security'], 'gadash_form' ) ) ) {
				$message = "<div class='error'><p>" . __( "Cheating Huh?", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			}
		}
		if ( isset( $_POST['Hide'] ) ) {
			if ( isset( $_POST['gadash_security'] ) && wp_verify_nonce( $_POST['gadash_security'], 'gadash_form' ) ) {
				$message = "<div class='updated'><p>" . __( "All other domains/properties were removed.", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
				$lock_profile = GADWP_Tools::get_selected_profile( $intel->config->options['ga_dash_profile_list'], $intel->config->options['ga_dash_tableid_jail'] );
				$intel->config->options['ga_dash_profile_list'] = array( $lock_profile );
				$options = self::update_options( 'general' );
			} else {
				$message = "<div class='error'><p>" . __( "Cheating Huh?", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
			}
		}
		?>
    <div class="wrap">
	    <?php echo "<h2>" . __( "Google Analytics Settings", 'google-analytics-dashboard-for-wp' ) . "</h2>"; ?>
	    <hr>
    </div>
      <div id="poststuff" class="gadwp">
        <div id="post-body" class="metabox-holder columns-2">
          <div id="post-body-content">
            <div class="settings-wrapper">
              <div class="inside">
<?php
		if ( $intel->gapi_controller->gapi_errors_handler() || GADWP_Tools::get_cache( 'last_error' ) ) {
			$message = sprintf( '<div class="error"><p>%s</p></div>', sprintf( __( 'Something went wrong, check %1$s or %2$s.', 'google-analytics-dashboard-for-wp' ), sprintf( '<a href="%1$s">%2$s</a>', menu_page_url( 'gadash_errors_debugging', false ), __( 'Errors & Debug', 'google-analytics-dashboard-for-wp' ) ), sprintf( '<a href="%1$s">%2$s</a>', menu_page_url( 'gadash_settings', false ), __( 'authorize the plugin', 'google-analytics-dashboard-for-wp' ) ) ) );
		}
		if ( isset( $_POST['Authorize'] ) ) {
			GADWP_Tools::clear_cache();
			$intel->gapi_controller->token_request();
			echo "<div class='updated'><p>" . __( "Use the red link (see below) to generate and get your access code!", 'google-analytics-dashboard-for-wp' ) . "</p></div>";
		} else {
			if ( isset( $message ) ) {
				echo $message;
			}
			?>
					<form name="ga_dash_form" method="post" action="<?php echo esc_url($_SERVER['REQUEST_URI']); ?>">
                                            <input type="hidden" name="options[ga_dash_hidden]" value="Y">
						<?php wp_nonce_field('gadash_form','gadash_security'); ?>
						<table class="gadwp-settings-options">
                                                <tr>
                                                    <td colspan="2">
                                                        <?php echo "<h2>" . __( "Plugin Authorization", 'google-analytics-dashboard-for-wp' ) . "</h2>";?>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2" class="gadwp-settings-info">
                                                        <?php printf(__('You should watch the %1$s and read this %2$s before proceeding to authorization. This plugin requires a properly configured Google Analytics account!', 'google-analytics-dashboard-for-wp'), sprintf('<a href="%1$s" target="_blank">%2$s</a>', 'https://deconf.com/google-analytics-dashboard-wordpress/?utm_source=gadwp_config&utm_medium=link&utm_content=top_video&utm_campaign=gadwp', __("video", 'google-analytics-dashboard-for-wp')), sprintf('<a href="%1$s" target="_blank">%2$s</a>', 'https://deconf.com/google-analytics-dashboard-wordpress/?utm_source=gadwp_config&utm_medium=link&utm_content=top_tutorial&utm_campaign=gadwp', __("tutorial", 'google-analytics-dashboard-for-wp')));?>
						                            </td>
                                                </tr>
						                          <?php if (! $options['ga_dash_token'] || $options['ga_dash_userapi']) {?>
						                          <tr>
                                                    <td colspan="2" class="gadwp-settings-info"><input name="options[ga_dash_userapi]" type="checkbox" id="ga_dash_userapi" value="1" <?php checked( $options['ga_dash_userapi'], 1 ); ?> onchange="this.form.submit()" <?php echo ($options['ga_dash_network'])?'disabled="disabled"':''; ?> /><?php echo " ".__("use your own API Project credentials", 'google-analytics-dashboard-for-wp' );?>
							                            </td>
                                                </tr>
                                				  <?php } if ($options['ga_dash_userapi']) { ?>
                                                <tr>
                                                    <td class="gadwp-settings-title"><label for="options[ga_dash_clientid]"><?php _e("Client ID:", 'google-analytics-dashboard-for-wp'); ?></label></td>
                                                    <td><input type="text" name="options[ga_dash_clientid]" value="<?php echo esc_attr($options['ga_dash_clientid']); ?>" size="40" required="required"></td>
                                                </tr>
                                                <tr>
                                                    <td class="gadwp-settings-title"><label for="options[ga_dash_clientsecret]"><?php _e("Client Secret:", 'google-analytics-dashboard-for-wp'); ?></label></td>
                                                    <td><input type="text" name="options[ga_dash_clientsecret]" value="<?php echo esc_attr($options['ga_dash_clientsecret']); ?>" size="40" required="required"> <input type="hidden" name="options[ga_dash_hidden]" value="Y">
									                    <?php wp_nonce_field('gadash_form','gadash_security'); ?>
								                    </td>
                                                </tr>
						<?php
			}
			if ( $options['ga_dash_token'] ) {
				?>
					                            <tr>
                                                    <td colspan="2"><input type="submit" name="Reset" class="button button-secondary" value="<?php _e( "Clear Authorization", 'google-analytics-dashboard-for-wp' ); ?>" <?php echo $options['ga_dash_network']?'disabled="disabled"':''; ?> /> <input type="submit" name="Clear" class="button button-secondary" value="<?php _e( "Clear Cache", 'google-analytics-dashboard-for-wp' ); ?>" /> <input type="submit" name="Reset_Err" class="button button-secondary" value="<?php _e( "Reset Errors", 'google-analytics-dashboard-for-wp' ); ?>" /></td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2"><hr></td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2"><?php echo "<h2>" . __( "General Settings", 'google-analytics-dashboard-for-wp' ) . "</h2>"; ?></td>
                                                </tr>
                                                <tr>
                                                    <td class="gadwp-settings-title"><label for="ga_dash_tableid_jail"><?php _e("Select View:", 'google-analytics-dashboard-for-wp' ); ?></label></td>
                                                    <td><select id="ga_dash_tableid_jail" <?php disabled(empty($options['ga_dash_profile_list']) || 1 == count($options['ga_dash_profile_list']), true); ?> name="options[ga_dash_tableid_jail]">
                                    								<?php
				if ( ! empty( $options['ga_dash_profile_list'] ) ) {
					foreach ( $options['ga_dash_profile_list'] as $items ) {
						if ( $items[3] ) {
							echo '<option value="' . esc_attr( $items[1] ) . '" ' . selected( $items[1], $options['ga_dash_tableid_jail'], false );
							echo ' title="' . __( "View Name:", 'google-analytics-dashboard-for-wp' ) . ' ' . esc_attr( $items[0] ) . '">' . esc_html( GADWP_Tools::strip_protocol( $items[3] ) ) . ' &#8658; ' . esc_attr( $items[0] ) . '</option>';
						}
					}
				} else {
					echo '<option value="">' . __( "Property not found", 'google-analytics-dashboard-for-wp' ) . '</option>';
				}
				?>
                                    							</select>
                                    							<?php
				if ( count( $options['ga_dash_profile_list'] ) > 1 ) {
					?>&nbsp;<input type="submit" name="Hide" class="button button-secondary" value="<?php _e( "Lock Selection", 'google-analytics-dashboard-for-wp' ); ?>" /><?php
				}
				?>
							                         </td>
                                                </tr>
							<?php
				if ( $options['ga_dash_tableid_jail'] ) {
					?>
							<tr>
                                                    <td class="gadwp-settings-title"></td>
                                                    <td><?php
					$profile_info = GADWP_Tools::get_selected_profile( $intel->config->options['ga_dash_profile_list'], $intel->config->options['ga_dash_tableid_jail'] );
					echo '<pre>' . __( "View Name:", 'google-analytics-dashboard-for-wp' ) . "\t" . esc_html( $profile_info[0] ) . "<br />" . __( "Tracking ID:", 'google-analytics-dashboard-for-wp' ) . "\t" . esc_html( $profile_info[2] ) . "<br />" . __( "Default URL:", 'google-analytics-dashboard-for-wp' ) . "\t" . esc_html( $profile_info[3] ) . "<br />" . __( "Time Zone:", 'google-analytics-dashboard-for-wp' ) . "\t" . esc_html( $profile_info[5] ) . '</pre>';
					?></td>
                                                </tr>
							<?php
				}
				?>
							                     <tr>
                                                    <td class="gadwp-settings-title"><label for="ga_dash_style"><?php _e("Theme Color:", 'google-analytics-dashboard-for-wp' ); ?></label></td>
                                                    <td><input type="text" id="ga_dash_style" class="ga_dash_style" name="options[ga_dash_style]" value="<?php echo esc_attr($options['ga_dash_style']); ?>" size="10"></td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2"><hr></td>
                                                </tr>
												<?php if ( !is_multisite()) {?>
												<tr>
                                                    <td colspan="2"><?php echo "<h2>" . __( "Automatic Updates", 'google-analytics-dashboard-for-wp' ) . "</h2>"; ?></td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2" class="gadwp-settings-title">
                                                        <div class="button-primary gadwp-settings-switchoo">
                                                            <input type="checkbox" name="options[automatic_updates_minorversion]" value="1" class="gadwp-settings-switchoo-checkbox" id="automatic_updates_minorversion" <?php checked( $options['automatic_updates_minorversion'], 1 ); ?>> <label class="gadwp-settings-switchoo-label" for="automatic_updates_minorversion">
                                                                <div class="gadwp-settings-switchoo-inner"></div>
                                                                <div class="gadwp-settings-switchoo-switch"></div>
                                                            </label>
                                                        </div>
                                                        <div class="switch-desc"><?php echo " ".__( "automatic updates for minor versions (security and maintenance releases only)", 'google-analytics-dashboard-for-wp' );?></div>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2"><hr></td>
                                                </tr>
												<?php }?>
                                                <tr>
                                                    <td colspan="2" class="submit"><input type="submit" name="Submit" class="button button-primary" value="<?php _e('Save Changes', 'google-analytics-dashboard-for-wp' ) ?>" /></td>
                                                </tr>
		<?php } else {?>
							                    <tr>
                                                    <td colspan="2"><hr></td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2"><input type="submit" name="Authorize" class="button button-secondary" id="authorize" value="<?php _e( "Authorize Plugin", 'google-analytics-dashboard-for-wp' ); ?>" <?php echo $options['ga_dash_network']?'disabled="disabled"':''; ?> /> <input type="submit" name="Clear" class="button button-secondary" value="<?php _e( "Clear Cache", 'google-analytics-dashboard-for-wp' ); ?>" /></td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2"><hr></td>
                                                </tr>
                                            </table>
                                        </form>
			<?php
				//self::output_sidebar();
				return;
			}
			?>
					</table>
                                        </form>
<?php
		}
		//self::output_sidebar();
	}
}
