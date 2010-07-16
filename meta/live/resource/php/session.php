<?php

  require(dirname(__FILE__).'/../../conf/live.conf');

  ini_set("display_errors", "On");
  ini_set("session.use_trans_sid", "0");
  error_reporting($_SERVER["SERVER_NAME"] == "localhost" ? E_ALL : 0);

  putenv("PATH=".LIVEROOT.DEFAULTVERSION."/bin:".ADDITIONALPATH);
  putenv("SYSCONFDIR=".LIVEROOT.DEFAULTVERSION."/etc");

function getDirName($uid)
{
  return WORKDIR."$uid";
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
  if ( ! isset( $_SESSION["dir"] ) )
  {
    $_SESSION["dir"] = getDirName($_SESSION["uid"]);
  }
  if ( ! file_exists( $_SESSION["dir"] ) )
  {
    mkdir( $_SESSION["dir"], 0777, true );
  }
  // echo "<!-- Unique ID ".$_SESSION["uid"]."-->\n";
?>
