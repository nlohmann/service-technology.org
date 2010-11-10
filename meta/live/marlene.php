<?php

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  $tool = "marlene";
  
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // prepare example
  if (!strcmp($_SESSION["marlene"]["process"], 'marlene/coffee1.owfn')) {
    $services = array("marlene/myCoffee.owfn", "marlene/myCustomer.owfn");
    $rules = array("marlene/coffee.ar");
  }

  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/coffee2.owfn')) {
    $services = array("marlene/myCoffee-rep.owfn", "marlene/myCustomer-rep.owfn");
    $rules = array("marlene/coffee.ar");
  }
  
  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/coffee3.owfn')) {
    $services = array("marlene/coffee_candy.owfn", "marlene/td_candy.owfn");
    $rules = array("marlene/td_candy.ar");
  }
  
  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/coffee4.owfn')) {
    $services = array("marlene/pro.owfn", "marlene/req.owfn");
    $rules = array("marlene/proreq.ar");
  }
  
  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/car.owfn')) {
    $services = array("marlene/Reservations.owfn", "marlene/ReservationsCustomer.owfn");
    $rules = array("marlene/reservation.ar");
  }
  
  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/ticket.owfn')) {
    $services = array("marlene/Ticket-Schalter.owfn", "marlene/Ticket-Schalter-partner.owfn");
    $rules = array("marlene/Ticket-Schalter.ar");
  }
  
  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/collapse.owfn')) {
    $services = array("marlene/shop.owfn", "marlene/customer.owfn");
    $rules = array("marlene/collapse.ar");
  }
  
  else if (!strcmp($_SESSION["marlene"]["process"], 'marlene/reroute.owfn')) {
    $services = array("marlene/ip.owfn", "marlene/ip2.owfn", "marlene/req.owfn");
    $rules = array("marlene/rerouting.ar");
  }
  
  // copy files to temporary directory, see files.php for details
  $services = prepareFiles($services);
  $rules = prepareFiles($rules);

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "marlene";
  $realcall = "marlene";
  
  if (isset($_SESSION[$tool]["costopt"]) && $_SESSION[$tool]["costopt"] == "yes")
  {
    $fakecall .= " --costoptimized";
    $realcall .= " --costoptimized";
  }
  
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
  /*
  if (isset($_SESSION[$tool]["costopt"]) && $_SESSION[$tool]["costopt"] == "yes")
  {
    $fakeresult .= "_costopt";
  }
  */
  $fakeresult .= ".owfn"; 
  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " -o ".$fakeresult;
  $realcall .= " -o ".$realresult;

  $rulefile = current($rules);
  $fakecall .= " -r ".$rulefile["basename"];
  $realcall .= " -r ".$rulefile["residence"];

  $realcall .= " -v";
  // end of building call strings

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live &ndash; Adapter Synthesis</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><a href="./#marlene"><img src="resource/images/live.png" alt="service-technology.org/live" /></a></div>
      <h1>Adapter Synthesis</h1>

      <!--
      <pre>
      <?php // print_r($_SESSION); ?>
      </pre>
      -->
      <h2>Parameters</h2>
      <ul>
        <li><strong>input file:</strong> 
        <?php
          foreach($services as $file)
          {
            echo '<a href="'.$file["link"].'" target="_blank">'.$file["basename"]."</a> ";
          }
          echo '<a href="'.$rulefile["link"].'" target="_blank">'.$rulefile["basename"]."</a> ";
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
      <!--
      <ul>
      <?php
        foreach($services as $info)
        {
          echo "<li><a href=".$info["link"].">".$info["basename"]."</a></li>";
        }
        echo "<li><a href=".$rulefile["link"].">".$rulefile["basename"]."</a></li>";
      ?>
      </ul>
      -->

      <h2>Call</h2>

      <?php
        // call console
        $call_result = console($fakecall, 'cd marlene; '.$realcall.' ');
      ?>

      <h2>Synthesized Adapter</h2>

      <!-- draw image of adapter -->
      <?php drawImage($fakeresult, 300 , (isset($_SESSION[$tool]["costopt"]) && $_SESSION[$tool]["costopt"] == "yes")?"Cost-optimized adapter":"Adapter"); ?>

      <!-- provide link for downloading adapter -->
      <ul>
      <?php
          echo "<li><a href=".getLink($fakeresult).">".basename($fakeresult)."</a></li>";
      ?>
      </ul>

    </div>
  </div>
  <div id="footer">
    <p><a href="http://service-technology.org">service-technology.org</a> is a cooperation between the <a  href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/">University of Rostock</a> and the <a  href="http://www2.informatik.hu-berlin.de/top/index.php">Humboldt-Universit&auml;t zu Berlin</a>.</p>
  </div>
</body>

</html>

