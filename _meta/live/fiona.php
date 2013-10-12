<?php
  if (empty($_REQUEST))
    header('Location:index.html#fiona');

  include 'resource/php/console.php';
  include 'resource/php/dotimg.php';

  // case: service was chosen from drop down menu
  if (!strcmp($_REQUEST['input'], 'given')) {
    $inputfilename = $_REQUEST['input_service'];
  }

  // case: service is given as URL -- download it
  if (!strcmp($_REQUEST['input'], 'url')) {
    $inputfilename = 'downloadedfile_'.rand().rand();
    $download_command = 'wget \''.$_REQUEST['input_url'].'\' -O fiona/'.$inputfilename.'.owfn';
    system($download_command);
  }
    
  // case: service was uploaded by user
  if (!strcmp($_REQUEST['input'], 'file')) {
    $inputfilename = 'uploadedfile_'.rand().rand();
    move_uploaded_file($_FILES['input_file']['tmp_name'], 'fiona/'.$inputfilename.'.owfn');
  }


  // set variables for the system call
  $command_mode = '-t '.((!strcmp($_REQUEST['mode'], 'ig'))?'ig':'og');
  $command_reduction = isset($_REQUEST['reduce'])?' -r':'';
  $command_responsive = isset($_REQUEST['responsive'])?' -p responsive':'';
  $command_service = $inputfilename.'.owfn';
  $output_file = $inputfilename.'.owfn.'.((!strcmp($_REQUEST['mode'], 'ig'))?'ig.':'').'og';
  $output_graphics = $inputfilename.'.owfn.'.(isset($_REQUEST['responsive'])?'responsive.':'').$_REQUEST['mode'].'.png';
  $output_bpmn = $inputfilename.'.owfn.'.(isset($_REQUEST['responsive'])?'responsive.':'').$_REQUEST['mode'].'.bpmn.png';
  $output_thumb = $inputfilename.'.owfn.'.(isset($_REQUEST['responsive'])?'responsive.':'').$_REQUEST['mode'].'.thumb.png';
  $output_bpmnthumb = $inputfilename.'.owfn.'.(isset($_REQUEST['responsive'])?'responsive.':'').$_REQUEST['mode'].'.bpmn.thumb.png';
  
  // either only return the result or make a verbose call
  if (!strcmp($_REQUEST['output'], 'result')) {
    $call = 'fiona fiona/'.$command_service.' '.$command_mode.$command_reduction.$command_responsive.' -p no-png &> /dev/null';
    
    header("Content-type: text/plain");
    system($call);
    readfile('fiona/'.$output_file);
    flush();
    return;
  } else {
    header("Content-Type: text/html");
    echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';
    
    $call = 'fiona '.$command_service.' '.$command_mode.$command_reduction.$command_responsive;

    $call_2 = 'rachel -d ';
    if (!strcmp($_REQUEST['mode'], 'ig'))
      $call_2 = $call_2.'-m sa -a '.$output_file;
    else
      $call_2 = $call_2.'-m og -o '.$output_file;
  }
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>FAAS: Fiona as a Service</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <h1>Fiona</h1>

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
        <li><strong>apply reduction techniques:</strong> 
          <?php echo ((isset($_REQUEST['reduction']))?'yes':'no'); ?>
        </li>
        <li><strong>create responsive partner:</strong> 
          <?php echo ((isset($_REQUEST['responsive']))?'yes':'no'); ?>
        </li>
      </ul>

      <h2>Result</h2>

      <?php console($call, 'cd fiona; '.$call)?>


    <h2>Output</h2>
    
    <?php
      if (!strcmp($_REQUEST['mode'], 'ig')) {
        $thumbnailcall = 'convert -resize 200x200 fiona/'.$output_graphics.' fiona/'.$output_thumb;
        system($thumbnailcall);
        echo '<p><a href="fiona/'.$output_graphics.'"><img src="fiona/'.$output_thumb.'" /></p>';
      } else {
        system('cd fiona; '.$call_2.' 2> /dev/null');
        dotimg('in=fiona/'.$output_file.'.dot&label='.urlencode("operating guideline"));
      }
    ?>
    
    <p><a href="fiona/<?=$output_file?>">result</a></p>

    </div>
  </div>
</body>
</html>