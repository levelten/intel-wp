<?php
/**
 * @file
 * @author  Tom McCracken <tomm@levelten.net>
 * @version 1.0
 * @copyright 2013 LevelTen Ventures
 *
 * @section LICENSE
 * All rights reserved. Do not use without permission.
 *
 */
namespace LevelTen\Intel;
if (!empty($_GET['debug'])) {
  require_once __DIR__ . '/../libs/class.debug.php';
}

class GAModel {
  public  $data = array();
  public  $filters;
  public  $gaFilters = array();
  public  $customFilters = array(
    'filter' => '',
    'segement' => '',
  );
  public  $context;
  private $start_date;
  private $end_date;
  public  $settings;
  public  $pathQueryFilters = array();
  private $pageAttributeFilter = array();
  private $pageAttributeInfo = array();
  private $requestCallback = '';
  private $requestCallbackOptions = array();
  private $dataIndexCallbacks = array();
  private $debug = 0;
  // flag to enable
  private $advancedSort = 0;
  private $reportModes = array();
  private $requestDefault = array(
    'dimensions' => array(),
    'metrics' => array(),
    'sort' => '',
    'start_date' => 0,
    'end_date' => 0,
    'filters' => '',
    'segment' => '',
    'max_results' => 50,
  );
  private $requestSettings = array(
    'type' => 'pageviews',
    'subType' => 'value',
    'indexBy' => 'date',
    'subIndexBy' => '',
    'details' => 0,
    'sortType' => '',
    'excludeGoalFields' => 0,
  );
  // stores array of entities, e.g. page attributes, indexed by pagepaths
  public $pagePathMap = array();

  function __contruct() {

  }

  function setDateRange($start_date, $end_date) {
    $this->start_date = $start_date;
    $this->end_date = $end_date;
    $this->requestDefault['start_date'] = $start_date;
    $this->requestDefault['end_date'] = $end_date;
  }

  function setRequestSetting($param, $value) {
    $this->requestSettings[$param] = $value;
  }

  function setRequestDefaultParam($param, $value) {
    $this->requestDefault[$param] = $value;
  }

  function setRequestCallback ($callback, $options = array()) {
    $this->requestCallback = $callback;
    $this->requestCallbackOptions = $options;
  }

  function setDataIndexCallback($index, $callback) {
    $this->dataIndexCallbacks[$index] = $callback;
  }

  function setReportModes($modes) {
    if (is_string($modes)) {
      $modes = explode('.', $modes);
    }
    $this->reportModes = $modes;
  }

  function setAdvancedSort($value) {
    $this->advancedSort = $value;
  }

  function setDebug($debug) {
    $this->debug = $debug;
    if ($this->debug && !function_exists('Debug::printVar')) {
      require_once __DIR__ . '/../libs/class.debug.php';
    }
  }

  function buildFilters($filters, $context = 'site') {
    $gasegments = array();
    $gafilters = array();
    $gapaths = array(
      'pagePath' => '',
      'landingPagePath' => '',
    );

    if (!empty($filters['page'])) {
      $a = explode(":", $filters['page']);
      $path = $a[1];

      $pathfilter = $this->formatPathFilter($path);
      $landingpagefilter = str_replace('pagePath', 'landingPagePath', $pathfilter);

      if ($context == 'page') {
        $gapaths['pagePath'] = $pathfilter;
        $gapaths['landingPagePath'] = str_replace('pagePath', 'landingPagePath', $landingpagefilter);
      }
      else {
        if ($a[0] == 'landingPagePath') {
          $gafilters['page'] = $landingpagefilter;
        }
        else {
          $gafilters['page'] = $pathfilter;
        }
      }
    }

    if (!empty($filters['page-attr'])) {
      $a = explode(':', $filters['page-attr']);
      // TODO add other attribute types
      if (($this->pageAttributeInfo['type'] == 'scalar') || ($this->pageAttributeInfo['type'] == 'item')) {
        $gafilters['page-attr'] = 'ga:dimension1=@&' . $a[0] . '=' . $a[1] . '&';
      }
      else if ($this->pageAttributeInfo['type'] == 'list') {
        $gafilters['page-attr'] = 'ga:dimension1=@&' . $a[0] . '.' . $a[1] . '&';
      }
    }

    if (!empty($filters['event'])) {
      $gasegments['event'] = "ga:eventCategory=~^" . $filters['event'];
    }

    if (!empty($filters['trafficsource'])) {
      $a = explode(":", $filters['trafficsource']);
      $gafilters['trafficsource'] = "ga:{$a[0]}=={$a[1]}";
    }

    if (!empty($filters['location'])) {
      $a = explode(":", $filters['location']);
      $gafilters['location'] = "ga:{$a[0]}=={$a[1]}";
      if (($a[0] == 'country') || ($a[0] == 'region') || ($a[0] == 'city') || ($a[0] == 'metro')) {
        $this->settings['location_dimension_level'] = 'region';
      }
    }

    if (!empty($filters['visitor'])) {
      $a = explode(":", $filters['visitor']);
      $vtk = $a[1];
      //$gasegments['visitor'] = "ga:customVarValue5==" . $vtk;
      $gafilters['visitor'] = "ga:dimension5==" . $vtk;
    }

    if (!empty($filters['visitor-attr'])) {
      $a = str_replace(':', '=', $filters['visitor-attr']);
      $query = l10insight_build_ga_filter_from_serialized_customvar($a);
      //$ga_segments['visitor-attr'] = "dynamic::ga:dimension3=" . $query;
    }
    $this->gaFilters = array(
      'filters' => $gafilters,
      'segments' => $gasegments,
      //'filter' => implode(";", $gafilters),
      //'segment' => implode(";", $gasegments),
      'paths' => $gapaths,
    );
  }
  /**
   *
   * @param $item
   * @param $type: valid values = 'filter' or 'segement'
   */
  function addGAFilter($item, $type = 'filter') {
    if ($type == 'segment') {
      $this->gaFilters['segements'][] = $item;
      //$this->gaFilters['segement'] =  implode(";", $this->gaFilters['segements']);
    }
    else if ($type == 'filter') {
      $this->gaFilters['filters'][] = $item;
      //$this->gaFilters['filter'] =  implode(";", $this->gaFilters['filters']);      
    }
  }

  function setPageAttributeFilter($filter) {
    $this->pageAttributeFilter = $filter;
  }

  function setPageAttributeInfo($info) {
    $this->pageAttributeInfo[$info['key']] = $info;
  }


  function applyFiltersToRequest($request) {
    if (!empty($this->gaFilters['filter'])) {
      $request['filters'] .= (($request['filters']) ? ';' : '') . $this->gaFilters['filter'];
    }
    if (!empty($this->gaFilters['segment'])) {
      $request['segment'] .= (($request['segment']) ? ';' : '') . $this->gaFilters['segment'];
    }
    return $request;
  }

  function setCustomFilter($string, $type = 'filter') {
    $this->customFilters[$type] = $string;
  }

  //function loadFeedData($type, $indexBy = '', $details = 0, $max_results = 100, $results_index = 0) {
  function loadFeedData($type = '', $indexBy = '', $details = 0) {
    // format the ga request array
    $request = $this->getRequestArray($type, $indexBy, $details);
    if ($this->debug) { Debug::printVar($request); }

    // advanced sort used on requests with 2 sorts to split the requests into
    // two
    $sorts = array();
    $threshold = 0;
    $advancedSort = $this->advancedSort;
    if ($advancedSort) {
      $sorts = explode(',', $request['sort']);
      if (count($sorts) == 2) {
        // if two sorts, implement advancedSort using half the max results on each request
        $request['max_results'] = floor($request['max_results']/2);
      }
      else {
        $advancedSort = 0;
      }
    }
    // call dyanmic function to fetch data from ga
    if ($this->requestCallback) {
      $func = $this->requestCallback;
      $feed = $func($request, $this->requestCallbackOptions);
    }
    if ($this->debug) { Debug::printVar($feed); }

    // if max_results not returned, disable advancedSort
    if (is_array($feed->results) && (count($feed->results) < $request['max_results'])) {
      $advancedSort = 0;
    }

    // if advanced search, determine the threshold for next request by looking
    // at the bottom values for the primary sort field
    if ($advancedSort && is_array($feed->results)) {
      // remove - sign if it exists on sort field
      $field = str_replace('-', '', $sorts[0]);
      $field = str_replace('ga:', '', $field);
      $last_i = count($feed->results)-1;
      $threshold = $feed->results[$last_i][$field];

      for ($i = $last_i - 1; $i; $i--) {
        if ($feed->results[$i][$field] != $threshold) {
          $last_i = $i;
          break;
        }
      }
      // unset records at the threshold
      for ($i = count($feed->results)-1; $i > $last_i; $i--) {
        // decrement totals for removed records
        foreach ($feed->totals AS $key => $value) {
          $feed->totals[$key] -= $feed->results[$i][$key];
        }
        unset($feed->results[$i]);
      }
      if ($this->debug) { Debug::printVar("field=$field, thresh=$threshold, last_i=$last_i"); }
      if ($this->debug) { Debug::printVar($feed); }
    }

    // parse data into $this->data
    $this->addFeedData($feed, $type, $indexBy, $details);
    if ($this->debug) { Debug::printVar($this->data); }

    // if advanced search, run second request fliping the sort and using the
    // threshold to
    if ($advancedSort && is_array($feed->results)) {
      $request['sort'] = $sorts[1] . ',' . $sorts[0];
      $request['filters'] .= ($request['filters'] ? ';' : '') . 'ga:' . $field . '<=' . $threshold;
      // expand result to cover unset records from prior query
      $request['max_results'] += ($request['max_results'] - $last_i);
      if ($this->debug) { Debug::printVar($request); }

      // call dyanmic function to fetch data from ga
      if ($this->requestCallback) {
        $func = $this->requestCallback;
        $feed = $func($request, $this->requestCallbackOptions);
      }
      if ($this->debug) { Debug::printVar($feed); }

      // parse data into $this->data
      $this->addFeedData($feed, $type, $indexBy, $details);
      if ($this->debug) { Debug::printVar($this->data); }

    }

    return $this->data;
  }

  //function getRequestArray($type, $sub_type = 'value_desc', $indexBy = '', $details = 0, $max_results = 100, $results_index = 0) {
  /**
   * Builds query array for Google Analytics API request.
   *
   * @param string $type
   *   pageviews: general metrics for page hits
   *   entrances: associates all general pageview metrics during sessions with
   *     first page hit of the session (entrance page)
   *   pageviews_valuedevents: valued event metrics associated with the prior pageview
   *   entrances_valuedevents: valued event metrics associated with entrance pages
   *   pageviews_goals: goal values & completions associated with the prior pageview
   *   entrances_goals: goal values & completions associated with entrance pages
   * @param string $indexBy
   * @param int $details
   * @return array
   */
  function getRequestArray($type = '', $indexBy = '', $details = 0) {
    $request = $this->requestDefault;
    $settings = $this->requestSettings;
    $type = ($type) ? $type : $settings['type'];
    $types = explode('_', $type);
    $subType = $settings['subType'];
    $indexBy = ($indexBy) ?  $indexBy : $settings['indexBy'];
    $subIndexBy = $settings['subIndexBy'];
    $details = ($details) ? $details : $settings['details'];
    $reportModes = $this->reportModes;
    $filters = $this->gaFilters['filters'];
    $segments = $this->gaFilters['segments'];

    // Phase I: Set metrics based on request $type
    // later, phase II: sets dimentions based on $indexBy
    if ($type == 'pageviews') {
      if (isset($reportModes[0]) && (($reportModes[0] == 'social') || ($reportModes[0] == 'seo'))) {
        $request['metrics'] = array('ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:exits');
        $request['sort'] = '-ga:pageviews';
      }
      else {
        $request['metrics'] = array('ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnPage', 'ga:exits', 'ga:pageValue', 'ga:goalValueAll', 'ga:goalCompletionsAll');
        $request['sort'] = '-ga:pageValue,-ga:pageviews';
      }
    }
    else if ($type == 'entrances') {
      if (isset($reportModes[0]) && (($reportModes[0] == 'social') || ($reportModes[0] == 'seo'))) {
        $request['metrics'] = array('ga:entrances', 'ga:newVisits', 'ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnSite', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll');
        $request['sort'] = '-ga:entrances';
      }
      else {
        $request['metrics'] = array('ga:entrances', 'ga:newVisits', 'ga:pageviews', 'ga:uniquePageviews', 'ga:timeOnSite', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll');
        $request['sort'] = '-ga:goalValueAll,-ga:entrances';
        //$request['metrics'] = array('ga:entrances', 'ga:newVisits', 'ga:pageviewsPerSession', 'ga:timeOnSite', 'ga:bounces', 'ga:goalValueAll', 'ga:goalCompletionsAll', 'ga:pageValue');
        //$request['sort'] = '-ga:pageValue,-ga:entrances';
      }
    }
    else if ($type == 'pageviews_valuedevents') {
      $request['metrics'] = array('ga:totalEvents', 'ga:uniqueEvents', 'ga:eventValue');
      if (isset($reportModes[0]) && ($reportModes[0] == 'landingpage')) {
        $filters[] = "ga:eventCategory=~^Landing page";
        $request['sort'] = '-ga:totalEvents';
      }
      else if (isset($reportModes[0]) && ($reportModes[0] == 'social')) {
        $filters[] = "ga:eventCategory=~^Social";
        $request['sort'] = '-ga:totalEvents';
      }
      else {
        $filters[] = "ga:eventCategory=~^*!$";
        $request['sort'] = '-ga:eventValue,-ga:totalEvents';
      }
    }
    else if ($type == 'entrances_valuedevents') {
      $request['metrics'] = array('ga:totalEvents', 'ga:uniqueEvents', 'ga:eventValue');
      $filters[] = "ga:eventCategory=~^*!$";
      $request['sort'] = '-ga:eventValue,-ga:totalEvents';
    }
    else if ($type == 'pageviews_goals_assist') {
      $request['metrics'] = array('ga:goalValueAll', 'ga:goalCompletionsAll');
      $request['dimensions'] = array('ga:goalCompletionLocation', 'ga:goalPreviousStep1', 'ga:goalPreviousStep2', 'ga:goalPreviousStep3');
    }
    else if ($type == 'pageviews_goals') {
      foreach ($details AS $id) {
        $request['metrics'][] = "ga:goal{$id}Completions";
        $request['metrics'][] = "ga:goal{$id}Value";
      }
    }
    else if ($type == 'entrances_goals') {
      foreach ($details AS $id) {
        $request['metrics'][] = "ga:goal{$id}Completions";
        $request['metrics'][] = "ga:goal{$id}Value";
      }
    }
    else if ($type == 'eventsource_events') {
      $request['dimensions'][] = 'ga:eventCategory';
      $request['dimensions'][] = 'ga:eventAction';
      $request['dimensions'][] = 'ga:eventLabel';
      //$request['dimensions'][] = 'ga:pagePath';
      $request['metrics'] = array('ga:totalEvents', 'ga:uniqueEvents', 'ga:eventValue');
      $request['sort'] = '-ga:totalEvents';
    }
    else if ($type == 'visit_info') {
      $request['dimensions'][] = 'ga:country';
      $request['dimensions'][] = 'ga:medium';
      $request['dimensions'][] = 'ga:socialNetwork';
      $request['metrics'] = array('ga:entrances', 'ga:goalValueAll');
      if ($reportModes[0] == 'visit') {  // visit.recent report
        $request['dimensions'][] = 'ga:visitCount';
        $request['dimensions'][] = 'ga:dimension4';
      }
      $request['sort'] = '-ga:goalValueAll,-ga:entrances';
      if ($this->gaFilters['paths']['pagePath']) {
        $segments[] = $this->gaFilters['paths']['pagePath'];
      }
    }
    else if ($type == 'pagepathmap_entrances') {
      $request['dimensions'][] = 'ga:dimension1';
      $request['dimensions'][] = 'ga:pagePath';
      $request['metrics'] = array('ga:entrances', 'ga:pageValue');
      $request['sort'] = '-ga:pageValue,-ga:entrances';
      $filters[] = 'ga:entrances>0';
    }

    // Phase II. Set elements based on extra filters

    // if page path filter
    if (($types[0] == 'entrances') && ($this->gaFilters['paths']['landingPagePath'])) {
      $filters[] = $this->gaFilters['paths']['landingPagePath'];
    }
    if (($types[0] == 'pageviews') && ($this->gaFilters['paths']['pagePath'])) {
      $filters[] = $this->gaFilters['paths']['pagePath'];
    }

    // if page attribute filter
    if (isset($filters['page-attr'])) {
      if ($type == 'entrances' || $type == 'entrances_valuedevents' || $type == 'entrances_goals') {
        $segments['page-attr'] = 'sessions::sequence::^' . $filters['page-attr'];
        unset($filters['page-attr']);
      }
    }

    // if indexing on content, add hostname and landingPagePath dimentions


    // i belive this is not needed. The filters are applied at the end
    //$request = $this->applyFiltersToRequest($request);
    if ($indexBy == 'date') {
      $request['dimensions'][] = 'ga:date';
      if (isset($filters['page-attr'])) {
        if ($type == 'entrances') {
          $request['dimensions'][] = 'ga:landingPagePath';
        }
      }
      // TODO do real days calculation
      //$request['max_results'] = 30 * $items;  
    }
    else if ($indexBy == 'content' || $indexBy == 'pagePath') {
      if ($type == 'entrances' || $type == 'entrances_valuedevents') {
        $request['dimensions'][] = 'ga:landingPagePath';
      }
      else {
        $request['dimensions'][] = 'ga:pagePath';
      }
    }
    else if ($indexBy == 'visitor') {
      $request['dimensions'][] = 'ga:dimension5';
    }
    else if ($indexBy == 'visit') {
      $request['dimensions'][] = 'ga:dimension5';
      $request['dimensions'][] = 'ga:visitCount';
      if ($reportModes[1] == 'recent') {
        $request['dimensions'][] = 'ga:dimension4';
        $request['sort'] = '-ga:dimension4';
        if ($type == 'entrances') {
          $filters[] = 'ga:entrances>0';
        }
      }
      //$request['segment'] = (!empty($request['segment']) ? ';' . $this->gaFilters['paths']['pagePath'] : ''); 
    }
    else if ($indexBy == 'trafficsources') {
      $request['dimensions'][] = 'ga:medium';
      $request['dimensions'][] = 'ga:source';
      $request['dimensions'][] = 'ga:referralPath';
      $request['dimensions'][] = 'ga:keyword';
      $request['dimensions'][] = 'ga:socialNetwork';
      $request['dimensions'][] = 'ga:campaign';
    }
    else if ($indexBy == 'trafficcategory') {
      $request['dimensions'][] = 'ga:medium';
      $request['dimensions'][] = 'ga:socialNetwork';
    }
    else if ($indexBy == 'searchKeyword') {
      $request['dimensions'][] = 'ga:medium';
      $request['dimensions'][] = 'ga:keyword';
      $filters[] = 'ga:medium==organic';
    }
    else if ($indexBy == 'searchEngine') {
      $request['dimensions'][] = 'ga:medium';
      $request['dimensions'][] = 'ga:source';
      $filters[] = 'ga:medium==organic';
    }
    else if ($indexBy == 'referralHostpath') {
      $request['dimensions'][] = 'ga:source';
      $request['dimensions'][] = 'ga:referralPath';
    }
    else if ($indexBy == 'landingpage') {
      $filters[] = "ga:eventCategory=~^Landing page";
    }
    else if (strpos($indexBy, 'pageAttribute:') === 0) {
      $indexBys = explode(':', $indexBy);
      $attr_key = $indexBys[1];
      // add dimension1 as dimension

      $filt = '';
      if (($this->pageAttributeInfo[$attr_key]['type'] == 'scalar') || ($this->pageAttributeInfo[$attr_key]['type'] == 'item')) {
        $filt = 'ga:dimension1=@&' . $indexBys[1] . '=';
      }
      else if ($this->pageAttributeInfo[$attr_key]['type'] == 'list') {
        $filt = 'ga:dimension1=@&' . $indexBys[1] . '.';
      }
      // don't add customVarValue dimension to entrance oriented requests.
      // the customVarValue for every session hit will be included
      if ($type == 'entrances' || $type == 'entrances_valuedevents') {
        $request['dimensions'][] = 'ga:landingPagePath';
        // helps to reduce records by filtering only records that enter in page-attr
        $segments['page-attr'] = 'sessions::sequence::^' . $filt;
      }
      else if ($type == 'pageviews') {
        $request['dimensions'][] = 'ga:dimension1';
      }
      else if ($type == 'pageviews_valuedevents') {
        $request['dimensions'][] = 'ga:dimension1';
        // reduce records by filtering only pageviews in page-attr
        $filters['page-attr'] = $filt;
      }

      /*
      // remove any existing filters, all page attributes needs filter without
      // default & at end
      //$filters = array();
      if (($this->pageAttributeInfo['type'] == 'scalar') || ($this->pageAttributeInfo['type'] == 'item')) {
        //$filters['page-attr'] = 'ga:dimension1=@&' . $indexBys[1] . '=';
      }
      else if ($this->pageAttributeInfo['type'] == 'list') {
        //$filters['page-attr'] = 'ga:dimension1=@&' . $indexBys[1] . '.';
      }
      if ($type == 'entrances') {
        $segments['page-attr'] = 'sessions::sequence::^' . $filters['page-attr'];
        unset($filters['page-attr']);
        //$segments[] = 'sessions::condition::ga:entrances>0';
      }
      */

    }
    else if (!empty($indexBy)) {
      $request['dimensions'][] = 'ga:' . $indexBy;
    }

    if ($subIndexBy) {
      if ($subIndexBy == 'trafficcategory') {
        $request['dimensions'][] = 'ga:medium';
        $request['dimensions'][] = 'ga:socialNetwork';
      }
      else if ($subIndexBy == 'socialNetwork') {
        $filters[] = 'ga:socialNetwork!=(not set)';
        $request['sort'] = '-ga:goalValueAll,-ga:entrances';
        $request['dimensions'][] = 'ga:socialNetwork';
      }
      else if ($subIndexBy == 'organicSearch') {
        $request['dimensions'][] = 'ga:medium';
        $request['metrics'][] = 'ga:organicSearches';
        $filters[] = 'ga:medium==organic';
        $request['sort'] = '-ga:organicSearches';
      }
      else if ($subIndexBy == 'searchKeyword') {
        $request['dimensions'][] = 'ga:medium';
        $request['dimensions'][] = 'ga:keyword';
        $request['metrics'][] = 'ga:organicSearches';
        $filters[] = 'ga:medium==organic';
        $filters[] = 'ga:keyword!@(not ';
        $request['sort'] = '-ga:organicSearches';
      }
      else {
        $request['dimensions'][] = 'ga:' . $subIndexBy;
      }

    }

    if ($details) {
      if ($type == 'pageviews' && (!in_array('ga:pagePath', $request['dimensions']))) {
        $request['dimensions'][] = 'ga:pagePath';
      }
      if (($type == 'entrances_valuedevents') || ($type == 'pageviews_valuedevents')) {
        $request['dimensions'][] = 'ga:eventCategory';
      }
      if (($type == 'entrances_goals') || ($type == 'pageviews_goals')) {

      }
    }

    $request['filters'] = $this->customFilters['filter'] . (($this->customFilters['filter'] && count($filters)) ? ';' : '') . implode(';', $filters);
    if (!empty($this->customFilters['segment'])) {
      $request['segment'] = $this->customFilters['segment'];
    }
    if (count($segments)) {
      $request['segment'] = (!empty($request['segment']) ? ';' : '') . implode(';', $segments);
    }
    return $request;
  }

  function addFeedData($feed, $type = '', $indexBy = '', $details = 0) {
    if (empty($feed->results) || !is_array($feed->results)) {
      return;
    }
    $settings = $this->requestSettings;

    $type = ($type) ? $type : $settings['type'];
    $subType = $settings['subType'];
    $indexBy = ($indexBy) ?  $indexBy : $settings['indexBy'];
    $subIndexBy = !empty($settings['subIndexBy']) ? $settings['subIndexBy'] : '';
    $details = ($details) ? $details : $settings['details'];

    // pagepathmap is a special case
    if ($type == 'pagepathmap_entrances') {
      $this->pagePathMap = $this->addPagePathMapFeedData($this->pagePathMap, $feed, $indexBy);
      return;
    }

    if (!isset($this->data[$indexBy])) {
      $this->data[$indexBy] = $this->initIndexDataStruc();
    }
    $prime_index = $indexBy;
    if ($prime_index == 'trafficsources') {
      $sub_indexes = $this->getTrafficsourcesSubIndexes();
    }
    else {
      $sub_indexes = array('-');
    }

    if (!$prime_index) {
      $prime_index = 'site';
    }

    foreach ($sub_indexes AS $sub_index) {
      if ($sub_index != '-') {
        $d = isset($this->data[$prime_index][$sub_index]) ? $this->data[$prime_index][$sub_index] : array();
        $curIndex = $sub_index;
        if (!isset($this->data[$prime_index][$sub_index])) {
          $this->data[$prime_index][$sub_index] = $this->initIndexDataStruc();
        }
      }
      else {
        if (!isset($this->data[$prime_index])) {
          continue;
        }
        $d = $this->data[$prime_index];
        $curIndex = $prime_index;
      }

      switch ($type) {
        case 'pageviews':
          $d = $this->addPageviewFeedData($d, $feed, $curIndex, $subIndexBy);
          break;
        case 'entrances':
          $d = $this->addEntranceFeedData($d, $feed, $curIndex, $subIndexBy);
          break;
        case 'pageviews_valuedevents':
          $d = $this->addValuedeventsFeedData($d, $feed, $curIndex, 'pageview');
          break;
        case 'entrances_valuedevents':
          $d = $this->addValuedeventsFeedData($d, $feed, $curIndex, 'entrance');
          break;
        case 'pageviews_goals':
          $d = $this->addGoalsFeedData($d, $feed, $curIndex, 'pageview', $details);
          break;
        case 'entrances_goals':
          $d = $this->addGoalsFeedData($d, $feed, $curIndex, 'entrance', $details);
          break;
        case 'pageviews_goals_assist':
          $d = $this->addGoalsAssistFeedData($d, $feed, $curIndex, 'pageview', $details);
          break;
        case 'eventsource_events':
          $d = $this->addEventsourceEventFeedData($d, $feed, $curIndex, 'pageview');
          break;
        case 'visit_info':
          $d = $this->addVisitInfoFeedData($d, $feed, $curIndex, 'entrance', $details);
          break;
        case 'pagepathmap_entrances':
          $d = $this->addPagePathMapFeedData($d, $feed, $curIndex, 'entrance');
          break;
      }
      if ($sub_index != '-') {
        $this->data[$prime_index][$sub_index] = $d;
      }
      else {
        $this->data[$prime_index] = $d;
      }
    }
  }

  function addPageviewFeedData($d, $feed, $indexBy, $subIndexBy) {
    if (!isset($d['_all']['pageview'])) {
      $d['_all']['pageview'] = $this->initPageviewDataStruc();
      $d['_totals']['pageview'] = $this->initPageviewDataStruc();
    }
    $this->_addPageviewFeedDataRow($d['_totals']['pageview'], $feed->totals);
    foreach($feed->results AS $row) {
      if (!$indexes = $this->initFeedIndex($row, $indexBy, $d, $pagePath)) {
        continue;
      }
      if (!is_array($indexes)) {
        $indexes = array($indexes);
      }
      foreach ($indexes AS $index) {
        if (!isset($d[$index]['pageview'])) {
          $d[$index]['pageview'] = $this->initPageviewDataStruc();
        }
      }
      $keys = $indexes;
      array_unshift($keys, '_all');
      //$keys = array('_all', $indexes);
      foreach ($keys AS $k) {
        $this->_addPageviewFeedDataRow($d[$k]['pageview'], $row);
      }
    }
    return $d;
  }

  function _addPageviewFeedDataRow(&$data, $row) {
    $data['recordCount']++;

    $data['pageviews'] += $row['pageviews'];
    $data['uniquePageviews'] += !empty($row['uniquePageviews']) ? $row['uniquePageviews'] : 0;
    //$data['pageviewsPerSession'] += ($row['pageviews'] * $row['pageviewsPerSession']);
    $data['timeOnPage'] += $row['timeOnPage'];
    $data['sticks'] += ($row['pageviews'] - $row['exits']);
    $data['goalValueAll'] += !empty($row['goalValueAll']) ? $row['goalValueAll'] : 0;
    $data['goalCompletionsAll'] += !empty($row['goalCompletionsAll']) ? $row['goalCompletionsAll'] : 0;

    // for queries with a single record per indexed entity, these numbers are exact
    // for queries with multiple records, this algo provides an estimated pageValueAll
    // using a weighted average across all records for the entity
    if (!empty($row['pageValue']) && !empty($row['uniquePageviews'])) {
      if ($data['uniquePageviews'] != 0) {
        $upv0 = $data['uniquePageviews'] - $row['uniquePageviews'];
        $data['pageValue'] =
          (($data['pageValue'] * $upv0) + ($row['pageValue'] * $row['uniquePageviews']))
          / ($upv0 + $row['uniquePageviews']);
      }
      //$data['pageValueAll'] = ($data['pageValue'] * $data['uniquePageviews']) / $data['recordCount'];
      $data['pageValueAll'] = ($data['pageValue'] * $data['uniquePageviews']); // / 2;
    }

    return $data;
  }

  function addEntranceFeedData($d, $feed, $indexBy, $subIndexBy = '') {
    if (!isset($d['_all']['entrance'])) {
      $d['_all']['entrance'] = $this->initEntranceDataStruc();
      $d['_totals']['entrance'] = $this->initEntranceDataStruc();
    }
    if ($subIndexBy && !isset($d['_all']['entrance'][$subIndexBy])) {
      $d['_all'][$subIndexBy] = $this->initEntranceDataStruc();
    }
    $this->_addEntranceFeedDataRow($d['_totals']['entrance'], $feed->totals);
    foreach($feed->results AS $row) {
      if (!$indexes = $this->initFeedIndex($row, $indexBy, $d)) {
        continue;
      }
      $subIndex = $this->initFeedIndex($row, $subIndexBy);
      if (!is_array($indexes)) {
        $indexes = array($indexes);
      }
      foreach ($indexes AS $index) {
        if (!isset($d[$index]['entrance'])) {
          $d[$index]['entrance'] = $this->initEntranceDataStruc();
        }
        if ($subIndexBy) {
          if (!isset($d[$index][$subIndexBy])) {
            $d[$index][$subIndexBy] = array('_all' => array());
            $d[$index][$subIndexBy]['_all']['entrance'] = $this->initEntranceDataStruc();
          }
          if ($subIndex && !isset($d[$index][$subIndexBy][$subIndex])) {
            $d[$index][$subIndexBy][$subIndex] = array('entrance' => $this->initEntranceDataStruc());
            $d[$index][$subIndexBy][$subIndex]['i'] = $subIndex;
          }
        }
      }

      $keys = $indexes;
      array_unshift($keys, '_all');

      $subkeys = array('_all');
      if ($subIndex) {
        $subkeys[] = $subIndex;
      }

      foreach ($keys AS $k) {
        if ($subIndexBy) {
          // only counts for _all once
          if ($k == '_all') {
            $this->_addEntranceFeedDataRow($d['_all'][$subIndexBy], $row);
          }
          else {
            foreach ($subkeys AS $sk) {
              $this->_addEntranceFeedDataRow($d[$k][$subIndexBy][$sk]['entrance'], $row);
            }
          }

        }
        else {
          $this->_addEntranceFeedDataRow($d[$k]['entrance'], $row);
        }
      }
    }
    return $d;
  }

  function _addEntranceFeedDataRow(&$data, $row) {
    $data['entrances'] += $row['entrances'];
    $data['newVisits'] += $row['newVisits'];
    //$data['pageviews'] += !empty($row['pageviews']) ? $row['pageviews'] : ($row['entrances'] * $row['pageviewsPerSession']);
    $data['pageviews'] += !empty($row['pageviews']) ? $row['pageviews'] : 0;
    $data['uniquePageviews'] += !empty($row['uniquePageviews']) ? $row['uniquePageviews'] : 0;
    //$data['pageviewsPerSession'] += ($row['entrances'] * $row['pageviewsPerSession']);
    $data['timeOnSite'] += $row['timeOnSite'];
    $data['sticks'] += ($row['entrances'] - $row['bounces']);
    $data['goalValueAll'] += !empty($row['goalValueAll']) ? $row['goalValueAll'] : 0;
    $data['goalCompletionsAll'] += !empty($row['goalCompletionsAll']) ? $row['goalCompletionsAll'] : 0;
    if (!empty($row['dimension4'])) {
      $data['timestamp'] = $row['dimension4'];
    }
    $data['recordCount']++;
    return $data;
  }

  function addValuedeventsFeedData($d, $feed, $indexBy, $mode = 'entrance') {
    if (!isset($d['_all'][$mode])) {
      $d['_all'][$mode] = $this->{'init' . $mode . 'DataStruc'}();
      $d['_totals'][$mode] = $this->{'init' . $mode . 'DataStruc'}();
    }
    $d['_totals'][$mode]['events']['_all'] = $this->_addValuedeventsFeedDataRow($d['_totals'][$mode]['events']['_all'], $feed->totals);
    foreach($feed->results AS $row) {
      if (!$indexes = $this->initFeedIndex($row, $indexBy, $d, $pagePath)) {
        continue;
      }

      if (!is_array($indexes)) {
        $indexes = array($indexes);
      }

      foreach ($indexes AS $index) {
        if (!isset($d[$index][$mode])) {
          $d[$index][$mode] = $this->{'init' . $mode . 'DataStruc'}();
        }
      }

      $keys = $indexes;
      array_unshift($keys, '_all');

      foreach ($keys AS $k) {
        if (isset($row['eventCategory'])) {
          $eventCateogry = $row['eventCategory'];
          if (!isset($d[$k][$mode]['events'][$eventCateogry ])) {
            $d[$k][$mode]['events'][$eventCateogry ] = $this->initEventsDataStruc();
            $d[$k][$mode]['events'][$eventCateogry]['i'] = $eventCateogry;
          }
          $d[$k][$mode]['events'][$eventCateogry] = $this->_addValuedeventsFeedDataRow($d[$k][$mode]['events'][$eventCateogry], $row);
        }
        $d[$k][$mode]['events']['_all'] = $this->_addValuedeventsFeedDataRow($d[$k][$mode]['events']['_all'], $row);
      }
    }
    return $d;
  }

  function _addValuedeventsFeedDataRow($data, $row) {
    $data['value'] += $row['eventValue'];
    $data['totalValuedEvents'] += $row['totalEvents'];
    $data['uniqueValuedEvents'] += $row['uniqueEvents'];
    return $data;
  }

  function addGoalsFeedData($d, $feed, $indexBy, $mode = 'entrance', $details) {
    if (!isset($d['_all'][$mode])) {
      $d['_all'][$mode] = $this->{'init' . $mode . 'DataStruc'}();
      $d['_totals'][$mode] = $this->{'init' . $mode . 'DataStruc'}();
    }
    foreach ($details AS $id) {
      $d['_totals'][$mode]['goals']['_all'] = $this->_addGoalsFeedDataRow($d['_totals'][$mode]['goals']['_all'], $feed->totals, $id);
    }
    foreach($feed->results AS $row) {
      if (!$indexes = $this->initFeedIndex($row, $indexBy, $d, $pagePath)) {
        continue;
      }

      if (!is_array($indexes)) {
        $indexes = array($indexes);
      }

      foreach ($indexes AS $index) {
        if (!isset($d[$index][$mode])) {
          $d[$index][$mode] = $this->{'init' . $mode . 'DataStruc'}();
        }
      }

      $keys = $indexes;
      array_unshift($keys, '_all');

      foreach ($keys AS $k) {
        foreach ($details AS $id) {
          if (!isset($d[$k][$mode]['goals']["n$id"])) {
            $d[$k][$mode]['goals']["n$id"] = $this->initGoalsDataStruc();
            $d[$k][$mode]['goals']["n$id"]['i'] = "n$id";
          }
          $d[$k][$mode]['goals']["n$id"] = $this->_addGoalsFeedDataRow($d[$k][$mode]['goals']["n$id"], $row, $id);
          $d[$k][$mode]['goals']['_all'] = $this->_addGoalsFeedDataRow($d[$k][$mode]['goals']['_all'], $row, $id);
        }
      }
    }
    return $d;
  }

  function _addGoalsFeedDataRow($data, $row, $id) {
    $data["completions"] += $row["goal{$id}Completions"];
    $data["value"] += $row["goal{$id}Value"];
    return $data;
  }

  function addGoalsAssistFeedData($d, $feed, $indexBy, $mode = 'entrance', $details = array()) {
    // TODO: incomplete. not used in any reports
    if (!isset($d['_all'][$mode])) {
      //$d['_all'][$mode] = $this->{'init' . $mode . 'DataStruc'}();
      //$d['_totals'][$mode] = $this->{'init' . $mode . 'DataStruc'}();
    }
    foreach ($details AS $id) {
      //$d['_totals'][$mode]['goals']['_all'] = $this->_addGoalsAssistFeedDataRow($d['_totals'][$mode]['goals']['_all'], $feed->totals, $id);
    }
    foreach($feed->results AS $row) {


      $indexes = array();

      foreach ($indexes AS $i => $index) {
        if (!isset($d[$index][$mode])) {
          $d[$index][$mode] = $this->{'init' . $mode . 'DataStruc'}();
        }
      }

      $keys = $indexes;
      array_unshift($keys, '_all');

      foreach ($keys AS $k) {
        foreach ($details AS $id) {
          if (!isset($d[$k][$mode]['goals']["n$id"])) {
            //$d[$k][$mode]['goals']["n$id"] = $this->initGoalsDataStruc();
            //$d[$k][$mode]['goals']["n$id"]['i'] = "n$id";
          }
          //$d[$k][$mode]['goals']["n$id"] = $this->_addGoalsFeedDataRow($d[$k][$mode]['goals']["n$id"], $row, $id);
          $d[$k][$mode]['goalsAssist']['_all'] = $this->_addGoalsAssistFeedDataRow($d[$k][$mode]['goals']['_all'], $row, $id);
        }
      }
    }
    return $d;
  }

  function _addGoalsAssistFeedDataRow($data, $row, $id) {
    $data["completions"] += $row["goal{$id}Completions"];
    $data["value"] += $row["goal{$id}Value"];
    return $data;
  }

  function addVisitInfoFeedData($d, $feed, $indexBy, $mode = 'entrance', $details) {
    $keyOps = array(
      'entrances' => 0,
      'goalValueAll' => 0,
      'dimension5' => 0,
    );
    foreach($feed->results AS $row) {
      $index = $this->initFeedIndex($row, $indexBy);
      if (!isset($d[$index]['visitinfo'])) {
        $d[$index]['visitinfo'] = array();
      }
      foreach ($row AS $key => $value) {
        if (!isset($keyOps[$key]) ||  $keyOps[$key]) {
          $d[$index]['visitinfo'][$key] = $value;
        }
      }
      if (isset($row['medium'])) {
        $d[$index]['visitinfo']['trafficcategory'] = $this->initFeedIndex($row, 'trafficcategory');
      }
      $d[$index]['visitinfo']['location'] = $this->initFeedIndex($row, 'location');
    }
    return $d;
  }

  function addEventsourceEventFeedData($d, $feed, $indexBy) {
    if (!isset($d['_all'])) {
      $d['_all'] = $this->initPageviewDataStruc();
      $d['_totals'] = $this->initPageviewDataStruc();
    }
    $d['_totals']['events']['_all'] = $this->_addEventsourceEventFeedDataRow($d['_totals'][$mode]['events']['_all'], $feed->totals);

    foreach($feed->results AS $row) {
      if (!$index = $this->initFeedIndex($row, $indexBy, $d)) {
        continue;
      }
      if (!isset($d[$index])) {
        $d[$index] = $this->initPageviewDataStruc();
      }
      if (!isset($d[$index]['title'])) {
        $d[$index]['title'] = $row['eventAction'];
        $d[$index]['url'] = $row['eventLabel'];
      }
      $keys = array('_all', $index);
      foreach ($keys AS $k) {
        if (isset($row['eventCategory'])) {
          $eventCateogry = $row['eventCategory'];
          if (!isset($d[$k]['events'][$eventCateogry])) {
            $d[$k]['events'][$eventCateogry ] = $this->initEventsDataStruc();
            $d[$k]['events'][$eventCateogry]['i'] = $eventCateogry;
          }
          $d[$k]['events'][$eventCateogry] = $this->_addEventsourceEventFeedDataRow($d[$k]['events'][$eventCateogry], $row);
        }
        $d[$k]['events']['_all'] = $this->_addEventsourceEventFeedDataRow($d[$k]['events']['_all'], $row);
      }
    }
    return $d;
  }

  function _addEventsourceEventFeedDataRow($data, $row) {
    $data['totalEvents'] += $row['totalEvents'];
    $data['uniqueEvents'] += $row['uniqueEvents'];
    if (substr($row['eventCategory'], -1) == '!') {
      $data['value'] += $row['eventValue'];
      $data['totalValuedEvents'] += $row['totalEvents'];
      $data['uniqueValuedEvents'] += $row['uniqueEvents'];
    }
    return $data;
  }

  function addPagePathMapFeedData($d, $feed, $indexBy) {
    $pathField = 'landingPagePath';
    $entityField = 'dimension1';
    foreach($feed->results AS $row) {
      if (!isset($d[$row[$pathField]])) {
        $d[$row[$pathField]] = array();
      }
      $d[$row[$pathField]][$row[$entityField]] = array();
      foreach ($row AS $key => $value) {
        if ($key == $pathField || $key == $entityField) {
          continue;
        }
        $d[$row[$pathField]][$row[$entityField]][$key] = $value;
      }
    }
    return $d;
  }

  function initFeedIndex(&$row, $indexBy, &$d = array(), &$pagePath = '') {
    if (!$indexBy) {
      return '';
    }
    $pagePath = !empty($row['pagePath']) ? $this->filterPagePath($row['pagePath']) : '';
    $landingPagePath = !empty($row['landingPagePath']) ? $this->filterPagePath($row['landingPagePath']) : '';
    $path = ($landingPagePath) ? $landingPagePath : $pagePath;
    if (!empty($this->dataIndexCallbacks[$indexBy])) {
      $func = $this->dataIndexCallbacks[$indexBy];
      $index = $func($row);
    }
    else if ($indexBy == 'content') {
      $index = (!empty($row['hostname']) ? $row['hostname'] : '') . $path;
    }
    else if ($indexBy == 'pagePath') {
      $index = (!empty($row['hostname']) ? $row['hostname'] : '') . $path;
    }
    else if ($indexBy == 'referralHostpath') {
      $index = ($row['referralPath'] != '(not set)') ? $row['source'] . $row['referralPath'] : FALSE;
    }
    else if ($indexBy == 'referralPath') {
      $index = ($row['referralPath'] != '(not set)') ? $row['referralPath'] : FALSE;
    }
    else if ($indexBy == 'socialNetwork') {
      $index = ($row['socialNetwork'] != '(not set)') ? $row['socialNetwork'] : FALSE;
    }
    else if ($indexBy == 'searchEngine') {
      if ($row['medium'] == 'organic') {
        $index = (!empty($row['source']) && ($row['source'] != '(not set)')) ? $row['source'] : FALSE;
      }
      else {
        $index = FALSE;
      }
    }
    else if ($indexBy == 'keyword') {
      $index = ($row['keyword'] != '(not set)') ? $row['keyword'] : FALSE;
    }
    else if (($indexBy == 'searchKeyword') || ($indexBy == 'organicSearch')) {
      if ($row['medium'] == 'organic') {
        $index = (!empty($row['keyword']) && ($row['keyword'] != '(not set)')) ? $row['keyword'] : FALSE;
      }
      else {
        $index = FALSE;
      }
    }
    else if ($indexBy == 'campaign') {
      $index = ($row['campaign'] != '(not set)') ? $row['campaign'] : FALSE;
    }
    else if ($indexBy == 'trafficcategory') {
      $index = $this->determineTrafficCategoryIndex($row);
    }
    else if ($indexBy == 'landingpage') {
      $index = $row['eventLabel'];
      // strip query string
      $a = explode('?', $index);
      // strip protocal
      $a = explode('//', $a[0]);
      $index = (count($a) == 2) ? $a[1] : $a[0];
    }
    else if ($indexBy == 'visitor') {
      $index = $row['dimension5'];
    }
    else if ($indexBy == 'visit') {
      $index = $row['dimension5'] . '-' . $row['visitCount'];
    }
    else if ($indexBy == 'location') {
      if (isset($row['city']) && isset($row['region'])) {
        $index = $row['city'] . ', ' . $row['region'] . (isset($row['country']) ? ' ' . $row['country'] : '');
      }
      else if (isset($row['country'])) {
        $index = $row['country'];
      }
    }
    /*
    else if ($indexBy == 'author') {
      $decoded = '';
      $data = $this->unserializeCustomVar($row['dimension1'], $decoded);
      $row['dimension1'] = $decoded;
      $index = $data['a'];
    }
    */
    else if (strpos($indexBy, 'pageAttribute:') === 0) {
      $decoded = '';
      $data = '';
      if (isset($row['dimension1'])) {
        $data = $this->unserializeCustomVar($row['dimension1'], $decoded);
      }
      // if dimension1 not in request, try looking for path in pagePathMap
      else {
        $v = '';
        if (isset($row['pagePath']) && isset($this->pagePathMap[$row['pagePath']])) {
          // get first key
          reset($this->pagePathMap[$row['pagePath']]);
          $v = key($this->pagePathMap[$row['pagePath']]);
        }
        else if (isset($row['landingPagePath']) && isset($this->pagePathMap[$row['landingPagePath']])) {
          // get first key
          reset($this->pagePathMap[$row['landingPagePath']]);
          $v = key($this->pagePathMap[$row['landingPagePath']]);
        }

        if ($v) {
          $data = $this->unserializeCustomVar($v, $decoded);
        }
      }

      $row['dimension1'] = $decoded;
      $indexBys = explode(':', $indexBy);
      $attr_key = $indexBys[1];

      if (($this->pageAttributeInfo[$attr_key]['type'] == 'scalar') || ($this->pageAttributeInfo[$attr_key]['type'] == 'item')) {
        $index = $data[$indexBys[1]];
      }
      else if ($this->pageAttributeInfo[$attr_key]['type'] == 'list') {
        $index = array();
        foreach ($data AS $key => $value) {
          if (strpos($key, $indexBys[1]) === 0) {
            $a = explode('.', $key);
            $index[] = $a[1];
            //$index[] = $key;
          }
        }
      }
      else {
        $index = $data[$indexBys[1]];
      }
    }
    else if ($indexBy == 'site') {
      return 'site';
    }
    else {
      $index = $row[$indexBy];
    }
    if ($index) {
      $indexes = is_array($index) ? $index : array($index);
      foreach ($indexes as $ind) {
        if (!isset($d[$ind])) {
          $d[$ind] = array();
          $d[$ind]['i'] = $ind;
        }
      }
    }


    return $index;
  }

  function determineTrafficCategoryIndex($row) {
    if (!empty($row['socialNetwork']) && ($row['socialNetwork'] != '(not set)')) {
      return 'social network';
    }
    if (!empty($row['hasSocialSourceReferral']) && ($row['hasSocialSourceReferral'] != 'Yes')) {
      return 'social network';
    }
    if ($row['medium'] == 'facebook') {
      return 'social network';
    }
    if ($row['medium'] == 'twitter') {
      return 'social network';
    }
    if ($row['medium'] == 'linkedin') {
      return 'social network';
    }
    if ($row['medium'] == '(none)') {
      return 'direct';
    }
    if ($row['medium'] == 'organic') {
      return 'organic search';
    }
    if ($row['medium'] == 'email') {
      return 'email';
    }
    if ($row['medium'] == 'referral') {
      return 'referral link';
    }
    if ($row['medium'] == 'feed') {
      return 'feed';
    }
    return 'other';
  }

  function getTrafficsourcesSubIndexes() {
    $s = array(
      'medium',
      'source',
      'referralHostpath',
      'searchKeyword',
      'socialNetwork',
      'campaign',
      'trafficcategory'
    );
    return $s;
  }

  function initIndexDataStruc() {
    $a = array(
      '_all' => array(),
      '_totals' => array(),
    );
    return $a;
  }

  function initEntranceDataStruc() {
    $a = array(
      'events' => $this->initEventsDataArrayStruc(),
      'goals' => $this->initGoalsDataArrayStruc(),
      'entrances' => 0,
      'newVisits' => 0,
      'pageviews' => 0,
      'uniquePageviews' => 0,
      //'pageviewsPerSession' => 0,
      'timeOnSite' => 0,
      'sticks' => 0,
      'goalValueAll' => 0,
      'goalCompletionsAll' => 0,
      'pageValue' => 0,
      'pageValueAll' => 0,
      'recordCount' => 0,
    );
    return $a;
  }

  function initPageviewDataStruc() {
    $a = array(
      'events' => $this->initEventsDataArrayStruc(),
      'goals' => $this->initGoalsDataArrayStruc(),
      'goalsAssist' => $this->initGoalsAssistDataArrayStruc(),
      'pageviews' => 0,
      'uniquePageviews' => 0,
      //'pageviewsPerSession' => 0,
      'timeOnPage' => 0,
      'sticks' => 0,
      'goalValueAll' => 0, // the goalValue directly generated by the entity
      'goalCompletionsAll' => 0,
      'pageValue' => 0,
      'pageValueAll' => 0, // the goalValue of any downstream goals (ie goal assists)
      'recordCount' => 0,
    );
    return $a;
  }

  function initEventsDataArrayStruc() {
    $a = array();
    $a['_all'] = $this->initEventsDataStruc();
    return $a;
  }

  function initEventsDataStruc() {
    $a = array(
      'value' => 0,
      'totalEvents' => 0,
      'uniqueEvents' => 0,
      'totalValuedEvents' => 0,
      'uniqueValuedEvents' => 0,
    );
    return $a;
  }

  function initGoalsDataArrayStruc() {
  $a = array();
  $a['_all'] = $this->initGoalsDataStruc();
  return $a;
}

  function initGoalsDataStruc() {
    $a = array(
      'value' => 0,
      'completions' => 0,
    );
    return $a;
  }

  function initGoalsAssistDataArrayStruc() {
    $a = array();
    $a['_all'] = $this->initGoalsAssistDataStruc();
    return $a;
  }

  function initGoalsAssistDataStruc() {
    $a = array(
      'prev1Value' => 0,
      'prev1Completions' => 0,
      'prev2Value' => 0,
      'prev2Completions' => 0,
      'prev3Value' => 0,
      'prev3Completions' => 0,
    );
    return $a;
  }

  function formatPathFilter($path, $type = '', $bestMatch = FALSE) {
    $f = (($type == 'landingpage') || ($type == 'landingPagePath')) ? 'landingPagePath' : 'pagePath';
    $path = urldecode($path);
    if ($bestMatch) {
      $filter = 'ga:' . $f . '=~' . preg_quote(substr($path, -100)) . '(#.*)?$';
      return $filter;
    }
    if (strlen($path) <= 120) {
      $filter = 'ga:' . $f . '=~^' . preg_quote($path) . '(#.*)?$';
    }
    else {
      $filter = 'ga:' . $f . '=@' . $path . ';ga:' . $f . '=~' . preg_quote(substr($path, -100)) . '(#.*)?$';
    }
    return $filter;
  }

  function unserializeCustomVar($str, $decoded_str = '') {
    $decoded_str = html_entity_decode($str);
    $a = explode("&", $decoded_str);
    $data = array();
    foreach ($a AS $i => $e) {
      $kv = explode("=", $e);
      if (empty($kv[0])) {
        continue;
      }
      if (count($kv) == 2) {
        $data[$kv[0]] = $kv[1];
      }
      else {
        $data[$kv[0]] = '';
      }
    }
    return $data;
  }

  function filterPagePath($path) {
    $path = html_entity_decode($path);
    $filter_queries = array(
      'sid',  // webform's submission id
      'submissionGuid', // HubSpot's form ids
      'hsCtaTracking', // HubSpot CTA tracking
      '_hsenc',  // HubSpot code
      '_hsmi', // HubSpot code
      '__hstc',
      '__hssc',
    );
    $a = explode('?', $path);
    $query = '';
    if (!empty($a[1])) {
      $b = explode('&', $a[1]);
      foreach ($b AS $c) {
        $d = explode('=', $c);
        if (!in_array($d[0], $filter_queries)) {
          $query .= (($query) ? '&' : '') . $c;
        }
      }
    }
    if ($query) {
      $path = $a[0] . "?$query";
    }
    else {
      $path = $a[0];
    }
    return $path;
  }

  static function formatGtRegexFilter($param, $number, $key = '') {
    $k = ($key) ? "&$key=" : "^";
    $end = ($key) ? '&' : "$";
    $nstr = (string)$number;
    $num_arr = str_split($nstr);
    $digits = count($num_arr);
    $regex = $param . '=~' . $k . '\d{' . ($digits+1) . '\,}' . $end;
    $p = '';
    foreach ($num_arr AS $i => $digit) {
      $digit = (int)$digit;
      if ($digit < 9) {
        $regex .= ',' . $param .  '=~' . $k . $p;
        if ((1 + $digit) == 9) {
          $regex .= '9';
        }
        else {
          $regex .= '[' . (1 + $digit) . '-9]';
        }
        if ($i < ($digits - 1)) {
          $regex .= '\d{' . ($digits - 1 - $i) . '}' . $end;
        }
        else {
          $regex .= $end;
        }
      }
      $p .= (string)$digit;
    }
    return $regex;
  }

  static function formatNltRegexFilter($param, $number) {
    $nstr = (string)$number;
    $num_arr = str_split($nstr);
    $digits = count($num_arr);
    $regex = $param . '!~^\d{0\,' . ($digits-1) . '}$';
    $p = '';
    foreach ($num_arr AS $i => $digit) {
      $digit = (int)$digit;
      if ($digit > 0) {
        $regex .= ';' . $param .  '!~^' . $p;
        if (($digit - 1) == 0) {
          $regex .= '0';
        }
        else {
          $regex .= '[0-' . ($digit - 1) . ']';
        }
        if ($i < ($digits - 1)) {
          $regex .= '\d{' . ($digits - 1 - $i) . '}$';
        }
        else {
          $regex .= '$';
        }
      }
      $p .= (string)$digit;
    }
    return $regex;
  }

}