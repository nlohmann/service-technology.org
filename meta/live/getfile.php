<?php

  require('resource/php/session.php');

  $id = $_GET["id"];
  $file = $_GET["file"];

  $slashes = array("/", "\\");

  $id = str_replace($slashes, "", urldecode($id));
  $file = str_replace($slashes, "", urldecode($file));

  $getfile = getDirName($id)."/".$file;

  // echo $id.", ".$file.", ".$getfile;

  if ( file_exists($getfile) )
  {
    $info = pathinfo($getfile);

    $ct = mime_content_type( $getfile );
    switch ($info["extension"])
    {
      case "owfn":
        $ct = "text/plain";
    }

     header('Content-type: '.$ct);
    // header('Content-Disposition: attachment; filename="'.$file.'"');
    readfile($getfile);
  }
  else
  {
    header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found", true, 404);
    echo "File not found. Please check URL.";
    // exit();
  }

?>
