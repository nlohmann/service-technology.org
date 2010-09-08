<?php

  require_once 'resource/php/session.php';

function deleteDirectory($dir) {
    if (!file_exists($dir)) return true;
    if (!is_dir($dir)) return unlink($dir);
    foreach (scandir($dir) as $item) {
        if ($item == '.' || $item == '..') continue;
        if (!deleteDirectory($dir.DIRECTORY_SEPARATOR.$item)) return false;
    }
    return rmdir($dir);
}
      
	// if ( isset( $_POST["logout"] ) )
	{ 
	  // cleaning up $_SESSION["dir"]
	  if (!empty($_SESSION["dir"]))
	  {
	    deleteDirectory($_SESSION["dir"]);
	  }
	  // restart 
		$_SESSION = array();
			if (isset($_COOKIE[session_name()])) {
			   setcookie(session_name(), '', time()-42000, '/');
			}
		session_destroy();
		session_start();

		$host = $_SERVER['HTTP_HOST'];
		$uri  = $_SERVER['REQUEST_URI'];
		header("Location: ".LIVEBASE);
		exit(0);
	}
?>
