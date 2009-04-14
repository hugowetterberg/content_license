<div id="<?php print $form['#id']; ?>-wrapper" class="content-licenses">
<?php
  foreach ($form['#options'] as $lid => $title) {
    $license = $lid!='none' ? $form['#licenses'][$lid] : (object)array(
      'license_badge' => url(drupal_get_path('module', 'content_license') . '/copyrighted.png'),
    );
    $id = $form['#id'] . '-' . $lid;
    ?>
    <div class="license">
      <div class="select">
        <input type="<?php print $widget ?>" id="<?php print $id ?>" name="<?php print "{$form['#name']}[{$lid}]";
          ?>" value="<?php print $lid ?>" <?php
          print in_array($lid, $form['#default_value']) ? 'checked="checked"' : '' ?>/>
        <label for="<?php print $id ?>">
          <?php print $title ?>
        </label>
      </div>
      <?php if (!empty($license->license_badge)): ?>
      <img src="<?php print $license->license_badge ?>" class="badge" />
      <?php endif; ?>
      <p class="description">
        <?php print $license->license_description ?>
      </p>
      <div class="links">
        <?php if (!empty($license->license_url)): ?>
        <a href="<?php print $license->license_url ?>" target="_blank">Read more about the license here</a>
        <?php endif; ?>
      </div>
    </div><?php
  }
?>
</div>