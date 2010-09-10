<?php
  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';

  $tool = "wendy";
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "wendy ";
  $realcall = "wendy ";
  $fakeresult = "";

  // collect parameters
  $_SESSION[$tool]['modus'] = (!strcmp($_SESSION[$tool]['mode'], 'ig'))?'sa':'og';
  if (isset($_REQUEST['reduction']))
    $_SESSION['reduction'] = $_REQUEST['reduction'];
  $_SESSION[$tool]['reduce_wait'] = (! empty($_SESSION[$tool]['reduce_wait']))?' --waitstatesOnly':'';
  $_SESSION[$tool]['reduce_reic'] = (! empty($_SESSION[$tool]['reduce_reic']))?' --receivingBeforeSending':'';
  $_SESSION[$tool]['reduce_sequ'] = (! empty($_SESSION[$tool]['reduce_sequ']))?' --seqReceivingEvents':'';
  $_SESSION[$tool]['reduce_succ'] = (! empty($_SESSION[$tool]['reduce_succ']))?' --succeedingSendingEvent':'';
  $_SESSION[$tool]['reduce_quit'] = (! empty($_SESSION[$tool]['reduce_quit']))?' --quitAsSoonAsPossible':'';

  /*
  $fileadditive = "_";
  $fileadditive .= (! empty($_SESSION[$tool]['reduce_wait']))?'1':'0';
  $fileadditive .= (! empty($_SESSION[$tool]['reduce_reic']))?'1':'0';
  $fileadditive .= (! empty($_SESSION[$tool]['reduce_sequ']))?'1':'0';
  $fileadditive .= (! empty($_SESSION[$tool]['reduce_succ']))?'1':'0';
  $fileadditive .= (! empty($_SESSION[$tool]['reduce_quit']))?'1':'0';
  */
  
  $inputfile = $_SESSION['wendy']["process"];
  
  $fakecall .= $process[$inputfile]["basename"];
  $realcall .= $process[$inputfile]["residence"];
  
  $fakecall .= $_SESSION[$tool]['reduce_wait'].$_SESSION[$tool]['reduce_reic'].$_SESSION[$tool]['reduce_sequ'].$_SESSION[$tool]['reduce_succ'].$_SESSION[$tool]['reduce_quit'];
  $realcall .= $_SESSION[$tool]['reduce_wait'].$_SESSION[$tool]['reduce_reic'].$_SESSION[$tool]['reduce_sequ'].$_SESSION[$tool]['reduce_succ'].$_SESSION[$tool]['reduce_quit'];

  $fakeresult .= $process[$inputfile]["filename"]; //.$fileadditive;
  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " --".$_SESSION[$tool]['modus'].'='.$fakeresult.".".$_SESSION[$tool]['modus'];
  $realcall .= " --".$_SESSION[$tool]['modus'].'='.$realresult.".".$_SESSION[$tool]['modus'];

  $realcall .= " -v 2>&1";
  // end of building call strings

  // if 'result' is set to 'output', only print the generated file and exit
  if (!strcmp($_SESSION[$tool]["output"], "result")) {
    resultOnly($realcall, $realresult.".".$_SESSION[$tool]['modus']);
  }

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live &ndash; Partner Synthesis</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><a href="index.html"><img src="resource/images/live.png" alt="service-technology.org/live" /></a></div>
      <h1>Partner Synthesis</h1>

      <h2>Parameters</h2>
      <ul>
        <li><strong>input file:</strong> <?=$process[$inputfile]["basename"]?>
          <?php
          switch($_SESSION['input_type']) {
            case "example":   echo "example file"; break;
            case "uploaded":  echo "uploaded file"; break;
            case "url":       echo "downloaded file"; break;
            case "given":     echo "given file"; break;
          }
          ?>
        </li>
        <li><strong>verification goal:</strong> 
          <?php echo ((!strcmp($_SESSION[$tool]['mode'], 'ig'))?'interaction graph':'operating guidelines'); ?>
        </li>
        <!--        <li><strong>apply reduction techniques:</strong> 
        <?php echo ((isset($_SESSION['reduction']))?'yes':'no'); ?>
        </li>-->
      </ul>

      <h2>Result</h2>
      <?php console($fakecall, $realcall)?>

      <h2>Output</h2>
      <?php drawImage($fakeresult.'.'.$_SESSION[$tool]['modus']); ?>

      <p><a href="<?=getLink($realresult.".".$_SESSION[$tool]['modus'])?>">result</a></p>

    </div>
  </div>
  <div id="footer">
    <p><a href="http://service-technology.org">service-technology.org</a> is a cooperation between the <a  href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/">University of Rostock</a> and the <a  href="http://www2.informatik.hu-berlin.de/top/index.php">Humboldt-Universit&auml;t zu Berlin</a>.</p>
  </div>
</body>
</html>
