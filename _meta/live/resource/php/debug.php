<?php 

function debug($id = "") {
  echo "<h2>$id</h2>";
  echo isset($_REQUEST) ? "_REQUEST-" : "";  
  print_r(isset($_REQUEST) ? $_REQUEST : "");
  echo isset($_SESSION) ? "<br>_SESSION-" : "";
  print_r(isset($_SESSION) ? $_SESSION : "");
}

?>