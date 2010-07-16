<?php

  require('resource/php/session.php');

  //print_r($_GET);
  $id = $_GET["id"];
  $file = $_GET["file"];
  // set size of thumbnails
  if ( ! isset($_GET['thumbnail_size']))
    $thumbnail_size = 300;
  else
    $thumbnail_size = $_GET['thumbnail_size'];

  $slashes = array("/", "\\");

  $id = str_replace($slashes, "", urldecode($id));
  $file = str_replace($slashes, "", urldecode($file));

  $file = $_SESSION["dir"]."/".$file;

  if ( ! file_exists($file))
  {
    header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found", true, 404);
    echo "File not found. Please check URL.";
    exit();
  }

  // echo $id.", ".$file.", ".$getfile;

  $info = pathinfo($file);
  
  $imagefile = $file.".png";
  $link = LIVEBASE."getfile.php?file=".urlencode($info["basename"].".png")."&amp;id=".urlencode($_SESSION["uid"]);
  $label = $info["filename"];

  if ( ! file_exists( $imagefile ) )
  {
    // echo $imagefile." ".$info["extension"];
    switch ($info["extension"])
    {
      case "owfn":
        system("cd ".$_SESSION["dir"]."; petri -opng ".basename($file)." &> /dev/null");
        system('convert -trim '.$imagefile.' '.$imagefile.' &> /dev/null');
        break;
    }
  }

  // create thumbnail
  $thumbnail = dirname($imagefile).'/thumb_'.basename($imagefile);
  $thumblink = LIVEBASE."getfile.php?file=".urlencode(basename($thumbnail))."&amp;id=".urlencode($_SESSION["uid"]);

  if (!file_exists($thumbnail)) {
    
    // check the size of the image: if 66% does not exceed the given
    // thumbnail size, resize it to 66%, otherwise to the thumbnail size
    list($width, $height) = getimagesize($imagefile);
    if (($width * .66 <= 700) && ($height * .66 <= $thumbnail_size)) {
      system('convert -thumbnail "66%" '.$imagefile.' '.$thumbnail);          
    } else {
      system('convert -thumbnail "700x'.$thumbnail_size.'>" '.$imagefile.' '.$thumbnail);
    }    
  }
  



  if ( file_exists($imagefile) )
  {
    /*
    $info = pathinfo($imagefile);

    $ct = mime_content_type( $imagefile );
    header('Content-type: '.$ct);
    readfile($imagefile);
    */

    $result = '<a href="'.$link.'" target="_blank"><img id="thumbnail" src="'.$thumblink.'" alt="'.$label.'" /></a>';

    echo $result;
    echo '<div class="label">'.$label.'</div>';
   
  }
  else
  {
    header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found", true, 404);
    echo "File not found. Please check URL.";
    // exit();
  }

?>
