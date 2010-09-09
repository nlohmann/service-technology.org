<?php

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  
  $tool = "bpel2owfn";
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "bpel2owfn";
  $realcall = "bpel2owfn";
  $fakeresult = "";

  $inputfile = $_SESSION["bpel2owfn"]["process"];
  $file = $process[$inputfile];
  $fakecall .= " -i ".$file["basename"];
  $realcall .= " ".$process[$inputfile]["residence"];
  
  $fakecall .= " -m petrinet -p ".$_SESSION[$tool]["patterns"]." -r ".$_SESSION[$tool]["reduce"]." -f ".$_SESSION[$tool]["format"];
  $realcall .= " -m petrinet -p ".$_SESSION[$tool]["patterns"]." -r ".$_SESSION[$tool]["reduce"]." -f ".$_SESSION[$tool]["format"];

  $fakeresult .= $process[$inputfile]["filename"];
//  $fakeresult .= ".owfn"; 
  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " --output=".$fakeresult;
  $realcall .= " --output=".$realresult;

  $realcall .= " -d2 2>&1";
  // end of building call strings
  
  if (!strcmp($_SESSION[$tool]["output"], "result")) {
    header("Content-type: text/plain");
    exec($realcall." > /dev/null");
    $lines = file($realresult.".owfn");

    foreach ($lines as $line_num => $line) {
        echo $line) . "\n";
    }

    exit;
  }
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live - Service Formalization</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><img src="resource/images/live.png" alt="service-technology.org/live" /></div>
      <h1>Service Formalization</h1>



      <h2>Parameters</h2>
      <ul>
        <li><strong>input file:</strong> <?=$process[$inputfile]["basename"]?>
          <?php
          switch($_SESSION[$tool]['input_type']) {
            case "example":   echo "example file"; break;
            case "uploaded":  echo "uploaded file"; break;
            case "url":       echo "downloaded file"; break;
            case "given":     echo "given file"; break;
          }
//          if (!strcmp($_SESSION['input_type'], 'url'))
//            echo ' (downloaded from <a href="'.$_SESSION["bpel2owfn"].'">'.$_SESSION["bpel2owfn"].'</a>)';
//          if (!strcmp($_SESSION['input_type'], 'example'))
//            echo ' (uploaded from local file '.basename($_SESSION["bpel2owfn"]).')';
          ?>
        </li>
        <li><strong>patterns:</strong> <?=$_SESSION[$tool]['patterns']?></li>
        <li><strong>file format:</strong> 
          <?php echo ((!strcmp($_SESSION[$tool]['format'], "owfn"))?'Fiona open net':'LoLA Petri net'); ?>
        </li>
        <li><strong>structural reduction level:</strong> <?=$_SESSION[$tool]['reduce']?></li>
      </ul>

      <h2>Call</h2>
      <?php  console($fakecall, $realcall.' '); ?>

      <h2>Result</h2>
      <?php drawImage($fakeresult.'.owfn'); ?>
      
      <?php
          echo "<p><li><a href=".getLink($fakeresult.'.owfn').">".basename($fakeresult)."</a></li></p>";
      ?>
    </div>
  </div>
  <div id="footer">
    <p><a href="http://service-technology.org">service-technology.org</a> is a cooperation between the <a  href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/">University of Rostock</a> and the <a  href="http://www2.informatik.hu-berlin.de/top/index.php">Humboldt-Universit&auml;t zu Berlin</a>.</p>
  </div>
</body>
</html>
