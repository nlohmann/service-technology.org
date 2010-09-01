<?php
  if (empty($_REQUEST))
    header('Location:index.html#wendy');

  require 'resource/php/session.php';
  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';

  // putenv("PATH=/var/www/service-tech/live/local/bin:$PATH");

  // case: service was chosen from drop down menu
  if (!strcmp($_REQUEST['input'], 'given')) {
    $inputfilename = $_REQUEST['input_service'];
  }

  // case: service is given as URL -- download it
  if (!strcmp($_REQUEST['input'], 'url')) {
    $inputfilename = 'downloadedfile_'.rand().rand();
    $download_command = 'wget \''.$_REQUEST['input_url'].'\' -O wendy/'.$inputfilename.'.owfn';
    system($download_command);
  }
    
  // case: service was uploaded by user
  if (!strcmp($_REQUEST['input'], 'file')) {
    $inputfilename = 'uploadedfile_'.rand().rand();
    move_uploaded_file($_FILES['input_file']['tmp_name'], 'wendy/'.$inputfilename.'.owfn');
  }


  // set variables for the system call
  $command_mode = '--'.((!strcmp($_REQUEST['mode'], 'ig'))?'sa':'og');
  $command_reduction = (isset($_REQUEST['reduce_wait'])?' --waitstatesOnly':'').(isset($_REQUEST['reduce_reic'])?' --receivingBeforeSending':'').(isset($_REQUEST['reduce_sequ'])?' --seqReceivingEvents':'').(isset($_REQUEST['reduce_succ'])?' --succeedingSendingEvent':'').(isset($_REQUEST['reduce_quit'])?' --quitAsSoonAsPossible':'');
  $command_service = $inputfilename.'.owfn';
  $output_file = $inputfilename.'.'.((!strcmp($_REQUEST['mode'], 'ig'))?'sa.':'').'og';
  $output_graphics = $inputfilename.'.dot.png';
  $output_bpmn = $inputfilename.'.owfn.'.$_REQUEST['mode'].'.bpmn.png';
  $output_thumb = $inputfilename.'.owfn.'.$_REQUEST['mode'].'.thumb.png';
  $output_bpmnthumb = $inputfilename.'.owfn.'.$_REQUEST['mode'].'.bpmn.thumb.png';
  
  // either only return the result or make a verbose call
  if (!strcmp($_REQUEST['output'], 'result')) {
    $call = 'wendy wendy/'.$command_service.' '.$command_mode.$command_reduction.' &> /dev/null';
    
    header("Content-type: text/plain");
    system($call);
    readfile('wendy/'.$output_file);
    flush();
    return;
  } else {
    header("Content-Type: text/html");
    echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';
    
    $call = 'wendy '.$command_service.' '.$command_mode.$command_reduction;
  }
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
        <li><strong>input file:</strong> <?=$inputfilename?>.owfn
          <?php
          if (!strcmp($_REQUEST['input'], 'url'))
            echo ' (downloaded from <a href="'.$_REQUEST['input_url'].'">'.$_REQUEST['input_url'].'</a>)';
          if (!strcmp($_REQUEST['input'], 'file'))
            echo ' (uploaded from local file '.$_FILES['input_file']['name'].')';
          ?>
        </li>
        <li><strong>verification goal:</strong> 
          <?php echo ((!strcmp($_REQUEST['mode'], 'ig'))?'interaction graph':'operating guidelines'); ?>
        </li>
<!--        <li><strong>apply reduction techniques:</strong> 
          <?php echo ((isset($_REQUEST['reduction']))?'yes':'no'); ?>
        </li>-->
      </ul>

      <h2>Result</h2>

      <?php console($call, 'cd wendy; '.$call.' --verbose --dot')?>


    <h2>Output</h2>
    
    <?php
      if (!strcmp($_REQUEST['mode'], 'ig')) {
        $thumbnailcall = 'convert -resize 200x200 wendy/'.$output_graphics.' wendy/'.$output_thumb;
        system($thumbnailcall);
        echo '<p><a href="wendy/'.$output_graphics.'"><img src="wendy/'.$output_thumb.'" /></p>';
      } else {
        dotimg('in=wendy/'.$inputfilename.'.dot&label='.urlencode("operating guideline"));
      }
    ?>
    
    <?php
    drawImage($inputfilename.'.dot');
    ?>
    
    <p><a href="wendy/<?=$output_file?>">result</a></p>

    </div>
  </div>
</body>
</html>
