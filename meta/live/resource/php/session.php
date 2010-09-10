<?php

  // read configuration file including information on PATH for tools
  require_once(dirname(__FILE__).'/../../conf/live.conf');

  // debugging, should be removed in later versions for security reasons
  ini_set("display_errors", "On");
  ini_set("session.use_trans_sid", "0");
  //error_reporting($_SERVER["SERVER_NAME"] == "localhost" ? E_ALL : 0);
  error_reporting(E_ALL);

  // set some environment variables
  // set PATH vor system calls
  putenv("PATH=".LIVEROOT.DEFAULTVERSION."/bin:".ADDITIONALPATH);
  // set sysconfdir (for .conf files) ?? any effect ??
  putenv("SYSCONFDIR=".LIVEROOT.DEFAULTVERSION."/etc");
  // set library path, needed right now for Marlene and libconfig++
  putenv("LD_LIBRARY_PATH=/usr/local/lib");


function getDirName($uid)
{
  return WORKDIR."$uid";
}

// create a string representing the URI for downloading a file
function getLink($file)
{
  return LIVEBASE."getfile.php?file=".urlencode(basename($file))."&amp;id=".urlencode($_SESSION["uid"])."&amp;tc=".urlencode($_SESSION["subdir"]);
}


  session_start();
  if ( ! isset( $_SESSION ) )
  {
    $_SESSION = array();
  }
  if ( ! isset( $_SESSION["uid"] ) )
  {
    $_SESSION["uid"] = md5(uniqid(mt_rand(), true));
  }
  if ( ! isset( $_SESSION["subdir"] ) )
  {
    $_SESSION["subdir"] = time();
  }
  //if ( ! isset( $_SESSION["dir"] ) )
  {
    $_SESSION["dir"] = getDirName($_SESSION["uid"])."/".$_SESSION["subdir"];
  }
  // if dir does not exist, create it recursively
  if ( ! file_exists( $_SESSION["dir"] ) )
  {
    mkdir( $_SESSION["dir"], 0777, true );
  }
  // echo "<!-- Unique ID ".$_SESSION["uid"]."-->\n";
?>
