<?php
  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  $tool = "rachel";

  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // prepare example
  switch ($_SESSION['rachel']['process']) {
    case "rachel/running_example.owfn":
      $automata = array("rachel/running_example.sa");
      $og = array("rachel/running_example.og");
      break;
    case "rachel/auction_service.owfn":
      $automata = array("rachel/auction_service.sa");
      $og = array("rachel/auction_service.og");
      break;
    case "rachel/car_return.owfn":
      $automata = array("rachel/car_return.sa");
      $og = array("rachel/car_return.og");
      break;
    case "rachel/credit_preparation.owfn":
      $automata = array("rachel/credit_preparation.sa");
      $og = array("rachel/credit_preparation.og");
      break;
    case "rachel/customer_service.owfn":
      $automata = array("rachel/customer_service.sa");
      $og = array("rachel/customer_service.og");
      break;
    case "rachel/loan_approval.owfn":
      $automata = array("rachel/loan_approval.sa");
      $og = array("rachel/loan_approval.og");
      break;
    case "rachel/online_shop.owfn":
      $automata = array("rachel/online_shop.sa");
      $og = array("rachel/online_shop.og");
      break;
    case "rachel/order_process.owfn":
      $automata = array("rachel/order_process.sa");
      $og = array("rachel/order_process.og");
      break;
    case "rachel/purchase_order.owfn":
      $automata = array("rachel/purchase_order.sa");
      $og = array("rachel/purchase_order.og");
      break;
    case "rachel/register_request.owfn":
      $automata = array("rachel/register_request.sa");
      $og = array("rachel/register_request.og");
      break;
    case "rachel/supply_order.owfn":
      $automata = array("rachel/supply_order.sa");
      $og = array("rachel/supply_order.og");
      break;
  }

  // copy files to temporary directory, see files.php for details
  $automata = prepareFiles($automata);
  $automata = current($automata);
  $og = prepareFiles($og);
  $og = current($og);

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "rachel -m ".$_SESSION['rachel']['mode']." -a ".$automata["basename"]." -o ".$og["basename"]." -v -d";
  $realcall = "rachel -m ".$_SESSION['rachel']['mode']." -a ".$automata["residence"]." -o ".$og["residence"]." -v -d";

  $realcall .= " -v 2>&1";

/*
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
      drawImage($automata["basename"]);

      drawImage($og["basename"]);
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
