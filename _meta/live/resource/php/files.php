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
 
  // checking existence is only a workaround, since we have examples,
  // where the example name does not correspond to exactly one file
  // therefore $file may not exist (but normally it should
  if(file_exists($file))
  copy($file, $result[$file]["residence"]);

  return $result;
}

// display result file only
function resultOnly($call, $file) {
    header("Content-type: text/plain");
    exec($call);
    $lines = file($file);

    foreach ($lines as $line_num => $line) {
        echo $line;
    }
    exit;
}
  

// function, which creates an image for a $file and actually writes some HTML code
// rest of the funtionality inside getimage.php
function drawImage($file, $thumbsize = 300, $label= "")
{
  static $imagecounter = 0;
  $imagecounter++;

  $info = pathinfo($file);

  $callstring = LIVEBASE."getimage.php?file=".urlencode($file)."&thumbnail_size=".$thumbsize."&label=".urlencode($label)."&id=".urlencode($_SESSION["uid"])."&tc=".urlencode($_SESSION["subdir"]);
  // $thumblink = getLink("thumb_".$file.".png");
  //LIVEBASE."getfile.php?file=".urlencode("thumb_".$file.".png")."&amp;id=".urlencode($_SESSION["uid"]);
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
    
    /*
    $_SESSION[$tool] = array();
    foreach ($_REQUEST as $key => $value)
    {
      $_SESSION[$tool][$key] = $value;
    }
    */
    
    // $_SESSION[$tool] = $_REQUEST;
    
    if (isset($_REQUEST["input_type"]))
    {
     $_SESSION[$tool] = $_REQUEST;
     $_SESSION["input_type"] = $_REQUEST["input_type"];
     
     // create new subdir vor request
     $_SESSION["subdir"] = $_SERVER["REQUEST_TIME"];
      
      if ( ! strcmp($_REQUEST["input_type"], 'example') )
      {
        // remember name of example in session
        $_SESSION[$tool]["process"] = "$tool/".$_REQUEST["input_example"];
        $set = true;
      }
      else if ( ! strcmp($_REQUEST["input_type"], 'uploaded') )
      {
        $dir = getDirName($_SESSION["uid"]);
        $target = $dir."/".$_FILES["input_uploaded"]["name"];
        move_uploaded_file($_FILES["input_uploaded"]['tmp_name'], $target);
        $_SESSION[$tool]["process"] = $target;
        $set = true;
      }
      else if ( ! strcmp($_REQUEST["input_type"], 'url') )
      {

        $dir = getDirName($_SESSION["uid"]);
        
        $download = 'cd '.$dir.'; pwd; wget -N --content-disposition \''.$_REQUEST["input_url"].'\' 2>&1 | grep "Saving to" 2>&1 | gawk \'/Saving to:/ { print $3; } \''; // -O '.$process[$_SESSION["$tool"]["process"]]["residence"];
        $output = array();
        $name = exec($download, $output);
        $marks = array("`", "'");
        $name = str_replace($marks, "", $name);
        /*
        echo $download."\n";
        print_r($output);
        echo $name."\n";
        */
        $_SESSION[$tool]["process"] = $dir."/".$name;
        //echo $_SESSION[$tool]["process"];
        //exit;
        $set = true;
      }
      else if ( ! strcmp($_REQUEST["input_type"], 'given') )
      {
        $_SESSION[$tool]["process"] = "given";
        $set = true;
      }
      if (strcmp($_REQUEST["input_type"], 'uploaded') && strcmp($_REQUEST["input_type"], 'url'))
      {
        switch ($tool)
        {
          case "bpel2owfn":
            $_SESSION[$tool]["process"] .= ".bpel";
            break;
          default:
            $_SESSION[$tool]["process"] .= ".owfn";
            break;
        }
      }
    }
    
    if ($set)
    {
      if ( isset($_REQUEST["input_type"]) )
      $_SESSION["input_type"] = $_REQUEST["input_type"];
      if ( isset($_REQUEST["input_given"]) )
      $_SESSION["input_given"] = $_REQUEST["input_given"];
      /*
      $_SESSION["patterns"] = $_REQUEST["patterns"];
      $_SESSION["format"] = $_REQUEST["format"];
      $_SESSION["reduce"] = $_REQUEST["reduce"];
      */
      
      unset($_REQUEST);
      // redirect to self, make back/forward buttons work without 
      // resending the request
      header("Location: ".$_SERVER["PHP_SELF"]);
      exit;
    }
  }
}

if ( (! isset($_SESSION[$tool]["process"])) || empty($_SESSION[$tool]))
{
/*
  echo "bla";
  print_r($_SESSION);
  exit;
*/
  // direct call of this page -> return to main page
  header('Location: index.html#'.$tool);
  exit;
}

// copied from Wendy ;)
include_once 'resource/php/console.php';
include_once 'resource/php/dotimg.php';
include_once 'resource/php/getnumber.php';

// output header
if (isset($_SESSION[$tool]["output"])) 
if (strcmp($_SESSION[$tool]["output"], "result")) {
  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="utf-8" ?>';
}


$process = $_SESSION[$tool]["process"];

if ( ! strcmp($_SESSION[$tool]["input_type"], 'example') )
{
  $process = prepareFile($process); 
}
else if ( ! strcmp($_SESSION[$tool]["input_type"], 'uploaded') )
{
  $process = prepareFile($process);
}
else if ( ! strcmp($_SESSION["input_type"], 'url') )
{
  $process = prepareFile($process);
}
else if ( ! strcmp($_SESSION["input_type"], 'given') )
{
  $process = createFile($process, $_SESSION["input_given"]);
}

if ( !is_array ($process))
{
echo "process is not an array";
// exit;
}

/*
 * Main part: This file MUST be included into any tool script. We process the given request variable
 * to set or create files respectively.
 */

?>
