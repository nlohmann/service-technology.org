<?php
  putenv("PATH=/usr/local/bin:/opt/local/bin:/opt/local/sbin:/usr/bin");

  $dotimg_counter = 0;

  function dotimg($callstring) {
    global $dotimg_counter;
    $dotimg_counter++;
    
    echo '<script type="text/javascript">'."\n";
    echo '<!--'."\n";
    echo '  $(document).ready(function(){'."\n";
    echo '    $(\'#thumb'.$dotimg_counter.'\').load(\'dot.php?'.$callstring.'\');'."\n";
    echo '    $(\'#thumb'.$dotimg_counter.'\').fadeIn("slow");'."\n";
    echo '  });'."\n";
    echo '// -->'."\n";
    echo '</script>'."\n";
    
    echo '<div class="dotimg" id="thumb'.$dotimg_counter.'">';
    echo '  <img src="resource/images/spinner.gif" alt="rendering image" /><br />rendering image...';
    echo '</div>';
  }
?>
