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
class Intel_Entity_Controller {

	/**
	 * Define the core functionality of the plugin.
	 *
	 * Set the plugin name and the plugin version that can be used throughout the plugin.
	 * Load the dependencies, define the locale, and set the hooks for the admin area and
	 * the public-facing side of the site.
	 *
	 * @since    1.0.0
	 */

	public $entity_type;
	public $entity_class;
	public $entity_info;
	public $base_table;
	public $key_id;
	public $fields;


	public function __construct($entityType, $entity_info) {
		$this->entity_type = $entityType;
		$this->entity_info = $entity_info;
		$this->entity_class = !empty($entity_info['entity class']) ? $entity_info['entity class'] : 'Intel_Entity';
		$this->base_table = $entity_info['base table'];

		$this->fields = $entity_info['fields'];
		$this->key_id = $entity_info['entity keys']['id'];
	}

	public function create(array $values = array()) {
	  intel_d($values);
		if (class_exists($this->entity_class)) {
			$entity = new $this->entity_class($values, $this);
		}
		else {
			$entity = new Intel_Entity($values, $this);
		}
		$entity->is_new = 1;
		return $entity;
	}

	public function get_fields() {
		return $this->fields;
	}

	public function get_key_id() {
		return $this->key_id;
	}

	public function save($entity) {
		global $wpdb;

		$data = array();
		$format = array();
		$data = $this->fields;

		if (empty($entity->{$this->key_id})) {
			array_shift($data);
		}
		$i = 0;
		foreach ($data as $k => $v) {
			$data[$k] = isset($entity->{$k}) ? $entity->{$k} : $v;
			$format[$i] = '%s';
			if (is_array($v) || is_object($v)) {
				$data[$k] = serialize($data[$k]);
			}
			elseif(is_integer($v)) {
				$format[$i] = '%d';
			}
			elseif (is_float($v)) {
				$format[$i] = '%f';
			}
			$i++;
		}

		if (empty($data[$this->key_id])) {
			$wpdb->insert($wpdb->prefix . $this->base_table, $data, $format );
			$entity->{$this->key_id} = $wpdb->insert_id;
		}
		else {
			$wpdb->replace($wpdb->prefix . $this->base_table, $data, $format);
		}
		return $entity;
	}

	public function load($ids, $conditions = array(), $reset = FALSE) {
		global $wpdb;

		if (!is_array($ids)) {
			$ids = array($ids);
		}
		$ids_query = implode(', ', $ids);
		$data = $ids;
		$id_cnt = count($ids);
		$id_placeholder = is_numeric($ids[0]) ? '%d' : '%s';
		$id_placeholders = array_fill(0, $id_cnt, $id_placeholder);
		$ids_query = implode(', ', $id_placeholders);

		$sql = "
		  SELECT *
		  FROM {$wpdb->prefix}{$this->base_table}
		  WHERE {$this->key_id} IN ( $ids_query )
		";

		$results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

		if (empty($results[0])) {
			return FALSE;
		}
		$entities = array();
		foreach ($results as  $row) {
			$entity = new $this->entity_class((array)$row, $this);
			$entities[$entity->id] = $entity;
		}
		return $entities;
	}

	public function loadOne($id, $conditions = array(), $reset = FALSE) {
		$entities = self::load(array($id), $conditions, $reset);
		if (empty($entities)) {
			return FALSE;
		}
		return array_shift($entities);
	}

	public function loadByVars($vars, $select_options = array(), $construct_entity = 1) {
		global $wpdb;

		$sql = "
		  SELECT *
		  FROM {$wpdb->prefix}{$this->base_table}
		";
		if (!empty($vars)) {
			$sql .= "\nWHERE\n";
			$where_cnt = 0;
			$data = array();
			foreach ($vars as $k => $v) {
				if ($where_cnt) {
					$sql .= ' AND ';
				}
				$where_cnt++;
				if (is_array($v)) {
					if (count($v) == 3) {
						$d0 = $d1 = (is_string($v[1])) ? '"' : '';
						$sql .= $v[0] . ' ' . $v[2] . ' ';
						if (strtolower($v[2]) == 'in') {
							if (!is_array($v[1])) {
								$v[1] = array($v[1]);
							}
							$placeholder = is_numeric($v[1]) ? '%d' : '%s';
							$placeholders = array_fill(0, count($v[1]), $placeholder);
							$vars_query = implode(', ', $placeholders);
							$sql .= "( $vars_query )";
							$data = array_merge($data, $v[1]);
						}
						else {
							$sql .= ((is_string($v[1])) ? '%s' : '%d');
						  $data[] = $v[1];
						}
					}
					elseif (count($v) == 2) {
						$sql .= $v[0] . ' = ' . ((is_string($v[1])) ? '%s' : '%d') . "\n";
						$data[] = $v[1];
					}
				}
				else {
					$sql .= "$k = " . ((is_string($v)) ? '%s' : '%d') . "\n";
					$data[] = $v;
				}
			}
		}

		$results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

		if (empty($results[0])) {
			return FALSE;
		}
		if (!$construct_entity) {
			return $results;
		}

		$entities = array();
		foreach ($results as $row) {
			$entity = new $this->entity_class((array)$row, $this);
			$entities[$row->{$this->key_id}] = $entity;
		}
		return $entities;
	}

	public function processWhere($vars, $data = array()) {
	  $sql = '';
    if (!empty($vars)) {
      $where_cnt = 0;
      $data = array();
      foreach ($vars as $k => $v) {
        if ($where_cnt) {
          $sql .= ' AND ';
        }
        $where_cnt++;
        if (is_array($v)) {
          if (count($v) == 3) {
            $d0 = $d1 = (is_string($v[1])) ? '"' : '';
            $sql .= $v[0] . ' ' . $v[2] . ' ';
            if (strtolower($v[2]) == 'in') {
              if (!is_array($v[1])) {
                $v[1] = array($v[1]);
              }
              $placeholder = is_numeric($v[1]) ? '%d' : '%s';
              $placeholders = array_fill(0, count($v[1]), $placeholder);
              $vars_query = implode(', ', $placeholders);
              $sql .= "( $vars_query )";
              $data = array_merge($data, $v[1]);
            }
            else {
              $sql .= ((is_string($v[1])) ? '%s' : '%d');
              $data[] = $v[1];
            }
          }
          elseif (count($v) == 2) {
            $sql .= $v[0] . ' = ' . ((is_string($v[1])) ? '%s' : '%d') . "\n";
            $data[] = $v[1];
          }
        }
        else {
          $sql .= "$k = " . ((is_string($v)) ? '%s' : '%d') . "\n";
          $data[] = $v;
        }
      }
    }

	  return $sql;
  }

  public function loadByFilter($filter = array(), $options = array(), $header = array(), $limit = 100, $offset = NULL) {
	  /*
    $query = db_select('intel_submission', 's')
      ->extend('PagerDefault')
      ->limit($limit);
    $v = $query->leftJoin('intel_visitor', 'v', '%alias.vid = s.vid');
    $query->fields('s');
    $query->addField($v, 'name', 'name');
    //$query->addField($v, 'email', 'email');
    //$query->addField($v, 'vtkid', 'vtkid');
    $query->addField($v, 'data', 'visitor_data');
    $query->addField($v, 'ext_data', 'visitor_ext_data');
    $query->addTag('intel_submission_load_filtered');

    if (!empty($header)) {
      $query->extend('TableSort')->orderByHeader($header);
    }
	  */

    global $wpdb;

    $sql = "
		  SELECT *
		  FROM {$wpdb->prefix}{$this->base_table}
		";
    $data = array();

    if (!empty($filter['where'])) {
      $where = $this->processWhere($filter['where'], $data);
      if ($where) {
        $sql .= "\n";
        $sql .= "WHERE\n";
        $sql .= $where;
      }
    }
    if (!empty($filter['conditions'])) {
      foreach ($filter['conditions'] AS $condition) {
        if (count($condition) == 3) {
          $query->condition($condition[0], $condition[1], $condition[2]);
        }
        else {
          $query->condition($condition[0], $condition[1]);
        }
      }
    }

    if ($options['order_by']) {
      $sql .= "\n";
      $sql .= "ORDER BY {$options['order_by']}\n";
    }

    $sql .= "\n";
    $sql .= "LIMIT %d";
    $data[] = $limit;
    if ($offset) {
      $sql .= " OFFSET %d";
      $data[] = $offset;
    }

    $results = $wpdb->get_results( $wpdb->prepare($sql, $data) );

    return $results;
  }

	public function deleteOne($id) {
		global $wpdb;
		$wpdb->delete( $wpdb->prefix . $this->base_table, array( $this->key_id => $id ) );
  }

	public function delete($ids) {
		if (!is_array($ids)) {
			$ids = array($ids);
		}
		foreach ($ids as $id) {
			self::deleteOne($id);
		}
	}

	public static function syncData($entity, $options = array()) {
		if (!empty($_GET['debug'])) {
			intel_d('entity0');//
			intel_d($entity);//
		}

		$entity_type = $entity->entity_type;
		if (strpos($entity_type, 'intel_') === 0) {
			$entity_type = substr($entity_type, 6);
		}

		// initial data gathering stage
		$entity = apply_filters('intel_sync_' . $entity_type, $entity, $options);

		// alter initial data gathering
		$entity = apply_filters('intel_sync_' . $entity_type . '_alter', $entity, $options);

		// data presave stage
		$entity = apply_filters('intel_sync_' . $entity_type . '_presave', $entity, $options);

		$statuses = $entity->getSyncProcessStatus();
		$synced = intel()->time();
		foreach ($statuses as $k => $v) {
			if (!$v) {
				$synced = 0;
				break;
			}
		}
		$entity->setSynced($synced);

		$entity->save();

		// data save stage
		do_action('intel_sync_' . $entity_type . '_save', $entity, $options);

		return $entity;
	}

	public static function get_class_from_entity_type($entity_type) {
		return str_replace(' ', '_', (ucwords( str_replace('_', ' ', $entity_type) ) ));
	}
}
