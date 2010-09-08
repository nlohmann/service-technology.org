<?php

  require_once 'resource/php/session.php';
  
	// if ( isset( $_POST["logout"] ) )
	{ 
	  // cleaning up $_SESSION["dir"]
	  // \todo: find a nice function to do this (see rmdir on php.net)
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
