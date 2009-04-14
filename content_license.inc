<?php

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
    $form['license']['#options']['none'] = t('None (all rights reserved)');
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