<?php

  // most important script! sets session information and PATH!
  require 'resource/php/session.php';

  // do not call the page without POST request, or only if session is 
  // already set to Marlene (all information about services available)
  if ( ! isset($_REQUEST) && ! isset($_SESSION["marlene"]))
  {
    // direct call of this page -> return to main page
    header('Location: index.html#marlene');
    exit;
  }
  else
  {
    // new request?
    if ( isset($_REQUEST) )
    {
      // variable example set in request?
      if ( isset($_REQUEST["example"]) )
      {
        // remember name of example in session
        $_SESSION["marlene"] = $_REQUEST["example"];
        // interactive example?
        if (!strcmp($_REQUEST["example"], "iRules") || !strcmp($_REQUEST["example"], "iCoffee"))
        {
          // redirect to interactive example
          header('Location: marlene_interactive.php');
          exit;
        }
        unset($_REQUEST);
        // redirect to self, make back/forward buttons work without 
        // resending the request
        header("Location: ".$_SERVER["PHP_SELF"]);
      }
    }
  }

  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require 'resource/php/files.php';

  // copied from Wendy ;)
  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';
  include 'resource/php/getnumber.php';

  // output header
  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="utf-8" ?>';

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

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>Adapter Synthesis <?php echo $_SESSION["uid"]; ?></title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Adapter Synthesis</h1>

      <h2>Input Service Models</h2>

      <?php
      foreach($services as $file)
      {
        drawImage($file["basename"]);
      }
      ?>


      <ul>
<?php
  foreach($services as $info)
  {
    echo "<li><a href=".$info["link"].">".$info["basename"]."</a></li>";
  }
?>
      </ul>

      <h2>Synthesized Adapter</h2>

      <?php
        $call_result = console($fakecall, 'cd marlene; '.$realcall.' ');
      ?>

      <?php drawImage($fakeresult); ?>
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
