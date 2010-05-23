<?php
  if (empty($_REQUEST))
    header('Location:index.html#migration');

  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';
  include 'resource/php/getnumber.php';

  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';

  $inputfilename = $_REQUEST['service'];
    
  // give a verbose description of the services
  $longname = "";
  $description = 'a WS-BPEL process which has been translated into a Petri net model using the tool <a href="http://service-technology.org/bpel2owfn">BPEL2oWFN</a>.';
  $target = $inputfilename.'-pv';
  switch ($inputfilename) {
    case "service-a":
      $longname="Running Example";
      $description="the running example used in the ICSOC submission (Fig. 1-3).";
      $target="service-b";
      break;
    case "service-d":
      $longname="Technical Example 1";
      $description="a technical example used in the ICSOC submission (Fig. 4(a)-(c)).";
      $target="service-e";
      break;
    case "service-f":
      $longname="Technical Example 2";
      $description="a technical example used in the ICSOC submission (Fig. 4(d)-(f)).";
      $target="service-g";
      break;
    case "reg_old":
      $longname="Registration Service";
      $description="a small example used to demonstrate substitutability.";
      $target="reg_new";
      break;
    case "AP": $longname="Passport Application"; break;
    case "BH": $longname="Internal Order"; break;
    case "CN": $longname="Contract Negotiation"; break;
    case "DG": $longname="Deliver Finished Goods"; break;
    case "PO": $longname="Purchase Order"; break;
    case "T2": $longname="Travel Service"; break;
    case "TR": $longname="Ticket Reservation"; break;
  }
  
  
  $call_1 = 'mia '.$inputfilename.'.owfn '.$target.'.owfn -v -o '.$inputfilename.'.jt';
  
  // some services need a higher message bound
  if (!strcmp($inputfilename, 'T2')) {
    $call_1 = $call_1.' -m2';
  }  
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>Another Approach to Service Instance Migration</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Another Approach to Service Instance Migration</h1>

      <p>This page demonstrates the calculation of a migration relation as described in a paper submitted to the <a href="http://icsoc.org/">ICSOC 2009</a> conference.</p
        <p>Note that for some examples, results may take a few minutes.</p>



      <h2>Input Service Model: <?=$longname?></h2>
      <p>This service is <?=$description?></p>

      <?php system('cd migration; petri '.$inputfilename.'.owfn -odot'); ?>
      <?php dotimg('in=migration/'.$inputfilename.'.owfn.dot&thumbnail_size=400&label='.urlencode("source service")); ?>
      <?php system('cd migration; petri '.$target.'.owfn -odot'); ?>
      <?php dotimg('in=migration/'.$target.'.owfn.dot&thumbnail_size=400&label='.urlencode("target service")); ?>

      <p>We use <strong>open nets</strong> to model services. An open net is a place/transition net with an interface consisting of input places (orange) and output places (yellow). These places model asynchronous message passing.</p>



      <h2>Calculating the Migration Relation</h2>

      <p>To calculate the migration relation between the source service <strong><code><?=$inputfilename?>.owfn</code></strong> and <strong><code><?=$target?>.owfn</code></strong>, we use the tool <a href="http://service-technology.org/mia">Mia</a>. It implements all the steps described in the paper, namely:
      <ul>
        <li>calculation of the most-permissive partner of the source service</li>
        <li>composition of this most-permissive partner with the target service</li>
        <li>calculation of the maximal migration relation (i.e., jumper transitions) from the results above</li>
      </ul>
      The console below shows the output of Mia running on this server:
      </p>

      <?php
        $call_result = console($call_1, 'cd migration; '.$call_1);
        $count_jt = getNumber($call_result[1], "jumper transitions found");
        $count_statessource = getNumber($call_result[1], "inner markings");
        $count_statesmpp = getNumber($call_result[1], "mia: most-permissive partner");
        $count_searchspace = getNumber($call_result[1], "tuples for target service found") * getNumber($call_result[1], "parsed migration information");
        $count_statescomp = getNumber($call_result[1], "generated state space of composition")
      ?>
      
      <p>Note that this server has less memory and a slower CPU than the machine used for the experiments in the paper.</p>

      <h2>Jumper Transitions</h2>
      
      <p>The input service has <?=$count_statessource?> states and its most-permissive partner has <?=$count_statesmpp?> states. The composition between the target service and the most-permissive partner has <?=$count_statescomp?> states. From a theoretical search space of <?=$count_searchspace?>, <?=$count_jt?> feasible jumper transitions were found:</p>
      
      <p>
      <?php
      if ($count_jt < 30) {
          print("<blockquote><pre>");
          include("migration/".$inputfilename.".jt");
          print("</pre></blockquote>");
      } else {
        print("download a file containing the <a href='migration/".$inputfilename.".jt'>jumper transitions</a>");
      }
      ?>
      </p>

      <h2>Summary</h2>

      <p>To reproduce these results on your own machine, proceed as follows:</p>
      <ul>
        <li>Preparations:
          <ul>
            <li>Download and set up the tool <a href="http://service-technology.org/mia">Mia</a> to calculate the migration relation.</li>
            <li>Download and set up the tool <a href="http://service-technology.org/wendy">Wendy</a> which is needed by Mia to calculate a most-permissive partner.</li>
            <li>Download and set up the tool <a href="http://service-technology.org/lola">LoLA</a> which is needed by Mia to calculate the compositions.</li>
          </ul>
        </li>
        <li>Download the service models &ldquo;<?=$longname?>&rdquo;:
          <ul>
             <li>source service: <a href="migration/<?=$inputfilename?>.owfn"><?=$inputfilename?>.owfn</a></li>
             <li>target service: <a href="migration/<?=$target?>.owfn"><?=$target?>.owfn</a></li>
          </ul>
        <li>Execute the following command: <code><?=$call_1?></code>
        </li>
        <li>The migration relation (i.e., the jumper transitions) are written to file <code><?=$inputfilename.'.jt'?></code>.</li>
      </ul>
    </div>
  </div>
</body>

</html>
