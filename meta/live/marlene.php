<?php
  if (empty($_REQUEST))
    header('Location:index.html#marlene');

  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';
  include 'resource/php/getnumber.php';

  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';

  $example = $_REQUEST['example'];
  
  if (!strcmp($example, 'coffee1')) {
    $service1 = 'myCoffee';
    $service2 = 'myCustomer';
    $rules = 'coffee.ar';
  }

  if (!strcmp($example, 'coffee2')) {
    $service1 = 'myCoffee-rep';
    $service2 = 'myCustomer-rep';
    $rules = 'coffee.ar';
  }
  
  // fiona -t adapter myCoffee.owfn myCustomer.owfn -a coffee.ar
  $call = 'marlene '.$service1.'.owfn '.$service2.'.owfn  -o '.$service1.'_'.$service2.'_result.owfn -v -r '.$rules;
  
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>Adapter Synthesis</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Adapter Synthesis</h1>

      <h2>Input Service Models</h2>

      <?php system('cd marlene; fiona -t png '.$service1.'.owfn -p no-png &> /dev/null'); ?>
      <?php system('cd marlene; fiona -t png '.$service2.'.owfn -p no-png &> /dev/null'); ?>
      <?php dotimg('in=marlene/'.$service1.'.owfn.out&thumbnail_size=400&label='.urlencode("service 1")); ?>
      <?php dotimg('in=marlene/'.$service2.'.owfn.out&thumbnail_size=400&label='.urlencode("service 2")); ?>


      <h2>Synthesized Adapter</h2>

      <?php
        $call_result = console($call, 'cd marlene; '.$call.' ');
      ?>

      <?php system('cd marlene; fiona -t png '.$service1.'_'.$service2.'_result.owfn -p no-png &> /dev/null'); ?>
      <?php dotimg('in=marlene/'.$service1.'_'.$service2.'_result.owfn.out&thumbnail_size=400&label='.urlencode("adapter")); ?>

      <p>
      	<a href="./#marlene" title="back to reality">Back to live</a>
      </p>
    </div>
  </div>
</body>

</html>
