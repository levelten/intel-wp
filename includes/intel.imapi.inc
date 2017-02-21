<?php
/**
 * @file
 * Functions to support extended Google Analytics data.
 *
 * @author Tom McCracken <tomm@getlevelten.com>
 */

/**
 * Returns imapi client
 *
 * @param $vars
 * @param array $options
 * @return bool|\LevelTen\Intel\ApiClient
 * @throws \Exception
 */
function intel_imapi_get_client(&$vars, $options = array()) {
  $vars['tid'] = isset($options['tid']) ? $options['tid'] : get_option('intel_ga_tid', '');
  if (empty($vars['tid'])) {
    $message = Intel_Df::t('Tracking id is not set.');
    throw new Exception($message);
    return FALSE;
  }
  $vars['apikey'] = isset($options['apikey']) ? $options['apikey'] : get_option('intel_apikey', '');
  if (empty($vars['apikey'])) {
    $message = Intel_Df::t('API key is not set');
    throw new Exception($message);
    return FALSE;
  }
  intel_include_library_file('class.apiclient.php');

  $apiUrl = get_option('intel_imapi_url', '');
  $api_params = get_option('intel_imapi_custom_params', array());
  $apiClientProps = array(
    'apiUrl' => ($apiUrl ? $apiUrl : INTEL_IMAPI_URL) . '/',
    'urlrewrite' => 1,
    //'apiConnector' => get_option('intel_l10iapi_connector', ''),
    'apiParams' => $api_params,
  );

  $apiclient = new \LevelTen\Intel\ApiClient($apiClientProps);

  return $apiclient;
}

function intel_imapi_ga_goal_get($goal_id = 0, $options = array()) {
  $vars = array();
  $apiclient = intel_imapi_get_client($vars);

  $response = FALSE;
  $params = isset($options['params']) ? $options['params'] : array();
  $params['tid'] = $vars['tid'];
  $data = array(
    'apikey' => $vars['apikey'],
  );
  if ($goal_id) {
    $params['goal_id'] = $goal_id;
  }

  try {
    $response = $apiclient->getJSON('ga_goal/get', $params, $data);
  }
  catch (Exception $e) {
    throw($e);
  }

  return $response['ga_goal'];

  return $response;
}

function intel_imapi_ga_goal_save($ga_goal, $options = array()) {

  $apiclient = intel_imapi_get_client($vars);

  $response = FALSE;
  $params = isset($options['params']) ? $options['params'] : array();
  $params['tid'] = $vars['tid'];
  $data = array(
    'apikey' => $vars['apikey'],
    'ga_goal' => $ga_goal,
  );

  try {
    $response = $apiclient->getJSON('ga_goal/save', $params, $data);
  }
  catch (Exception $e) {
    throw($e);
  }
  return $response;
}

function intel_imapi_ga_goal_insert($ga_goal, $options = array()) {

  $apiclient = intel_imapi_get_client($vars);

  $response = FALSE;
  $params = isset($options['params']) ? $options['params'] : array();
  $params['tid'] = $vars['tid'];
  $data = array(
    'apikey' => $vars['apikey'],
    'ga_goal' => $ga_goal,
  );

  try {
    $response = $apiclient->getJSON('ga_goal/insert', $params, $data);
  }
  catch (Exception $e) {
    throw($e);
  }
  return $response;
}

function intel_imapi_ga_goal_update($ga_goal, $options = array()) {

  $apiclient = intel_imapi_get_client($vars);

  $response = FALSE;
  $params = isset($options['params']) ? $options['params'] : array();
  $params['tid'] = $vars['tid'];
  $data = array(
    'apikey' => $vars['apikey'],
    'ga_goal' => $ga_goal,
  );

  try {
    $response = $apiclient->getJSON('ga_goal/update', $params, $data);
  }
  catch (Exception $e) {
    throw($e);
  }
  return $response;
}



