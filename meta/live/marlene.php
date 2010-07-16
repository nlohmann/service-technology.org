<?php

  require 'resource/php/session.php';

  if ( ! isset($_REQUEST) && ! isset($_SESSION["marlene"]))
  {
    header('Location: index.html#marlene');
  }
  else
  {
    if ( isset($_REQUEST) )
    {
      if ( isset($_REQUEST["example"]) )
      {
        $_SESSION["marlene"] = $_REQUEST["example"];
        unset($_REQUEST);
        header("Location: ".$_SERVER["PHP_SELF"]);
      }
    }
  }

  require 'resource/php/files.php';

  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';
  include 'resource/php/getnumber.php';

  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="utf-8" ?>';

  if (!strcmp($_SESSION["marlene"], 'coffee1')) {
    $services = array("marlene/myCoffee.owfn", "marlene/myCustomer.owfn");
    $rules = array("marlene/coffee.ar");
  }

  if (!strcmp($_SESSION["marlene"], 'coffee2')) {
    $services = array("marlene/myCoffee-rep.owfn", "marlene/myCustomer-rep.owfn");
    $rules = array("marlene/coffee.ar");
  }
  
  $services = prepareFiles($services);
  $rules = prepareFiles($rules);


  // fiona -t adapter myCoffee.owfn myCustomer.owfn -a coffee.ar
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
	// system("petri --removePorts -o dot ".$file["residence"]);
        // dotimg("in=".urlencode($file["residence"]).".dot&amp;thumbnail_size=400&amp;label=".urlencode($file["basename"]));
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
        $call_result = console($fakecall, 'cd marlene; env; '.$realcall.' ');
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
