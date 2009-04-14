<?php
// $Id$

function _content_license_settings($form_state) {
  $form = array();

  $licenses = ContentLicense::load();
  if (!empty($licenses)) {
    $form['licenses'] = array(
      '#type' => 'select',
      '#title' => t('Edit a license'),
      '#options' => array(),
    );

    foreach ($licenses as $license) {
      $form['licenses']['#options'][$license->lid] = t($license->license_title);
    }

    $form['edit'] = array(
      '#type' => 'submit',
      '#value' => t('Edit'),
    );
  }

  $form['add_license'] = array(
    '#type' => 'item',
    '#value' => l(empty($licenses) ? t('No licenses have been added, click here to add one') : t('Add license'), 'admin/content/licenses/add'),
  );

  $form['mode'] = array(
    '#type' => 'radios',
    '#title' => t('Licensing mode'),
    '#options' => array(
      'single' => t('Only single licenses'),
      'multi' => t('Allow multiple licenses'),
    ),
    '#default_value' => variable_get('content_license_mode', 'multi'),
  );

  $form['selector'] = array(
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

  $form['show_license_info'] = array(
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

  $form['types'] = array(
    '#title' => t('Enable licensing options for'),
    '#type' => 'checkboxes',
    '#options' => $type_options,
    '#default_value' => $type_default,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function _content_license_settings_submit($form, $form_state) {
  $values = $form_state['values'];
  if ($values['op'] == t('Save')) {
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
  else if ($values['op'] == t('Edit')) {
    drupal_goto('admin/content/licenses/edit/' . $values['licenses']);
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