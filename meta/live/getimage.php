<?php

  /*** Create visualization for certain file types ***/

  // we need session information
  require_once('resource/php/session.php');

  // we need the session id and filename, for locating the file in the
  // working directory
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

  // the file to visualize actually is:
  $file = $_SESSION["dir"]."/".$file;

  // if file not found, then file not found
  if ( ! file_exists($file))
  {
    header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found", true, 404);
    echo "File not found. Please check URL.";
    exit();
  }

  // make it easy, get extension, basename and so on ...
  $info = pathinfo($file);
  
  // target name of image (for now png, maybe late dependend on file type)
  $imagefile = $file.".png";
  // actual URI to image for HTML source
  $link = LIVEBASE."getfile.php?file=".urlencode($info["basename"].".png")."&amp;id=".urlencode($_SESSION["uid"]);
  // info for the title attribute
  $label = $info["filename"];

  // caching, only create file, if it does not exist
  if ( ! file_exists( $imagefile ) )
  {
    // echo $imagefile." ".$info["extension"];
    switch ($info["extension"])
    {
      // we know how to convert owfn files using petri
      case "owfn":
        system("cd ".$_SESSION["dir"]."; petri -opng --removePorts ".basename($file)." &> /dev/null");
        // remove white border
        system('convert -trim '.$imagefile.' '.$imagefile.' &> /dev/null');
        break;
      case "og":
        echo "og found!";
        break;
      case "sa":
        echo "sa found!";
        break;
    }
  }

  // filename and link for thumbnail
  $thumbnail = dirname($imagefile).'/thumb_'.basename($imagefile);
  $thumblink = LIVEBASE."getfile.php?file=".urlencode(basename($thumbnail))."&amp;id=".urlencode($_SESSION["uid"]);

  // create thumbnail, if it does not exists
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

  // write link and thumbnail to HTML output
  if ( file_exists($imagefile) )
  {
    $result = '<a href="'.$link.'" target="_blank" title="'.$label.'"><img id="thumbnail" src="'.$thumblink.'" alt="'.$label.'" /></a>';

    echo $result;
    echo '<div class="label">'.$label.'</div>';
   
  }
  else
  {
    // probably some error occured
    header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found", true, 404);
    echo "File not found. Please check URL.";
    // exit();
  }

?>
