<?php
  if (empty($_REQUEST))
    header('Location:index.html#bpel2owfn');

  include 'resource/php/console.php';
  
  // required because
  if (!strcmp($_REQUEST["input_type"], 'example'))
  {
    $_REQUEST["input_example"] = 'bpel2owfn/'.$_REQUEST["input_example"].'.bpel';
  }
  require 'resource/php/files.php';
  
  // DEBUG:
  echo '$_REQUEST:<br>';
  print_r($_REQUEST);
  echo '<br>';
  echo '$_SESSION:<br>';
  print_r($_SESSION);
  
  foreach ($fileinfo as $file)
  {

    system('cd '.$file["dirname"]);
    
    if (!strcmp($_REQUEST['output'], 'result')) {
      $call = 'bpel2owfn -i '.$file["basename"].' -m petrinet -p '.$_REQUEST['patterns'].' -f '.$_REQUEST['format'].' -r '.$_REQUEST['reduce'];
  //    $call = $call.((isset($_REQUEST['reduce']))?' -p reduce':'');
  
      header("Content-type: text/plain");
      system($call);
      return;
    } else {
      header("Content-Type: text/html");
      echo '<?xml version="1.0" encoding="ISO-8859-1" ?>';
      
      $call = 'bpel2owfn -i '.$file["basename"].' -m petrinet -p '.$_REQUEST['patterns'].' -f '.$_REQUEST['format'].' -r '.$_REQUEST['reduce'].' -f dot -o '.$file["filename"].".dot";
  //    $call = $call.((isset($_REQUEST['reduce']))?' -p reduce':'');
    }
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>BAAS: BPEL2oWFN as a Service</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
  
  <h1>BPEL2oWFN</h1>

  <h2>Parameters</h2>

  <ul>
    <li><strong>input file:</strong> <?=$file["basename"]?>
      <?php
        if (!strcmp($_REQUEST['input_type'], 'url'))
          echo ' (downloaded from <a href="'.$file["link"].'">'.$file["link"].'</a>)';
        if (!strcmp($_REQUEST['input_type'], 'file'))
          echo ' (uploaded from local file '.$file["basename"].')';
      ?>
    </li>
    <li><strong>patterns:</strong> <?=$_REQUEST['patterns']?></li>
    <li><strong>file format:</strong> 
      <?php echo ((!strcmp($_REQUEST['format'], "owfn"))?'Fiona open net':'LoLA Petri net'); ?>
    </li>
    <li><strong>structural reduction level:</strong> <?=$_REQUEST['reduce']?></li>
  </ul>
  
  <h2>Result</h2>
  
  <?php console($call, 'cd bpel2owfn; '.$call); ?>
  
<?php
  $thumbnailcall = 'convert -transparent "rgb(255,255,255)" -resize 200x200 '.$file["short"].'.png '.$file["short"].'.thumb.png';
  system($thumbnailcall);
?>

  <h2>Schema</h2>
  <table> 
  <?php
    echo '<tr>';
    echo '<td><img src="'.$file["short"].'.png" height="100" width="100"></td>';
    echo '<td><img src="resource/images/arrow.png" height="96" width="96"></td>';
    echo '<td><img src="resource/images/tool.png" height="100" width="100"></td>';
    echo '<td><img src="resource/images/arrow.png" height="96residence" width="96"></td>';
    echo '<td><img src="resource/images/doc.png" height="100" width="100" style="margin-left:-30px;"><img src="'.$file["short"].'.thumb.png" height="80" width="50" style="margin-bottom: 10px; margin-left: -80px;"></td>';
    echo '</tr>';

    echo '<tr>';
    echo '<td style="width: 150px;">';
    if (!strcmp($_REQUEST['input_type'], 'url'))
      echo '<a href="'.$file["link"].'">downloaded file</a>';
    if (!strcmp($_REQUEST['input_type'], 'given'))
      echo '<a href="'.$file["residence"].'">BPEL process</a>';
    if (!strcmp($_REQUEST['input_type'], 'uploaded'))
      echo '<a href="'.$file["residence"].'"/>'.$_FILES['input_file']['name'].'</a>';
    echo '</td>';
    echo '<td></td>';
    echo '<td style="width: 150px;">BPEL2oWFN</td>';
    echo '<td></td>';
    echo '<td style="width: 150px;"><a href="'.$file["residence"].'.'.$_REQUEST['format'].'"/>'.((!strcmp($_REQUEST['format'], "owfn"))?'Fiona open net':'LoLA Petri net').'</a></td>';
    echo '</tr>';
  ?>
  </table>
  
  
  <h2>Output</h2>
  <p>
    <a href="<?=$file["short"]?>.png" target="_blank"><img src="<?=$file["short"]?>.thumb.png" />
  </p>
  <p>
    <a href="<?=$file["short"]?>.<?=$_REQUEST['format']?>">result</a>
  </p>
  
  </div>
  </div>
</body>
</html>
<?php 

  }

?>
