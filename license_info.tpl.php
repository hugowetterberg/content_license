<div class="license">
  <div class="title"><?php print $license->license_title ?></div>
  <?php if (!empty($license->license_badge)): ?>
    <img src="<?php print $license->license_badge ?>" class="badge" />
  <?php endif; ?>
  <div class="links">
    <?php if (!empty($license->license_url)): ?>
      <a href="<?php print $license->license_url ?>" target="_blank">Read more about the license here</a>
    <?php endif; ?>
  </div>
</div>