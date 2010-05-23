<?php
  if (empty($_REQUEST))
    header('Location:index.html#pnog');

  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';
  include 'resource/php/getnumber.php';

  header("Content-Type: text/html");
  echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';

  $inputfilename = $_REQUEST['service'];
    
  // give a verbose description of the services
  $longname = "";
  $description = 'a WS-BPEL process which has been translated into a Petri net model using the tool <a href="http://service-technology.org/bpel2owfn">BPEL2oWFN</a>.';
  switch ($inputfilename) {
    case "RE":
      $longname="Running Example";
      $description="the running example used in the ACSD submission (Fig. 1).";
      break;
    case "SD":
      $longname="Technical Example";
      $description="a technical example used in the ACSD submission (Fig. 3).";
      break;
    case "myCoffee":
      $longname="Coffee Vending Machine";
      $description="a simple service modelling a coffee vending machine.";
      break;
    case "phcontrol3.unf":
      $longname="Dining Philosophers";
      $description="a service modeling three dining philosophers.";
      break;
    case "auctionService":
      $longname="Auction Service";
      break;
    case "loanApprovalProcess":
      $longname="Loan Approval Process";
      break;
    case "AP": $longname="Passport Application"; break;
    case "CN": $longname="Contract Negotiation"; break;
    case "DG": $longname="Deliver Finished Goods"; break;
    case "BH": $longname="Internal Order"; break;
    case "OS": $longname="Online Shop"; break;
    case "QR": $longname="Quotation Requisition"; break;
    case "PO": $longname="Purchase Order"; break;
    case "RS": $longname="Reservations"; break;
    case "T2": $longname="Travel Service"; break;
    case "TR": $longname="Ticket Reservation"; break;
  }
  
  
  $call_1 = 'fiona '.$inputfilename.'.owfn -t og -R';
  
  // some services need a higher message bound
  if (!strcmp($inputfilename, 'T2')) {
    $call_1 = $call_1.' -m2';
  }
  
  $call_2a = 'rachel -m annotation -o '.$inputfilename.'.owfn.R.og -d';
  $call_2b = 'rachel -m og -o '.$inputfilename.'.owfn.R.og -d';  
  $call_3 = 'fiona '.$inputfilename.'.owfn.R.og -t mostpermissivepartner';  
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>Petrifying Operating Guidelines</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Petrifying Operating Guidelines</h1>

      <p>This page demonstrates the calculation of a Petri net operating guideline as described in the paper <a href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/staff/publications-complete.htm#LohmannW_2009_acsd">Petrifying Operating Guidelines for Services</a> accepted at the <a href="http://www.informatik.uni-augsburg.de/acsd/">ACSD 2009</a> conference. The translation is performed in three steps:</p>
      <ol>
        <li>The <strong>operating guideline</strong> of the input service is calculated using the tool Fiona. The operating guideline is a service automaton whose states are annotated with Boolean formulae.</li>
        <li>An <strong>implicit representation</strong> of the formulae is calculated using the tool Rachel. The implicit annotation consists of sets of states from which the original formulae can be reconstructed.</li>
        <li>The structure of the operating guideline (i.e. the service automaton without annotated formulae) is translated into a <strong>Petri net</strong> using Fiona. Fiona is using Petrify as backend to implement the region theory.</li>
      </ol>
      <p>Below, the steps are described together with the calls to the respective tools and the intermediate results. Note that for some results may take a few minutes.</p>



      <h2>Input Service Model: <?=$longname?></h2>
      <p>This service is <?=$description?></p>

      <?php system('cd pnog; fiona -t png '.$inputfilename.'.owfn -p no-png &> /dev/null'); ?>
      <?php dotimg('in=pnog/'.$inputfilename.'.owfn.out&thumbnail_size=400&label='.urlencode("input service")); ?>

      <p>We use <strong>open nets</strong> to model services. An open net is a place/transition net with an interface consisting of input places (orange) and output places (yellow). These places model asynchronous message passing.</p>



      <h2>Step 1: Calculate Operating Guidelines</h2>

      <p>To calculate the operating guideline, Fiona reads the service from file <strong><code><?=$inputfilename?>.owfn</code></strong>. It then calculates the operating guideline and writes it into file <strong><code><?=$inputfilename?>.owfn.R.og</code></strong>.</p>

      <?php
        $call_result = console($call_1, 'cd pnog; '.$call_1.' -p no-dot');
        $finalcount_bluenodes = getNumber($call_result[1], "number of blue nodes");
        $finalcount_blueedges = getNumber($call_result[1], "number of blue edges");
      ?>

      <p>Fiona further creates a graphical representation of the operating guidelines. Each state is annotated with a Boolean formula, and each edge is labeled with the name of a message channel. Thereby, a trailing exclamation mark denotes sending of a message, and a trailing question mark denotes receiving of a message.</p>

      <?php system('cd pnog; '.$call_2b.' 2> /dev/null'); ?>
      <?php dotimg('in=pnog/'.$inputfilename.'.owfn.R.og.dot&label='.urlencode("operating guideline")); ?>



      <h2>Step 2: Implicit Representation of the Annotated Formulae</h2>

      <p>The implicit representation of the annotated formulae are calculated by Rachel, reading the operating guideline from file <strong><code><?=$inputfilename?>.owfn.R.og</code></strong>. It returns statistics about the size of the sets S and F, as well as about the further reduced sets S1, S2 and F'. See the paper submission for further details.</p>

      <?php
        $call_result = console($call_2a, 'cd pnog; '.$call_2a);
        $finalcount_s = getNumber($call_result[1], "|S|");
        $finalcount_f = getNumber($call_result[1], "|F|");
        $finalcount_s1 = getNumber($call_result[1], "|S1|");
        $finalcount_s2 = getNumber($call_result[1], "|S2|");
        $finalcount_fp = getNumber($call_result[1], "|F'|");
      ?>

      <p>Rachel further creates a graphical representation of the implicit representations by coloring the states of the operating guideline according to which set they belong:</p>

      <ul>
        <li>A white state is not element of any set.</li>
        <li>A green state is member of the set F or F', respectively.</li>
        <li>A blue state is member of the set S or S1, respectively.</li>
        <li>A violet state is member of the set S2.</li>
      </ul>

      <p>Note that the set S2 is usually empty, and the number of states that belong to any set is small compared to the number of overall states.</p>

      <?php dotimg('in=pnog/'.$inputfilename.'.owfn.R.og.bits1.dot&label='.urlencode("operating guideline with implicit annotations")); ?>
      <?php dotimg('in=pnog/'.$inputfilename.'.owfn.R.og.bits2.dot&label='.urlencode("operating guideline with reduced implicit annotations")); ?>



      <h2>Step 3: Representing the Operating Guidelines as Petri Net</h2>

      <p>To further reduce the space needed to store an operating guideline, its underlying service automaton is translated into a Petri net using region theory. Fiona reads the operating guideline from file <strong><code><?=$inputfilename?>.owfn.R.og</code></strong>, calls the tool Petrify as backend and creates a Petri net stored in file <strong><code><?=$inputfilename?>-partner.owfn</code></strong>.</p>

      <?php
        $call_result = console($call_3, 'cd pnog; '.$call_3.' -d2 -p no-png');
        
        if ($call_result[0] == 0) {
          $finalcount_places = getNumber($call_result[1], "|P|");
          $finalcount_transitions = getNumber($call_result[1], "|T|", 4);
          $finalcount_arcs = getNumber($call_result[1], "|F|", 5);          
        }
      ?>

      <?php
      if ($call_result[0] == 0) {
        dotimg('in=pnog/'.$inputfilename.'-partner.dot&thumbnail_size=400&label='.urlencode("Petri net operating guideline"));
      } else {
        echo '<p>For this example, Petrify ran out of memory. Note that this server only has 1 GB of memory. On machines with more memory, Petrify might be able to finish its calculations.</p>';
        $finalcount_places = "?";
        $finalcount_transitions = "?";
        $finalcount_arcs = "?";
      }
      ?>

      <p>If the translation succeeds, a graphical representation of the synthesized Petri net is created. Again, the Petri net has an interface, consisting of input places (orange) and output places (yellow) modeling asynchronous message exchange.</p>

      <p>The set of states calculated in step 2 can likewise be represented by a set of markings.</p>



      <h2>Conclusion</h2>
      
      <ol>
        <li>The operating guideline of this example consists of <strong><?=$finalcount_bluenodes?>&nbsp;states</strong>, <strong><?=$finalcount_blueedges?>&nbsp;edges</strong>, and <strong><?=$finalcount_bluenodes?>&nbsp;Boolean formulae</strong> (one for each state).</li>
        <li>The formulae can be implicitly represented by state sets:  instead of storing <?=$finalcount_bluenodes?>&nbsp;Boolean formulae, two sets consisting of only <strong><?=($finalcount_s+$finalcount_f)?>&nbsp;elements</strong> in total need to be stored. With the reduced implicit representation,  <strong><?=($finalcount_s1+$finalcount_s2+$finalcount_fp)?>&nbsp;elements</strong> are needed.</li>
        <li>To compact the structure of the operating guideline, it can be translated into a Petri net. This net consists of <strong><?=$finalcount_places?>&nbsp;places, <?=$finalcount_transitions?>&nbsp;transitions, and <?=$finalcount_arcs?>&nbsp;arcs</strong > compared to the <?=$finalcount_bluenodes?>&nbsp;states and <?=$finalcount_blueedges?>&nbsp;edges of the original operating guideline's structure.</li>
      </ol>

      <h2>Summary</h2>

      <p>To reproduce these results on your own machine, proceed as follows:</p>
      <ul>
        <li>Preparations:
          <ul>
            <li>Download and set up the tools <a href="http://service-technology.org/fiona">Fiona</a> and <a href="http://service-technology.org/rachel">Rachel</a>.</li>
            <li>To create the Petri net operating guideline, the tool <a href="http://www.lsi.upc.edu/~jordicf/petrify/distrib/home.html">Petrify</a> is needed.</li>
            <li>To create the graphics, the tool <a href="http://www.graphviz.org/">Graphviz dot</a> is needed.</li>
          </ul>
        </li>
        <li>Download the input service model &ldquo;<?=$longname?>&rdquo; <a href="pnog/<?=$inputfilename?>.owfn"><?=$inputfilename?>.owfn</a>.</li>
        <li>Enter the following commands:
          <ul>
            <li><code><?=$call_1?></code></li>
            <li><code><?=$call_2a?></code></li>
            <li><code><?=$call_3?></code></li>
          </ul>
        </li>
      </ul>
    </div>
  </div>
</body>

</html>
