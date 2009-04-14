<?php
// $Id$

class ContentLicense {
  private $id;
  private $type;
  private $licenses;

  private static $availableLicenses;

  public function __construct($id, $type='node') {
    $this->id = $id;
    $this->type = $type;
  }

  public function reset() {
    $this->licenses = NULL;
  }

  public function applies($lid) {
    return $this->getLicenses($lid) != NULL;
  }

  public function unlicensed() {
    $this->getLicenses();
    return empty($this->licenses);
  }

  public function getLicenses($lid=NULL) {
    if (!$this->licenses) {
      $this->licenses = array();
      $res = db_query("SELECT *
        FROM {content_license_licensed}
        WHERE content_id = %d
          AND content_type='%s'", array(
        ':content_id' => $this->id,
        ':content_type' => $this->type,
      ));
      while ($obj = db_fetch_object($res)) {
        $this->licenses[$obj->lid] = $obj;
      }
    }

    if ($lid) {
      if (isset($this->licenses[$lid])) {
        return $this->licenses[$lid];
      }
      return NULL;
    }
    return $this->licenses;
  }

  public function addLicense($lid) {
    if (!self::load($lid)) {
      throw new Exception(t('Unknown license type'));
    }

    if (!$this->getLicenses($lid)) {
      $this->licenses[$lid] = TRUE;
      db_query("INSERT INTO {content_license_licensed}(lid, content_id, content_type)
        VALUES(%d, %d, '%s')", array(
          ':lid' => $lid,
          ':content_id' => $this->id,
          ':content_type' => $this->type,
      ));
    }
  }

  public function removeLicense($lid) {
    db_query("DELETE FROM {content_license_licensed}
      WHERE lid = %d AND content_id = %d AND content_type = '%s'", array(
      ':lid' => $lid,
      ':content_id' => $this->id,
      ':content_type' => $this->type,
    ));
  }

  public static function load($lid=NULL) {
    if (!self::$availableLicenses) {
      self::$availableLicenses = array();
      $res = db_query("SELECT * FROM {content_license}");
      while ($obj = db_fetch_object($res)) {
        self::$availableLicenses[$obj->lid] = $obj;
      }
    }

    if ($lid) {
      if (isset(self::$availableLicenses[$lid])) {
        return self::$availableLicenses[$lid];
      }
      return NULL;
    }
    return self::$availableLicenses;
  }

  public static function write($values) {
    $values = (array)$values;
    $update = isset($values['lid']) ? array('lid') : NULL;
    drupal_write_record('content_license', $values, $update);
  }
}