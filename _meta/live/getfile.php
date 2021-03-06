<?php

  /*** wrapper for downloading files in working directory ***/

  require('resource/php/session.php');

  // get session id and filename
  $id = $_GET["id"];
  $file = $_GET["file"];
  $subdir = $_GET["tc"];

  $slashes = array("/", "\\");

  $id = str_replace($slashes, "", urldecode($id));
  $file = str_replace($slashes, "", urldecode($file));
  $subdir = str_replace($slashes, "", urldecode($subdir));

  // here should be the file:
  $getfile = getDirName($id)."/".$subdir."/".$file;

  if ( file_exists($getfile) )
  {
    // for extension of the file
    $info = pathinfo($getfile);

    // try to set mime type automatically 
    $ct = mime_content_type( $getfile );
    switch ($info["extension"])
    {
      // for owfn files we set the mime type manually
      case "owfn":
        $ct = "text/plain";
        break;
      // for bpel files we set the mime type manually
      case "bpel":
        $ct = "text/xml";
        break;
    }

    // set mime type
    header('Content-type: '.$ct);
    // set filename for download
    header('Content-Disposition: inline; filename="'.$file.'"');
    // printout file
    readfile($getfile);
  }
  else
  {
    // upps, file not found
    header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found", true, 404);
    echo "File not found. Please check URL.";
    // exit();
  }

?>
