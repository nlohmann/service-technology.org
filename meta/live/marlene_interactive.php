<?php

  // most important script! sets session information and PATH!
  require_once 'resource/php/session.php';
  $tool = "marlene_ia";
  
  if ( isset($_REQUEST["rules"] ))
  {
    $_SESSION[$tool]["rules"] = $_REQUEST["rules"];
    header("Location: ".$_SERVER["PHP_SELF"]);
    exit;
  }

  // some functions for copying/creating files to/in temporary directory
  // see files.php for further information
  require_once 'resource/php/files.php';

  if ( ! isset($_SESSION[$tool]["rules"]) )
  {
    $_SESSION[$tool]["rules"] = "";
  }

  if (!strcmp($_SESSION[$tool]["process"], "marlene_ia/iRules.owfn"))
  {
    $services = array("marlene/pro.owfn", "marlene/req.owfn");
    $rules = createFile("diag_iRules.ar");
  }
  else if (!strcmp($_SESSION[$tool]["process"], "marlene_ia/iCoffee.owfn"))
  {
    $services = array("marlene/myCoffee.owfn", "marlene/myCustomer.owfn");
    $rules = createFile("diag_iCoffee.ar");
  }


  
  $services = prepareFiles($services);
  // $rules = createFile("diag.ar");
  $diagfile = current(createFile("live.txt"));
  
  $rulefile = current($rules);

  $handle = fopen($rulefile["residence"], "w+");
  fwrite($handle, stripslashes($_SESSION[$tool]['rules']));
  fclose($handle);

  $fakecall = "marlene";
  $realcall = "marlene";
  $fakeresult = "";
  $first = true;

  foreach($services as $info)
  {
    $fakecall .= " ".$info["basename"];
    $realcall .= " ".$info["residence"];
    if ($first == true)
    {
      $first = false;
    }
    else
    {
      $fakeresult .= "_";
    }
    $fakeresult .= $info["filename"];
  }
  $fakeresult .= ".owfn"; 
  $realresult = $_SESSION["dir"]."/".$fakeresult;

  $fakecall .= " -o ".$fakeresult;
  $realcall .= " -o ".$realresult;

  $fakecall .= " -r ".$rulefile["basename"];
  $realcall .= " -r ".$rulefile["residence"];

  $realcall .= " --diagnosis --property=livelock --live=".$diagfile["residence"]; //." -v 2>&1";

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <link rel="stylesheet" type="text/css" href="resource/css/format.css" />
  <link rel="stylesheet" type="text/css" href="resource/css/console.css" />
  <link rel="shortcut icon" href="resource/favicon.ico" type="image/x-icon" />
  <link rel="icon" href="resource/favicon.ico" type="image/x-icon" />
  <title>service-technology.org/live &ndash; Interactive Adapter Rule Design</title>
  <script type="text/javascript" src="resource/js/jquery-1.2.6.pack.js"></script>
</head>

<body>
  <div id="container">
    <div id="content">
      <div style="float: right; top: -10px;"><a href="./#marlene_ia"><img src="resource/images/live.png" alt="service-technology.org/live" /></a></div>
      <h1>Interactive Adapter Rule Design</h1>

      <h2>Diagnosis</h2>

      <?php
        // $call_result = console($fakecall, 'cd marlene; echo $PATH; which marlene; echo $LD_LIBRARY_PATH; '.$realcall.' ');
        $call_result = console($fakecall, $realcall);
      ?>
      
    <a name="rules"></a>
    <h3>Rules</h3>
    <div style="text-align:center;">
    <form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST">
      
      <?php
        echo "<textarea style=\"text-align: center;\" name=\"rules\" cols=\"50\" rows=\"10\">".$_SESSION[$tool]["rules"]."</textarea>";
      ?>
        <p style="text-align: center"><b>Hint:</b> Rules must have the form:<br/> 
          <code>message<sub>1</sub>, message<sub>2</sub>, ... -&gt; message<sub>3</sub>, message<sub>4</sub>, ... ;</code><br/>
          (One side of the arrow may even be empty.)</p>
        <p><input type="submit" value=" Save Rules "><input type="reset" value=" Undo Changes "></p>
     </form>
     </div>
<script>
function toggle(infoid)
{
if ($("#more"+infoid).is(":hidden")) {
$("#more"+infoid).slideDown("slow");
} else {
$("#more"+infoid).hide();
}

}

</script>

     <div style="margin-left:auto; margin-right:auto;">
<!--     
       <table style="margin-left:auto; margin-right:auto; border-width: 2px; border-style: solid; border-color: black;">
          <tr>
            <td>DL</td>
            <td>pending</td>
            <td>&rarr;</td>
            <td>required</td>
            <td onClick="toggle(bla)">more</td>
          </tr>
          <tr>
            <td colspan="5">
               <div id="morebla" style="display:none;">
                  bla
               </div>
            </td>
          </tr>
       </table>
-->       
       <table style="width: 80%; margin-left:auto; margin-right:auto; border-width: 2px; border-style: solid; border-color: black;">
<?PHP
    $hasAdapter = true;

    echo "<tr><th width=\"5%\">type</th>\n";
    echo "<th width=\"43%\">pending messages</th>\n";
    echo "<th width=\"4%\"></th>\n";
    echo "<th width=\"43%\">required messages</th>\n";
    echo "<th width=\"5%\"></th></tr>\n";
  $dll = file($diagfile["residence"]);
  foreach ($dll as $index => $line)
  {
    $hasAdapter = false;
    // echo $line;
    list($type, $finals, $pending, $required, $rules) = explode(";", $line);
print<<<END
   
          <tr>
END;
    if ( ! strcmp($type, "DL") )
    {
      $type = "deadlock";
    }
    else if ( ! strcmp($type, "LL") )
    {
      $type = "livelock";
    }
  
    $more = LIVEBASE."/resource/images/more.png";
    echo <<<END
    <td width="5%">$type</td>
    <td width="43%">$pending</td>
    <td width="4%"><!--&rarr;--></td>
    <td width="43%">$required</td>
    <td width="5%" onClick='toggle($index)'><img src="$more" alt="toggle" title="More information"/></td>
END;
//    echo "<td width=\"5%\">$type</td>\n";
//    echo "<td width=\"43%\">$pending</td>\n";
//    echo "<td width=\"4%\"><!--&rarr;--></td>\n";
//    echo "<td width=\"43%\">$required</td>\n";
//    echo "<td width=\"5%\" onClick='toggle($index)'><img src=\"".LIVEBASE."/resource/images/more.png\" alt=\"toggle\" title=\"More information\"/></td>\n";
print<<<END
          </tr>
          <tr>
            <td colspan="5">
END;
    echo "<div id=\"more$index\" style=\"display:none\">";
    echo "<h3>Additional information</h3>";
    if ( strcmp(rtrim($finals), "") )
    {
    $finals = explode(",", $finals);
    foreach ( $finals as $net )
    {
      // print_r($services);
      reset($services);
      if ( ! strcmp($net,"net1.") )
      {
        $serv = current($services);
        echo "<p>Net <i>".$serv["basename"]."</i> is already in a final state.</p>";
      }
      else
      {
        $serv = next($services);
        echo "<p>Net <i>".$serv["basename"]."</i> is already in a final state.</p>";
      }
    }
    }
    // $rules, $finals
    echo "<h4>Previously applied rules</h4>";
    if ( ! strcmp(rtrim($rules), "") )
    {
      echo "<p>none.</p>";
    }
    else
    {
      $ruleindex = explode(";", $_SESSION[$tool]["rules"]);
      // print_r($ruleindex);
      $rules = explode (",", $rules);
      foreach($rules as $rule)
      {
          echo "<p>".$ruleindex[$rule-1]."</p>";
      }
     
    }
    echo "<hr /></div>\n";
print<<<END
            </td>
          </tr>
END;
  }
?>
       </table>
     </div>

      <h2>Input Service Models</h2>

      <?php
      foreach($services as $file)
      {
	// system("petri --removePorts -o dot ".$file["residence"]);
        // dotimg("in=".urlencode($file["residence"]).".dot&amp;thumbnail_size=400&amp;label=".urlencode($file["basename"]));
        drawImage($file["basename"]);
      }
      ?>


      <ul>
<?php
  foreach($services as $info)
  {
    echo "<li><a href=".$info["link"].">".$info["basename"]."</a></li>";
  }
?>
      </ul>
     
<?php
  if ( $hasAdapter )
  {
    echo "<h2>Synthesized Adapter</h2>\n";
    drawImage($fakeresult);
    echo "<ul>";
    echo "<li><a href=".getLink($fakeresult).">".basename($fakeresult)."</a></li>";
    echo "</ul>\n";
  }
?>

    </div>
  </div>
  <div id="footer">
    <p><a href="http://service-technology.org">service-technology.org</a> is a cooperation between the <a  href="http://wwwteo.informatik.uni-rostock.de/ls_tpp/">University of Rostock</a> and the <a  href="http://www2.informatik.hu-berlin.de/top/index.php">Humboldt-Universit&auml;t zu Berlin</a>.</p>
  </div>
</body>

</html>
