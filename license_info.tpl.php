<div class="content-licenses">
<?php
  foreach ($licenses as $lid => $license) {
?>
  <div class="license">
    <h2><?php print $license->license_title ?></h2>
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
  </div>
<?php
  }
?>
</div>