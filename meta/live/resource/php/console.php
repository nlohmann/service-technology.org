<?php

putenv("PATH=/usr/local/bin:/opt/local/bin:/opt/local/sbin");

$console_counter = 0;

function console($public, $private) {
  global $console_counter;
  $console_counter++;
  
  echo '
<div class="console" id="c'.$console_counter.'">
  <div class="title">
    <img src="resource/images/toggle.png" alt="toggle window" class="toggle" />
    <img src="resource/images/close.png" alt="close window" class="close" />
    <img src="resource/images/minimize.png" alt="minimize window" class="minimize" />
    <img src="resource/images/resize.png" alt="adjust window size" class="resize" />
  </div>
  <div class="window">
    <div class="status2"><img src="resource/images/bar.gif" alt="running" /></div>
    <div class="content"><div class="command">$ '.$public.'</div>';

  // execute the command
  flush();
  ob_flush();
  $start = time();
  $resultfilename = tempnam("tmp", "console-".$console_counter);
  system($private.' &> '.$resultfilename, $result);
  readfile($resultfilename);

  echo '</div>';
  
  // evaluate exit code and write status bar
  echo '    <div class="status">';
  echo '    <a href="tmp/'.basename($resultfilename).'" target="_blank">';
  if ($result == 0) {
    echo '<img src="resource/images/ok.png" alt="OK" /></a>Runtime: '.(time() - $start).' seconds.';
  } else {
    echo '<img src="resource/images/error.png" alt="error" /></a>Runtime: '.(time() - $start).' seconds. An error occurred.';
  }
  echo '    </div>';
  
  echo '  </div>';
  echo '</div>';

  // write logging information to temp file
  $fh = fopen($resultfilename, 'a');
  fwrite($fh, "\n\n\n");
  fwrite($fh, "ST_COMMAND:     ".$public."\n");
  fwrite($fh, "ST_EXIT-CODE:   ".$result."\n");
  fwrite($fh, "ST_RUNTIME:     ".(time() - $start)."\n");
  fwrite($fh, "ST_URL:         ".$_SERVER['REQUEST_URI']."\n");
  fwrite($fh, "ST_CALLER:      ".$_SERVER['REMOTE_ADDR']."\n");
  fclose($fh);
  
  // Javascript for console GUI
  echo '<script type="text/javascript">'."\n";
  echo '<!--'."\n";
  echo '$(document).ready(function(){
      $(\'.console#c'.$console_counter.' .window .status2\').hide();
      
      $(\'.console#c'.$console_counter.' .title img.toggle\').click(
        function() {
          $(\'.console#c'.$console_counter.' .window\').slideToggle();
        }
      );
      $(\'.console#c'.$console_counter.' .title img.close\').click(
        function() {
          $(\'.console#c'.$console_counter.'\').hide();
        }
      );
      $(\'.console#c'.$console_counter.' .title img.minimize\').click(
        function() {
          $(\'.console#c'.$console_counter.' .window .content\').css(\'height\', \'2em\');
          $(\'.console#c'.$console_counter.'\').css(\'width\', \'300px\');
          $(\'.console#c'.$console_counter.' .window .content\').css(\'min-height\', \'2em\');
          $(\'.console#c'.$console_counter.' .title\').css(\'background-image\', \'url(resource/images/console-small.png)\');
          $(\'.console#c'.$console_counter.' .window .content\').css(\'overflow\', \'hidden\');
        }
      );
      $(\'.console#c'.$console_counter.' .title img.resize\').click(
        function() {
          $(\'.console#c'.$console_counter.' .window .content\').css(\'height\', \'100%\');
          $(\'.console#c'.$console_counter.' .window .content\').css(\'min-height\', \'100%\');
          $(\'.console#c'.$console_counter.' .title\').css(\'background-image\', \'url(resource/images/console.png)\');
          $(\'.console#c'.$console_counter.' .window .content\').css(\'overflow\', \'hidden\');
          $(\'.console#c'.$console_counter.'\').css(\'width\', \'558px\');
        }
      );
    });'."\n";;
  echo '// -->'."\n";
  echo '</script>'."\n";
  
  return array($result, $resultfilename);
}

?>
