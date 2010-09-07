<?php

require 'session.php';

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

// create a string representing the URI for downloading a file
function getLink($file)
{
  return LIVEBASE."getfile.php?file=".urlencode(basename($file))."&amp;id=".urlencode($_SESSION["uid"]);
}

// function, which creates an image for a $file and actually writes some HTML code
// rest of the funtionality inside getimage.php
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

/*
 * 
 */
function exampleFile($example, $basedir)
{
  $result = array();
  $info = pathinfo($example);
  
  $result[$example] = $info;
  $result[$example]["residence"] = $_SESSION["dir"]."/".$info["basename"];
  $result[$example]["short"] = $_SESSION["dir"]."/".$info["filename"];
  $result[$example]["link"] = getLink($info["basename"]);
  
  copy($basedir."/".$example, $result[$example]["residence"]);
  
  return $result;
}

/*
 * Creating a file from a given URL. Return value as above.
 */
function createFileFromUrl($url)
{
  $result = array();
  
  $info = pathinfo($url);
  
  $result[$url] = $info;
  $result[$url]["residence"] = $_SESSION["dir"]."/".$info["basename"];
  $result[$url]["short"] = $_SESSION["dir"]."/".$info["filename"];
  $result[$url]["link"] = $url;
  
  $download = $download = 'wget \''.$url.'\' -O '.$result[$url]["residence"];
  system($download);
  
  return $result;
}

/*
 * 
 */
function createFileFromUpload()
{
  $result = array();
  
  $upload = 'uploadedfile_'.rand().rand();
  $info = pathinfo($upload);
  
  $result[$upload] = $info;
  $result[$file]["residence"] = $_SESSION["dir"]."/".$info["basename"];
  $result[$file]["short"] = $_SESSION["dir"]."/".$info["filename"];
  $result[$file]["link"] = getLink($info["basename"]); // FIXME
    
  move_uploaded_file($_FILES['input_file']['tmp_name'], $result[$upload]["residence"]);
  
  return $result;
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
  $result[$file]["short"] = $_SESSION["dir"]."/".$info["filename"];
  $result[$file]["link"] = getLink($info["basename"]); // FIXME
   
  $handle = fopen($result[$file]["residence"], "w+");
  fwrite($handle, $content);
  fclose($handle);

  return $result;
}

/*
 * Main part: This file MUST be included into any tool script. We process the given request variable
 * to set or create files respectively.
 */

$fileinfo = array();

// in case an example file was chosen from drop-down
if ($_REQUEST["input_type"] == 'example')
{
  $fileinfo = exampleFile($_REQUEST["input_example"].'.'.$_REQUEST["fileextension"], $_REQUEST["toolname"]);
}

// in case an URL was given
if ($_REQUEST["input_type"] == 'url')
{
  $fileinfo = createFileFromUrl($_REQUEST["input_url"]);
}

// in case a file was uploaded
if ($_REQUEST["input_type"] == 'uploaded')
{
  $fileinfo = createFileFromUpload($_REQUEST["input_uploaded"]);
}

// in case a file was given
if ($_REQUEST["input_type"] == 'given')
{
  $fileinfo = createFile('givenfile_'.rand().rand(), $_REQUEST["input_given"]);
}

?>
