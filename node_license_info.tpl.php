<div class="content-license">
  <h3><?php print t('License') ?></h3>
  <?php
  foreach (element_children($element) as $key) {
    print theme('license_info', $element[$key]['#license']);
  }
  ?>
</div>