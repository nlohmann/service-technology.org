<?php

function prepareFiles($fileArray)
{
  $result = array();
  
  foreach($fileArray as $file)
  {
    // returns "basename", "extension", "dirname", and "filename"
    $info = pathinfo($file);
    
    $result[$file] = $info;
    $result[$file]["residence"] = $_SESSION["dir"]."/".$info["basename"];
    $result[$file]["link"] = getLink($info["basename"]);
   
    copy($file, $result[$file]["residence"]);
  }

  return $result;
}

function getLink($file)
{
  return LIVEBASE."getfile.php?file=".urlencode(basename($file))."&amp;id=".urlencode($_SESSION["uid"]);
}


function drawImage($file)
{
  static $imagecounter = 0;
  $imagecounter++;

  $info = pathinfo($file);

  $callstring = LIVEBASE."getimage.php?file=".urlencode($file)."&id=".urlencode($_SESSION["uid"]);
  $thumblink = LIVEBASE."getfile.php?file=".urlencode("thumb_".$file.".png")."&amp;id=".urlencode($_SESSION["uid"]);
  $label = $info["filename"];

  echo '<script type="text/javascript">'."\n";
  echo '<!--'."\n";
  echo '  $(document).ready(function(){'."\n";
  echo '    $(\'#thumb'.$imagecounter.'\').load(\''.$callstring.'\');'."\n";
  echo '    $(\'#thumb'.$imagecounter.'\').fadeIn("slow");'."\n";
  echo '  });'."\n";
  echo '// -->'."\n";
  echo '</script>'."\n";
    
  echo '<div class="dotimg" id="thumb'.$imagecounter.'">';
  echo '  <img src="resource/images/spinner.gif" alt="rendering image" /><br />rendering image...';
  echo '</div>';

}
?>
