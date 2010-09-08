<?php

  require_once 'session.php';

/**
 * Gets an array of file names, copies the files to the working
 * directory and returns an structured array with information on the 
 * files:
 * 
 *  $a = prepareFiles( array ("foo/file1.bla", "bar/file2.blob") );
 * 
 *  $a => (
              [foo/file1] => Array
                  (
                      // original dirname
                      [dirname] => foo
                      // full filename
                      [basename] => file1.bla
                      // file extension
                      [extension] => bla
                      // filename without extension
                      [filename] => file1
                      // actual place in working directory (based on session id)
                      [residence] => /tmp/live/12ea9bc4df7609ee32f88a07a2714220/file1.bla
                      // link for downloading file
                      [link] => /live/getfile.php?file=file1.bla&amp;id=12ea9bc4df7609ee32f88a07a2714220
                  )

              [bar/file2.blob] => Array
                  (
                      [dirname] => bar
                      [basename] => file2.blob
                      [extension] => blob
                      [filename] => file2
                      [residence] => /tmp/live/12ea9bc4df7609ee32f88a07a2714220/file2.blob
                      [link] => /live/getfile.php?file=file2.blob&amp;id=12ea9bc4df7609ee32f88a07a2714220
                  )

          )
 */
 
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

function prepareFile($file)
{  
  $result = array();
  // returns "basename", "extension", "dirname", and "filename"
  $info = pathinfo($file);
  
  $result[$file] = $info;
  $result[$file]["residence"] = $_SESSION["dir"]."/".$info["basename"];
  $result[$file]["link"] = getLink($info["basename"]);
 
  copy($file, $result[$file]["residence"]);

  return $result;
}

// create a string representing the URI for downloading a file
function getLink($file)
{
  return LIVEBASE."getfile.php?file=".urlencode(basename($file))."&amp;id=".urlencode($_SESSION["uid"]);
}

// function, which creates an image for a $file and actually writes some HTML code
// rest of the funtionality inside getimage.php
function drawImage($file, $thumbsize = 300)
{
  static $imagecounter = 0;
  $imagecounter++;

  $info = pathinfo($file);

  $callstring = LIVEBASE."getimage.php?file=".urlencode($file)."&thumbnail_size=".$thumbsize."&id=".urlencode($_SESSION["uid"]);
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

/**
* Similar to prepareFiles, but instead of copying an array of files,
* one single file is created.
* Return value as above.
*/
function createFile($file, $content = "")
{
  $result = array();
  
  // returns "basename", "extension", "dirname", and "filename"
  $info = pathinfo($file);
    
  $result[$file] = $info;
  $result[$file]["residence"] = $_SESSION["dir"]."/".$info["basename"];
  $result[$file]["link"] = getLink($info["basename"]);
   
  $handle = fopen($result[$file]["residence"], "w+");
  fwrite($handle, stripslashes($content));
  fclose($handle);

  return $result;
}

// do not call the page without POST request, or only if session is 
// already set to Marlene (all information about services available)
if ( ! isset($_REQUEST) && ! isset($_SESSION[$tool]))
{
  // direct call of this page -> return to main page
  header("Location: index.html#$tool");
  exit;
}
else
{
  // new request?
  if ( isset($_REQUEST) && ! empty($_REQUEST))
  {
    $set = false;
    
    $_SESSION["input_type"] = $_REQUEST["input_type"];
    
    if ( ! strcmp($_REQUEST["input_type"], 'example') )
    {
      // remember name of example in session
      $_SESSION[$tool] = "$tool/".$_REQUEST["input_example"];
      $set = true;
    }
    else if ( ! strcmp($_REQUEST["input_type"], 'uploaded') )
    {
      $_SESSION[$tool] = $_REQUEST["input_uploaded"];
      $set = true;
    }
    else if ( ! strcmp($_REQUEST["input_type"], 'url') )
    {
      $_SESSION[$tool] = $_REQUEST["input_url"];
      $set = true;
    }
    else if ( ! strcmp($_REQUEST["input_type"], 'given') )
    {
      $_SESSION[$tool] = "given_".md5(uniqid(mt_rand(), true));
      $set = true;
    }
    if (strcmp($_REQUEST["input_type"], 'uploaded'))
    {
      switch ($tool)
      {
        case "bpel2owfn":
          $_SESSION[$tool] .= ".bpel";
          break;
        default:
          $_SESSION[$tool] .= ".owfn";
          break;
      }
    }
    
    if ($set)
    {
      $_SESSION["input_type"] = $_REQUEST["input_type"];
      $_SESSION["input_given"] = $_REQUEST["input_given"];
      $_SESSION["patterns"] = $_REQUEST["patterns"];
      $_SESSION["format"] = $_REQUEST["format"];
      $_SESSION["reduce"] = $_REQUEST["reduce"];
      
      unset($_REQUEST);
      // redirect to self, make back/forward buttons work without 
      // resending the request
      header("Location: ".$_SERVER["PHP_SELF"]);
      exit;
    }
  }
}

if ( ! isset($_SESSION["$tool"]))
{
  // direct call of this page -> return to main page
  header('Location: index.html#'.$tool);
  exit;
}

// copied from Wendy ;)
include_once 'resource/php/console.php';
include_once 'resource/php/dotimg.php';
include_once 'resource/php/getnumber.php';

// output header
header("Content-Type: text/html");
echo '<?xml version="1.0" encoding="utf-8" ?>';

$process = $_SESSION["$tool"];

if ( ! strcmp($_SESSION["input_type"], 'example') && ! strcmp($tool, "bpel2owfn") )
{
  $process = prepareFile($process); 
}
else if ( ! strcmp($_SESSION["input_type"], 'uploaded') )
{
  $process = createFile($process);
  move_uploaded_file($_FILES['input_file']['tmp_name'], $process[$_SESSION["$tool"]]["residence"]);
}
else if ( ! strcmp($_SESSION["input_type"], 'url') )
{
  $process = createFile($process);
  $download = 'wget \''.$_SESSION["$tool"].'\' -O '.$process[$_SESSION["$tool"]]["residence"];
  system($download);
}
else if ( ! strcmp($_SESSION["input_type"], 'given') )
{
  $process = createFile($process, $_SESSION["input_given"]);
}

/*
 * Main part: This file MUST be included into any tool script. We process the given request variable
 * to set or create files respectively.
 */

?>
