<?php
  if (empty($_REQUEST))
    header('Location:index.html#rachel');

  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';

  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';

  $choreography = $_REQUEST['chor'];
  $editdistance = $_REQUEST['mode'];

  $call_1 = 'rachel -m og -o '.$choreography.'.og -d';  
  $call_2 = 'rachel -m sa -a '.$choreography.'.sa -d';  
  $call_3 = 'rachel -m '.$editdistance.' -a '.$choreography.'.sa -o '.$choreography.'.og -d';  
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>RAAS: Rachel as a Service</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Rachel</h1>

      <h2>Input</h2>

      <?php system('cd rachel; '.$call_1.' 2> /dev/null'); ?>
      <?php dotimg('in=rachel/'.$choreography.'.sa.dot&label='.urlencode("service automaton")); ?>
      <?php system('cd rachel; '.$call_2.' 2> /dev/null'); ?>
      <?php dotimg('in=rachel/'.$choreography.'.og.dot&label='.urlencode("operating guideline")); ?>

      <h2>Result</h2>
      
      <?php $call_result = console($call_3, 'cd rachel; '.$call_3); ?>
      
      <?php dotimg('in=rachel/'.$choreography.'.sa_'.$choreography.'.og_'.$editdistance.'.dot&label='.urlencode("edit distance")); ?>
      
    </div>
  </div>
</body>

</html>
