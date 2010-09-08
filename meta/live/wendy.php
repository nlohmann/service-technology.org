<?php

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  
  if (!empty($_REQUEST))
  {
    $_SESSION['mode'] = (!strcmp($_REQUEST['mode'], 'ig'))?'sa':'og';
    if (isset($_REQUEST['reduction']))
      $_SESSION['reduction'] = $_REQUEST['reduction'];
    $_SESSION['reduce_wait'] = (isset($_REQUEST['reduce_wait']))?' --waitstatesOnly':'';
    $_SESSION['reduce_reic'] = (isset($_REQUEST['reduce_reic']))?' --receivingBeforeSending':'';
    $_SESSION['reduce_sequ'] = (isset($_REQUEST['reduce_sequ']))?' --seqReceivingEvents':'';
    $_SESSION['reduce_succ'] = (isset($_REQUEST['reduce_succ']))?' --succeedingSendingEvent':'';
    $_SESSION['reduce_quit'] = (isset($_REQUEST['reduce_quit']))?' --quitAsSoonAsPossible':'';
  }
  
  $tool = "wendy";
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  // prepare strings for system call (realcall) 
  // and output on console (fakecall)
  $fakecall = "wendy ";
  $realcall = "wendy ";
  $fakeresult = "";

  $fakecall .= $process[$_SESSION['wendy']]["basename"];
  $realcall .= $process[$_SESSION['wendy']]["residence"];
  
  $fakecall .= $_SESSION['reduce_wait'].$_SESSION['reduce_reic'].$_SESSION['reduce_sequ'].$_SESSION['reduce_succ'].$_SESSION['reduce_quit'];
  $realcall .= $_SESSION['reduce_wait'].$_SESSION['reduce_reic'].$_SESSION['reduce_sequ'].$_SESSION['reduce_succ'].$_SESSION['reduce_quit'];

  $fakeresult .= $process[$_SESSION["wendy"]]["filename"];
  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " --".$_SESSION['mode'].'='.$fakeresult.".".$_SESSION['mode'];
  $realcall .= " --".$_SESSION['mode'].'='.$realresult.".".$_SESSION['mode'];

  $realcall .= " -v 2>&1";
  // end of building call strings


  // set variables for the system call
//  $command_mode = '--'.((!strcmp($_REQUEST['mode'], 'ig'))?'sa':'og');
//  $command_reduction = (isset($_REQUEST['reduce_wait'])?' --waitstatesOnly':'').(isset($_REQUEST['reduce_reic'])?' --receivingBeforeSending':'').(isset($_REQUEST['reduce_sequ'])?' --seqReceivingEvents':'').(isset($_REQUEST['reduce_succ'])?' --succeedingSendingEvent':'').(isset($_REQUEST['reduce_quit'])?' --quitAsSoonAsPossible':'');
//  $command_service = $inputfilename.'.owfn';
//  $output_file = $inputfilename.'.'.((!strcmp($_REQUEST['mode'], 'ig'))?'sa.':'').'og';
//  $output_graphics = $inputfilename.'.dot.png';
//  $output_bpmn = $inputfilename.'.owfn.'.$_REQUEST['mode'].'.bpmn.png';
//  $output_thumb = $inputfilename.'.owfn.'.$_REQUEST['mode'].'.thumb.png';
//  $output_bpmnthumb = $inputfilename.'.owfn.'.$_REQUEST['mode'].'.bpmn.thumb.png';
  
  // either only return the result or make a verbose call
//  if (!strcmp($_REQUEST['output'], 'result')) {
//    $call = 'wendy wendy/'.$command_service.' '.$command_mode.$command_reduction.' &> /dev/null';
//    
//    header("Content-type: text/plain");
//    system($call);
//    readfile('wendy/'.$output_file);
//    flush();
//    return;
//  } else {
//    header("Content-Type: text/html");
/*    echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';
//    
//    $call = 'wendy '.$command_service.' '.$command_mode.$command_reduction;
//  }*/
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>WAAS: Wendy as a Service</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Wendy</h1>

      <h2>Parameters</h2>
      <ul>
        <li><strong>input file:</strong> <?=$process[$_SESSION["wendy"]]["basename"]?>.owfn
          <?php
            switch($_SESSION['input_type']) {
              case "example":   echo "example file"; break;
              case "uploaded":  echo "uploaded file"; break;
              case "url":       echo "downloaded file"; break;
              case "given":     echo "given file"; break;
            }
//          if (!strcmp($_REQUEST['input'], 'url'))
//            echo ' (downloaded from <a href="'.$_REQUEST['input_url'].'">'.$_REQUEST['input_url'].'</a>)';
//          if (!strcmp($_REQUEST['input'], 'file'))
//            echo ' (uploaded from local file '.$_FILES['input_file']['name'].')';
          ?>
        </li>
        <li><strong>verification goal:</strong> 
          <?php echo ((!strcmp($_SESSION['mode'], 'ig'))?'interaction graph':'operating guidelines'); ?>
        </li>
<!--        <li><strong>apply reduction techniques:</strong> 
          <?php echo ((isset($_SESSION['reduction']))?'yes':'no'); ?>
        </li>-->
      </ul>

      <h2>Result</h2>

      <?php console($fakecall, $realcall)?>


    <h2>Output</h2>
    
    <?php
      if (!strcmp($_SESSION['mode'], 'ig')) {
        drawImage($fakeresult.'.sa');
//        $thumbnailcall = 'convert -resize 200x200 wendy/'.$output_graphics.' wendy/'.$output_thumb;
//        system($thumbnailcall);
//        echo '<p><a href="wendy/'.$output_graphics.'"><img src="wendy/'.$output_thumb.'" /></p>';
      } else {
        drawImage($fakeresult.'og');
        //dotimg('in=wendy/'.$inputfilename.'.dot&label='.urlencode("operating guideline"));
      }
    ?>
    
    <p><a href="<?=getLink($realresult.".".$_SESSION['mode'])?>">result</a></p>

    </div>
  </div>
</body>
</html>
