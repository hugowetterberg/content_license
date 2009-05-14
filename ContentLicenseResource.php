<?php
// $Id$

/**
 * Class that defines the content license resource
 */
class ContentLicenseResource {
  /**
   * Creates a license
   *
   * @param object $license ["data"]
   * @return object
   *
   * @Access(callback='user_access', args={'create content licenses'}, appendArgs=false)
   */
  public static function create($license) {
    $res = array();
    $res['lid'] = ContentLicense::write($license);
    $res['uri'] = services_resource_uri(array('content_license', $res['lid']));
    return (object)$res;
  }

  /**
   * Retrieves a license
   *
   * @param int $lid ["path","0"]
   *  The id of the license to get
   * @return object
   *
   * @Access(callback='user_access', args={'access content'}, appendArgs=false)
   */
  public static function retrieve($lid) {
    return ContentLicense::load($lid);
  }

  /**
   * Updates a license
   *
   * @param int $lid ["path","0"]
   *  The id of the license to update
   * @param object $license ["data"]
   *  The license object
   * @return object
   *
   * @Access(callback='user_access', args={'update content licenses'}, appendArgs=false)
   */
  public static function update($lid, $license) {
    $res = array();
    $event['lid'] = $lid;
    $res['lid'] = ContentLicense::write($license);
    $res['uri'] = services_resource_uri(array('content_license', $res['lid']));
    return (object)$res;
  }

  /**
   * Deletes a license
   *
   * @param int $lid ["path","0"]
   *  The id of the license to delete
   * @return bool
   *
   * @Access(callback='user_access', args={'delete content licenses'}, appendArgs=false)
   */
  public static function delete($lid) {
    ContentLicense::delete($lid);
  }

  /**
   * Retrieves a listing of content licenses
   * 
   * @return array
   *
   * @Access(callback='user_access', args={'access content'}, appendArgs=false)
   */
  public static function index() {
    return array_values(ContentLicense::load());
  }
}