<?php

/**
 * @file
 *  This file contains the implementation of all functions that only are called occasionally due
 *  to caching or infrequent use. Implementations of hooks are prefixed with _ to not collide with the
 *  hook function.
 */

/**
 * Implementation of hook_menu().
 */
function _content_license_menu() {
  $menu = array();

  $menu['admin/content/licenses'] = array(
    'title' => t('Content licenses'),
    'file' => 'content_license.admin.inc',
    'description' => t('Administer the available content licenses'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('_content_license_settings'),
    'access arguments' => array('administer nodes'),
  );

  $menu['admin/content/licenses/add'] = array(
    'title' => t('Add content license'),
    'file' => 'content_license.admin.inc',
    'description' => t('Create a content license'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('_content_license_add'),
    'access arguments' => array('administer nodes'),
    'type' => MENU_CALLBACK,
  );

  $menu['admin/content/licenses/edit/%license'] = array(
    'title' => t('Add content license'),
    'file' => 'content_license.admin.inc',
    'description' => t('Create a content license'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('_content_license_edit', 4),
    'access arguments' => array('administer nodes'),
    'type' => MENU_CALLBACK,
  );

  $menu['admin/content/licenses/delete/%license'] = array(
    'title' => t('Delete content license'),
    'file' => 'content_license.admin.inc',
    'description' => t('Delete a content license'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('_content_license_delete', 4),
    'access arguments' => array('administer nodes'),
    'type' => MENU_CALLBACK,
  );

  $menu['content-license/info'] = array(
    'title' => t('License information'),
    'description' => t('Create a content license'),
    'page callback' => '_content_license_info',
    'access arguments' => array('access content'),
    'type' => MENU_CALLBACK,
  );

  return $menu;
}

/**
 * Implementation of hook_theme().
 */
function _content_license_theme() {
  $theme = array();

  $theme['license_info_page'] = array(
    'template' => 'license_info_page',
    'arguments' => array('licenses' => array()),
  );

  $theme['license_info'] = array(
    'template' => 'license_info',
    'arguments' => array('license' => NULL),
  );

  $theme['node_license_info'] = array(
    'template' => 'node_license_info',
    'arguments' => array('element' => array()),
  );

  $theme['license_radios'] = array(
    'template' => 'license_options',
    'arguments' => array('form' => NULL, 'widget' => 'radio'),
  );

  $theme['license_checkboxes'] = array(
    'template' => 'license_options',
    'arguments' => array('form' => NULL, 'widget' => 'checkbox'),
  );

  return $theme;
}

function _content_license_node_form_alter(&$form) {
  $selector = variable_get('content_license_selector', 'full');
  $multiple = variable_get('content_license_mode', 'multi') == 'multi';
  $form['license'] = array(
    '#title' => t('Content license'),
    '#licenses' => ContentLicense::load(),
  );

  switch ($selector) {
    case 'full':
      $form['license']['#type'] = $multiple ? 'checkboxes' : 'radios';
      $form['license']['#theme'] = $multiple ? 'license_checkboxes' : 'license_radios';
      $form['license']['#after_build'] = array('_content_license_form_support');
      break;
    case 'compact':
      $form['license']['#type'] = 'select';
      if ($multiple) {
        $form['license']['#multiple'] = TRUE;
        $form['license']['#size'] = count($form['license']['#licenses']);
      }

      $form['license']['#description'] = l(t('Click here to read more about the different licenses'), 'content-license/info', array(
        'attributes' => array(
          'target' => '_blank',
        ),
      ));
      break;
  }

  if (!$multiple) {
    $form['license']['#options'][''] = t('None (all rights reserved)');
  }

  $licenses = new ContentLicense($form['#node']->nid);
  foreach ($form['license']['#licenses'] as $lid => $license) {
    $form['license']['#options'][$lid] = $license->license_title;
    if ($licenses->applies($lid)) {
      $form['license']['#default_value'][] = $lid;
    }
  }

  if (!$multiple && $licenses->unlicensed()) {
    $form['license']['#default_value'] = array('none');
  }
}

/**
 * Implementation of hook_nodeapi().
 **/
function _content_license_nodeapi(&$node, $op, $a3=Null, $page=Null) {
  if (!is_array($node->license)) {
    $node->license = array($node->license => $node->license);
  }

  switch ($op) {
    // The node is being deleted.
    case 'delete':
    break;
    // The node is being created (inserted in the database)
    case 'insert':
      $license = new ContentLicense($node->nid);
      foreach ($node->license as $lid => $lidOn) {
        if ($lidOn) {
          $license->addLicense($lid);
        }
      }
    break;
    // The node is being updated.
    case 'update':
      $license = new ContentLicense($node->nid);
      $licenses = array();
      foreach ($node->license as $lid => $lidOn) {
        if ($lidOn) {
          $licenses[] = $lid;
        }
      }
      $license->set($licenses);
    break;
    // The node is about to be loaded from the database. This hook can be used to load additional data at this time.
    case 'load':
      $licenses = new ContentLicense($node->nid);
      $node->license = $licenses->getLicenses();
    break;
    // The node is about to be shown on the add/edit form.
    case 'prepare':
    break;
    case 'prepare translation':
      $node->license = $node->translation_source->license;
    break;
    case 'rss item':
    break;
    // The node content is being assembled before rendering. The module may add elements $node->content prior to rendering.
    // This hook will be called after hook_view(). The format of $node->content is the same as used by Forms API.
    case 'view':
      if ($node->license) {
        if (($a3 && variable_get('content_license_show_in_teaser', FALSE)) ||
            (!$a3 && variable_get('content_license_show_in_full_node', FALSE))) {
          $node->content['license']['#theme'] = 'node_license_info';
          foreach ($node->license as $lid => $lidOn) {
            if ($lidOn) {
              $node->content['license'][$lid] = array(
                '#license' => ContentLicense::load($lid),
              );
            }
          }
        }
      }
    break;
  }
}