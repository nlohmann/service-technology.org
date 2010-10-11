<?php

  include 'resource/php/debug.php';

//  debug("before session import");

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
//  debug("after session import");
  $tool = "mia";
  
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';
  
  $inputfile = $_SESSION["mia"]["process"];
  $description = "an example service";
  $target = $process[$inputfile]["filename"];
  switch ($process[$inputfile]["filename"]) {
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
  
  $targetfile = current(prepareFile($tool."/".$target.".owfn"));
  $jtfile = current(createFile($process[$inputfile]["filename"].".jt"));
  
  $realcall = "mia ".$process[$inputfile]["residence"]." ".$targetfile["residence"]." -v -o ".$jtfile["residence"];
  $fakecall = "mia ".$process[$inputfile]["basename"] ." ".$targetfile["basename"]    ." -o ".$jtfile["basename"];

  // some services need a higher message bound
  if (!strcmp($process[$inputfile]["filename"], 'T2')) {
    $realcall .= " -m2";
    $fakecall .= " -m2";
  }  
  
  //$realcall .= " 2>&1";
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live &ndash; Instance Migration</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><a href="./#mia"><img src="resource/images/live.png" alt="service-technology.org/live" /></a></div>
      <h1>Instance Migration</h1>

      <p>This page demonstrates the calculation of a migration relation as described in a paper submitted to the <a href="http://icsoc.org/">ICSOC 2009</a> conference.</p>
        <p>Note that for some examples, results may take a few minutes.</p>



      <h2>Input Service Model: <?=$longname?></h2>
      <p>This service is <?=$description?></p>

      <?php drawImage($process[$inputfile]["basename"], 400, "source service"); ?>
      <?php drawImage($targetfile["basename"], 400, "target service"); ?>

      <p>We use <strong>open nets</strong> to model services. An open net is a place/transition net with an interface consisting of input places (orange) and output places (yellow). These places model asynchronous message passing.</p>



      <h2>Calculating the Migration Relation</h2>

      <p>To calculate the migration relation between the source service <strong><code><?=$process[$inputfile]["basename"]?></code></strong> and <strong><code><?=$targetfile["basename"]?></code></strong>, we use the tool <a href="http://service-technology.org/mia">Mia</a>. It implements all the steps described in the paper, namely:
      <ul>
        <li>calculation of the most-permissive partner of the source service</li>
        <li>composition of this most-permissive partner with the target service</li>
        <li>calculation of the maximal migration relation (i.e., jumper transitions) from the results above</li>
      </ul>
      The console below shows the output of Mia running on this server:
      </p>

      <?php
        $call_result = console($fakecall, $realcall);
        $count_jt = getNumber($call_result[1], "jumper transitions found");
        $count_statessource = getNumber($call_result[1], "inner markings");
        $count_statesmpp = getNumber($call_result[1], "mia: most-permissive partner");
        $count_searchspace = getNumber($call_result[1], "tuples for target service found") * getNumber($call_result[1], "parsed migration information");
        $count_statescomp = getNumber($call_result[1], "generated state space of composition");
      ?>
      
      <p>Note that this server has less memory and a slower CPU than the machine used for the experiments in the paper.</p>

      <h2>Jumper Transitions</h2>
      
      <p>The input service has <?=$count_statessource?> states and its most-permissive partner has <?=$count_statesmpp?> states. The composition between the target service and the most-permissive partner has <?=$count_statescomp?> states. From a theoretical search space of <?=$count_searchspace?>, <?=$count_jt?> feasible jumper transitions were found:</p>
      
      <p>
      <?php
      if ($count_jt < 30) {
          print("<blockquote><pre>");
          readfile($jtfile["residence"]);
          print("</pre></blockquote>");
      } else {
        print("download a file containing the <a href=".$jtfile["link"].">jumper transitions</a>");
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
             <li>source service: <a href="<?=$process[$inputfile]["link"]?>"><?=$process[$inputfile]["basename"]?></a></li>
             <li>target service: <a href="<?=$targetfile["link"]?>"><?=$targetfile["basename"]?></a></li>
          </ul>
        <li>Execute the following command: <code><?=$fakecall?></code>
        </li>
        <li>The migration relation (i.e., the jumper transitions) are written to file <code><?=$jtfile["basename"]?></code>.</li>
      </ul>
    </div>
  </div>
</body>

</html>
