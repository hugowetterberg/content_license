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

  /**
   * Resets the license cache to force reloading form db.
   */
  public function reset() {
    $this->licenses = NULL;
  }

  /**
   * Checks if the given license applies to the content.
   *
   * @param int $lid
   *  The id of the license to check for
   *
   * @return bool
   */
  public function applies($lid) {
    return $this->getLicenses($lid) != NULL;
  }

  /**
   * Checks if the content doesn't have a license attached to it.
   * This usually means that all rights are reserved.
   *
   * @return bool
   *  TRUE if the content doesn't have a license, otherwise FALSE is returned.
   */
  public function unlicensed() {
    $this->getLicenses();
    return empty($this->licenses);
  }

  /**
   * Sets the license of the content to a specific set of none or more licenses.
   * Any licenses that are not already set will be added and any licences that are set
   * but not included in $licenses will be removed.
   *
   * @param array $licenses
   *  An array with the ids of the licenses that should be used for the content.
   * @return void
   */
  public function set($licenses) {
    $current = $this->getLicenses();

    // Find licenses that should be added
    foreach ($licenses as $license) {
      if (!isset($current[$license])) {
        $this->addLicense($license);
      }
      else {
        unset($current[$license]);
      }
    }

    // Removed unused licenses
    foreach ($current as $lid => $used) {
      $this->removeLicense($lid);
    }
  }

  /**
   * Gets all licenses for the content. If a $lid is provided only the specified license is returned.
   *
   * @param string $lid
   *  Optional. The id of the license to get.
   * @return array|object
   *  An array of license objects or a single object if a specific license was requested. NULL will be returned if
   *  the requested license doesn't apply to the content.
   */
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

  /**
   * Adds a license to the content.
   *
   * @param int $lid
   *  The id of the license to add.
   * @return void
   */
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

  /**
   * Removes a license from the content.
   *
   * @param int $lid
   *  The id of the license to remove.
   * @return void
   */
  public function removeLicense($lid) {
    if ($this->getLicenses($lid)) {
      unset($this->licenses[$lid]);
      db_query("DELETE FROM {content_license_licensed}
        WHERE lid = %d AND content_id = %d AND content_type = '%s'", array(
        ':lid' => $lid,
        ':content_id' => $this->id,
        ':content_type' => $this->type,
      ));
    }
  }

  /**
   * Removes all licenses from the content.
   *
   * @return void
   */
  public function removeAllLicenses() {
    $this->licenses = array();
    db_query("DELETE FROM {content_license_licensed}
      WHERE content_id = %d AND content_type = '%s'", array(
      ':content_id' => $this->id,
      ':content_type' => $this->type,
    ));
  }

  /**
   * Gets license information.
   *
   * @param int $lid
   *  Optional. The id of the license to get.
   * @return array|object
   *  An array of license objects or a single object if a specific license was requested. NULL will be returned if
   *  the requested license doesn't exist.
   */
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

  /**
   * Saves a license to the database.
   *
   * @param array $values
   *  The properties of the license as an array.
   * @return int
   *  The id of the license that was written to the database
   */
  public static function write($values) {
    $values = (array)$values;
    $update = isset($values['lid']) ? array('lid') : NULL;
    drupal_write_record('content_license', $values, $update);
    self::$availableLicenses[$values['lid']] = (object)$values;
    return $values['lid'];
  }
}