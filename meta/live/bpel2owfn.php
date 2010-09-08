<?php

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';

  // do not call the page without POST request, or only if session is 
  // already set to Marlene (all information about services available)
  if ( ! isset($_REQUEST) && ! isset($_SESSION["bpel2owfn"]))
  {
    // direct call of this page -> return to main page
    header('Location: index.html#bpel2owfn');
    exit;
  }
  else
  {
    // new request?
    if ( isset($_REQUEST) )
    {
      $set = false;
      
      if ( ! strcmp($_REQUEST["input_type"], 'example') )
      {
        // remember name of example in session
        $_SESSION["bpel2owfn"] = $_REQUEST["input_example"];
        $set = true;
      }
      if ( ! strcmp($_REQUEST["input_type"], 'uploaded') )
      {
        $_SESSION["bpel2owfn"] = $_REQUEST["input_uploaded"];
        $set = true;
      }
      if ( ! strcmp($_REQUEST["input_type"], 'url') )
      {
        $_SESSION["bpel2owfn"] = $_REQUEST["input_url"];
        $set = true;
      }
      if ( ! strcmp($_REQUEST["input_type"], 'given') )
      {
        $_SESSION["bpel2owfn"] = $_REQUEST["input_given"];
        $set = true;
      }
      
      if ($set)
      {
        $_SESSION["input_type"] = $_REQUEST["input_type"];
        $_SESSION["patterns"] = $_REQUEST["patterns"];
        $_SESSION["format"] = $_REQUEST["format"];
        $_SESSION["reduce"] = $_REQUEST["reduce"];
        
        unset($_REQUEST);
        // redirect to self, make back/forward buttons work without 
        // resending the request
        header("Location: ".$_SERVER["PHP_SELF"]);
      }
    }
  }

  if ( ! isset($_SESSION["bpel2owfn"]))
  {
    // direct call of this page -> return to main page
    header('Location: index.html#bpel2owfn');
    exit;
  }

  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // copied from Wendy ;)
  include_once 'resource/php/console.php';
  include_once 'resource/php/dotimg.php';
  include_once 'resource/php/getnumber.php';

  // output header
  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="utf-8" ?>';

  // prepare example
//  if (!strcmp($_SESSION["marlene"], 'coffee1')) {
//    $services = array("marlene/myCoffee.owfn", "marlene/myCustomer.owfn");
//    $rules = array("marlene/coffee.ar");
//  }
//
//  if (!strcmp($_SESSION["marlene"], 'coffee2')) {
//    $services = array("marlene/myCoffee-rep.owfn", "marlene/myCustomer-rep.owfn");
//    $rules = array("marlene/coffee.ar");
//  }
  
  // copy files to temporary directory, see files.php for details
//  $services = prepareFiles($services);
//  $rules = prepareFiles($rules);
  
  $process = $_SESSION["bpel2owfn"];
  
  if ( ! strcmp($_SESSION["input_type"], 'example') )
  {
    $process = prepareFile('bpel2owfn/'.$process.'.bpel'); 
  }
  

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "bpel2owfn";
  $realcall = "bpel2owfn";
  $fakeresult = "";

  $fakecall .= " -i ".$process["basename"];
  $realcall .= " ".$process["residence"];
  
  $fakecall .= " -m petrinet -p ".$_SESSION["patterns"]." -r ".$_SESSION["reduce"]." -f ".$_SESSION["format"];
  $realcall .= " -m petrinet -p ".$_SESSION["patterns"]." -r ".$_SESSION["reduce"]." -f ".$_SESSION["format"];

  $fakeresult .= $process["filename"];
  $fakeresult .= ".owfn"; 
//  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " -o ".$fakeresult;
  $realcall .= " -o ".$realresult;

//  $realcall .= " -v 2>&1";
  // end of building call strings
  
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>BAAS: BPEL2oWFN as a Service</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
  
  <h1>BPEL2oWFN</h1>

  <h2>Parameters</h2>

  <ul>
    <li><strong>input file:</strong> <?=$process["basename"]?>
      <?php
        if (!strcmp($_SESSION['input_type'], 'url'))
          echo ' (downloaded from <a href="'.$process["link"].'">'.$process["link"].'</a>)';
        if (!strcmp($_SESSION['input_type'], 'file'))
          echo ' (uploaded from local file '.$process["basename"].')';
      ?>
    </li>
    <li><strong>patterns:</strong> <?=$_SESSION['patterns']?></li>
    <li><strong>file format:</strong> 
      <?php echo ((!strcmp($_SESSION['format'], "owfn"))?'Fiona open net':'LoLA Petri net'); ?>
    </li>
    <li><strong>structural reduction level:</strong> <?=$_SESSION['reduce']?></li>
  </ul>
  
  <h2>Result</h2>
  
  <?php 
  
    console($fakecall, $realcall.' ');

    drawImage($fakeresult);
  
  ?>
  
  <p>
  	<a href="./#bpel2owfn" title="back to reality">Back to live</a>
  </p>
  
  </div>
  </div>
</body>
</html>
