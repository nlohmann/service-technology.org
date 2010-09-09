<?php
  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  $tool = "rachel";

  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // prepare example
  switch () {
    case "rachel/running_example.owfn":
      $automata = array("rachel/running_example.sa");
      $og = array("rachel/running_example.og");
      break;
  }

  // copy files to temporary directory, see files.php for details
  $automata = prepareFiles($automata);
  $og = prepareFiles($og);

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "rachel --help";
  $realcall = "rachel --help"

//  $choreography = $_REQUEST['chor'];
//  $editdistance = $_REQUEST['mode'];

/*
  $call_1 = 'rachel -m og -o '.$choreography.'.og -d';  
  $call_2 = 'rachel -m sa -a '.$choreography.'.sa -d';  
  $call_3 = 'rachel -m '.$editdistance.' -a '.$choreography.'.sa -o '.$choreography.'.og -d';  
*/
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live &ndash; Service Correction</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><a href="index.html"><img src="resource/images/live.png" alt="service-technology.org/live" /></a></div>
      <h1>Service Correction</h1>

      <h2>Input</h2>

      <!-- draw images of services -->
      <?php
      foreach($automata as $file)
      {
        drawImage($file["basename"]);
      }
      foreach($og as $file)
      {
        drawImage($file["basename"]);
      }
      ?>

      <h2>Result</h2>
      <?php console($fakecall, $realcall)?>
            
    </div>
  </div>
  <div id="footer">
    <p><a href="http://service-technology.org">service-technology.org</a> is a cooperation between the <a  href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/">University of Rostock</a> and the <a  href="http://www2.informatik.hu-berlin.de/top/index.php">Humboldt-Universit&auml;t zu Berlin</a>.</p>
  </div>
</body>

</html>
