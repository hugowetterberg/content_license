<?php
// $Id$

function _content_license_settings($form_state) {
  $form = array(
    '#id' => 'content-license-settings',
  );

  drupal_add_css(drupal_get_path('module', 'content_license') . '/content_license.admin.css');

  $licenses = ContentLicense::load();
  $form['licenses'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="licenses"><h3>' . t('Licenses') . '</h3>',
    '#suffix' => '</div>',
  );

  foreach ($licenses as $license) {
    $form['licenses'][$license->lid] = array(
      '#type' => 'item',
      '#title' => $license->license_title,
      '#value' => '<p>' . $license->license_description . '</p>' . theme('links', array(
        array('title' => t('Edit'), 'href' => 'admin/content/licenses/edit/' . $license->lid),
        array('title' => t('Delete'), 'href' => 'admin/content/licenses/delete/' . $license->lid),
      )),
    );
  }

  $form['licenses']['add_license'] = array(
    '#type' => 'item',
    '#value' => l(empty($licenses) ? t('No licenses have been added, click here to add one') : t('Add license'), 'admin/content/licenses/add'),
  );

  $form['settings'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="settings"><h3>' . t('Settings') . '</h3>',
    '#suffix' => '</div>',
  );

  $form['settings']['mode'] = array(
    '#type' => 'radios',
    '#title' => t('Licensing mode'),
    '#options' => array(
      'single' => t('Only single licenses'),
      'multi' => t('Allow multiple licenses'),
    ),
    '#default_value' => variable_get('content_license_mode', 'multi'),
  );

  $form['settings']['selector'] = array(
    '#type' => 'radios',
    '#title' => t('License selection widget'),
    '#options' => array(
      'compact' => t('Compact selector'),
      'full' => t('Full license information listing'),
    ),
    '#default_value' => variable_get('content_license_selector', 'full'),
  );

  $show_in = array();
  if (variable_get('content_license_show_in_teaser', FALSE)) {
    $show_in[] = 'teaser';
  }
  if (variable_get('content_license_show_in_full_node', FALSE)) {
    $show_in[] = 'full';
  }

  $form['settings']['show_license_info'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Show license information in'),
    '#options' => array(
      'teaser' => t('Teaser'),
      'full' => t('Full node'),
    ),
    '#default_value' => $show_in,
  );

  $node_types = node_get_types();
  $type_options = array();
  $type_default = array();
  foreach ($node_types as $type) {
    $type_options[$type->type] = $type->name;
    if (variable_get('content_license_node_' . $type->type, FALSE)) {
      $type_default[] = $type->type;
    }
  }

  $form['settings']['types'] = array(
    '#title' => t('Enable licensing options for'),
    '#type' => 'checkboxes',
    '#options' => $type_options,
    '#default_value' => $type_default,
  );

  $form['settings']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<div class="footer">',
    '#suffix' => '</div>',
  );

  return $form;
}

function _content_license_settings_submit($form, $form_state) {
  $values = $form_state['values'];
  variable_set('content_license_mode', $values['mode']);
  variable_set('content_license_selector', $values['selector']);
  foreach ($values['types'] as $type => $enabled) {
    variable_set('content_license_node_' . $type, $enabled !== 0);
  }
  foreach ($values['show_license_info'] as $type => $enabled) {
    if ($type=='teaser') {
      variable_set('content_license_show_in_teaser', $enabled !== 0);
    }
    else if ($type=='full') {
      variable_set('content_license_show_in_full_node', $enabled !== 0);
    }
  }
}

function _content_license_add($form_state) {
  return _content_license_form();
}

function _content_license_add_submit($form, $form_state) {
  ContentLicense::write($form_state['values']);
  drupal_goto('admin/content/licenses');
}

function _content_license_edit($form_state, $license) {
  return _content_license_form($license);
}

function _content_license_edit_submit($form, $form_state) {
  ContentLicense::write($form_state['values']);
  drupal_goto('admin/content/licenses');
}

function _content_license_delete($form_state, $license) {
  drupal_set_title(t('Deleting "@title"', array('@title' => $license->license_title)));
  $form = array(
    '#license' => $license,
  );

  $form['text'] = array(
    '#value' => t('Are you sure that you want to delete the license @title?', array(
      '@title' => $license->license_title)
    ),
  );

  $form['warning'] = array(
    '#value' => '<p><strong>' . t('WARNING: If there\'s content on this site owned by someone other than yourself that uses this license you have to make sure that it\'s actually within your legal rights to delete this license.') . '</strong></p>',
  );

  $form['cancel'] = array(
    '#type' => 'item',
    '#value' => l(t('No, take me back'), 'admin/content/licenses'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Yes, delete the license')
  );

  return $form;
}

function _content_license_delete_submit($form, $form_state) {
  ContentLicense::delete($form['#license']->lid);
  drupal_goto('admin/content/licenses');
}

function _content_license_form($license=array()) {
  $form = array();

  $form['#validate'][] = '_content_license_form_validate';

  if (is_array($license)) {
    $license = (object)array_merge($license, array(
      'license_title' => '',
      'license_url' => '',
      'license_badge' => '',
      'license_description' => '',
    ));
  }

  if (isset($license->lid)) {
    $form['lid'] = array(
      '#type' => 'value',
      '#value' => $license->lid,
    );
  }

  $form['license_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => $license->license_title,
  );

  $form['license_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Url'),
    '#required' => TRUE,
    '#default_value' => $license->license_url,
  );

  $form['license_badge'] = array(
    '#type' => 'textfield',
    '#title' => t('Badge url'),
    '#default_value' => $license->license_badge,
  );

  $form['license_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#cols' => 30,
    '#rows' => 10,
    '#default_value' => $license->license_description,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function _content_license_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  $redirect = FALSE;
  if (!_content_license_valid_uri($values['license_url'], $redirect)) {
    form_set_error('license_url', t('Invalid or broken url to the license agreement'));
  }
  if ($redirect) {
    $values['license_url'] = $redirect;
  }

  $redirect = FALSE;
  if (!empty($values['license_badge']) && !_content_license_valid_uri($values['license_badge'], $redirect)) {
    form_set_error('license_badge', t('Invalid or broken url to the license badge'));
  }
  if ($redirect) {
    $values['license_badge'] = $redirect;
  }
}

function _content_license_valid_uri($uri, &$redirect) {
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, $uri);
  curl_setopt($ch, CURLOPT_HEADER, 1);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_NOBODY, 1);
  $response = curl_exec($ch);
  $error = curl_errno($ch);
  curl_close($ch);

  if ($error) {
    return FALSE;
  }

  if (preg_match('/HTTP\/1.\d 30[12]/', $response)) {
    $location = array();
    if (preg_match('/Location:\s+(.+)/', $response, $location)) {
      $redirect = trim($location[1]);
      return TRUE;
    }
    return FALSE;
  }

  return preg_match('/HTTP\/1.\d (200)|(307)/', $response);
}