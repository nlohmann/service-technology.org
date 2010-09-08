<?php

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  $tool = "marlene";
  
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  print_r ($_SESSION);
  // prepare example
  if (!strcmp($_SESSION["marlene"], 'coffee1')) {
    $services = array("marlene/myCoffee.owfn", "marlene/myCustomer.owfn");
    $rules = array("marlene/coffee.ar");
  }

  if (!strcmp($_SESSION["marlene"], 'coffee2')) {
    $services = array("marlene/myCoffee-rep.owfn", "marlene/myCustomer-rep.owfn");
    $rules = array("marlene/coffee.ar");
  }
  
  // copy files to temporary directory, see files.php for details
  $services = prepareFiles($services);
  $rules = prepareFiles($rules);

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "marlene";
  $realcall = "marlene";
  $fakeresult = "";
  $first = true;

  foreach($services as $info)
  {
    $fakecall .= " ".$info["basename"];
    $realcall .= " ".$info["residence"];
    if ($first == true)
    {
      $first = false;
    }
    else
    {
      $fakeresult .= "_";
    }
    $fakeresult .= $info["filename"];
  }
  $fakeresult .= ".owfn"; 
  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " -o ".$fakeresult;
  $realcall .= " -o ".$realresult;

  $rulefile = current($rules);
  $fakecall .= " -r ".$rulefile["basename"];
  $realcall .= " -r ".$rulefile["residence"];

  $realcall .= " -v 2>&1";
  // end of building call strings

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
      <h1>Adapter Synthesis</h1>

      <h2>Parameters</h2>
      <ul>
        <li><strong>input file:</strong> 
        <?php
          foreach($services as $file)
          {
            echo $file["basename"]." ";
          }
          ?>
          <?php
          
          switch($_SESSION['input_type']) {
            case "example":   echo "example file(s)"; break;
            case "uploaded":  echo "uploaded file(s)"; break;
            case "url":       echo "downloaded file(s)"; break;
            case "given":     echo "given file(s)"; break;
          }
          ?>
        </li>
      </ul>

      <h2>Input Service Models</h2>

      <!-- draw images of services -->
      <?php
      foreach($services as $file)
      {
        drawImage($file["basename"]);
      }
      ?>


      <!-- provide links for downloading services -->
      <ul>
      <?php
        foreach($services as $info)
        {
          echo "<li><a href=".$info["link"].">".$info["basename"]."</a></li>";
        }
      ?>
      </ul>

      <h2>Call</h2>

      <?php
        // call console
        $call_result = console($fakecall, 'cd marlene; '.$realcall.' ');
      ?>

      <h2>Synthesized Adapter</h2>

      <!-- draw image of adapter -->
      <?php drawImage($fakeresult); ?>

      <!-- provide link for downloading adapter -->
      <ul>
      <?php
          echo "<li><a href=".getLink($fakeresult).">".basename($fakeresult)."</a></li>";
      ?>
      </ul>

      <p>
      	<a href="./#marlene" title="back to reality">Back to live</a>
      </p>
    </div>
  </div>
</body>

</html>

