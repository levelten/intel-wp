<?php
/**
 * @file
 * Functions to support extended Google Analytics data.
 * 
 * @author Tom McCracken <tomm@getlevelten.com>
 */

function intel_annotation_period_options() {
  $options = array(
    '1' => Intel_Df::t('1 hr'),
    '2' => Intel_Df::t('2 hr'),
    '4' => Intel_Df::t('4 hr'),
    '8' => Intel_Df::t('8 hr'),
    '24' => Intel_Df::t('24 hrs'),
    '48' => Intel_Df::t('2 days'),
    '96' => Intel_Df::t('4 days'),
    '168' => Intel_Df::t('1 week'),
    '336' => Intel_Df::t('2 weeks'),
    '672' => Intel_Df::t('4 weeks'),
    '2184' => Intel_Df::t('1 Quarter'),
  );

  return $options;
}