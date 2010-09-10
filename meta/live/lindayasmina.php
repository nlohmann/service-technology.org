<?php
  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';

  $tool = "lindayasmina";
  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';


  // Grundgeruest:
	// - call linda for each selected owfn
	// - call yasmina with all fingerprints together	
  
  $paramsL = "";
  if (isset( $_SESSION[$tool]["ly_singleEvents"])) { $paramsL .= " -0"; }
  if (isset( $_SESSION[$tool]["ly_mutualExclusion"])) { $paramsL .= " -1"; }
  if (isset( $_SESSION[$tool]["ly_octagon"])) { $paramsL .= " --oct"; }
  if (isset( $_SESSION[$tool]["ly_mdb"])) { $paramsL .= " --mdb"; }

  if (isset( $_SESSION[$tool]["ly_output_linda_output"]) && $_SESSION[$tool]["ly_output_linda_output_option"] == "verbose") { $paramsL .= " -v"; }
  if (isset( $_SESSION[$tool]["ly_output_fingerprints"]) && $_SESSION[$tool]["ly_output_fingerprints_option"] == "formula") { $paramsL .= " --pretty"; }

  $lindaFakeCalls = array();
  $lindaCalls = array();
  $fingerprintFiles = array();
  $opennetFiles = array();
  $yasminaCall = "yasmina ";
  $yasminaFakeCall = "yasmina ";
  
 foreach( $_SESSION[$tool]["input_example"] as $key => $value){

   $opennetFiles[$key] = current(prepareFile("lindayasmina/${value}.owfn"));
  	$fingerprintFiles[$key] = current(createFile("${value}.owfn.fp"));
  
   $lindaCalls[$key] = "linda $paramsL " . $opennetFiles[$key]["residence"] . " -o "  . $fingerprintFiles[$key]["residence"] . " 2>&1";
  $lindaFakeCalls[$key] = "linda $paramsL $value.owfn -o $value.owfn.fp";
   $yasminaCall .= " -m " . $fingerprintFiles[$key]["residence"];
   $yasminaFakeCall .= " -m $value.owfn.fp";   
  }
  if (isset( $_SESSION[$tool]["ly_output_yasmina_output"]) && $_SESSION[$tool]["ly_output_yasmina_output_option"] == "verbose") { $yasminaCall .= " -v"; $yasminaFakeCall .= " -v";}
  $yasminaCall .=  " 2>&1";

  ?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live - Service Formalization</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><img src="resource/images/live.png" alt="service-technology.org/live" /></div>
      <h1>Communication Fingerprints</h1>
      
      <h2>Selected files</h2>
    <?php  
      foreach( $opennetFiles as $value){
      echo '<a href="'.$value["link"].'" target="_blank">'.$value["basename"].'</a><br />';
      if (isset( $_SESSION[$tool]["ly_output_nets"])) { 
        //drawImage($opennetFiles[$key]["lindayasmina/$value.owfn"]);
        drawImage($value["basename"]);
      }
    }
    
      ?>

      <h2>Computing fingerprints with Linda</h2>
     <i>Selected term sets:</i> 
<?php
  if (isset( $_SESSION[$tool]["ly_singleEvents"])) { echo "single events; "; }
  if (isset( $_SESSION[$tool]["ly_mutualExclusion"])) { echo "mutual exclusion; "; }
  if (isset( $_SESSION[$tool]["ly_octagon"])) { echo "octagon terms; "; }
  if (isset( $_SESSION[$tool]["ly_mdb"])) { echo "mdb terms; "; }
?>
<br><br>
 <?php  

      foreach( $_SESSION[$tool]["input_example"] as $key => $value){
        echo "<br />$value.owfn.fp";
         if (isset( $_SESSION[$tool]["ly_output_linda_output"])) { 
          console($lindaFakeCalls[$key], $lindaCalls[$key]); 
       } else {
       
           exec($lindaCalls[$key]);
       
       }
       if (isset( $_SESSION[$tool]["ly_output_fingerprints"]) && $_SESSION[$tool]["ly_output_fingerprints_option"] == "formula") { 
?><div align="center">
        <textarea cols="70" rows="2"><?php 

       echo exec($lindaCalls[$key] . " | grep Fingerprint 2>&1 | sed 's/linda: Fingerprint: //g'");
       ?>
               </textarea></div>
       <?php
 }
       if (isset( $_SESSION[$tool]["ly_output_fingerprints"]) && $_SESSION[$tool]["ly_output_fingerprints_option"] == "file") { 
        ?>
        <div align="center">
        <textarea cols="70" rows="10"><?php 
      $lines = file($fingerprintFiles[$key]["$value.owfn.fp"]["residence"]);
      foreach ($lines as $line_num => $line) {
        echo $line;
      }
        ?>
        </textarea></div>
        <?php
      }
        }
        ?>

      <h2>Matching fingerprints with Yasmina</h2>
 <?php  
 if (isset( $_SESSION[$tool]["ly_output_yasmina_output"])) { 
          console($yasminaFakeCall,$yasminaCall); 
       } else {
       
           exec($yasminaCall);
           echo "... done.";
       
       }    ?>

      <h2>Result</h2>
      <?php 
      if (trim(exec("$yasminaCall | grep \"Not compatible\"")) != "") {
        echo "Not compatible!";
      } else {
        echo "Inconclusive!";
      }
      
      ?>
      
    </div>
  </div>
  <div id="footer">
    <p><a href="http://service-technology.org">service-technology.org</a> is a cooperation between the <a  href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/">University of Rostock</a> and the <a  href="http://www2.informatik.hu-berlin.de/top/index.php">Humboldt-Universit&auml;t zu Berlin</a>.</p>
  </div>
</body>
</html>
