<?php
  putenv("PATH=/usr/local/bin:/opt/local/bin:/opt/local/sbin:/usr/bin");

  function getNumber ($file, $string, $position = 1) {
    
    // get the line
    $searchstring = shell_exec('grep "'.$string.'" '.$file);
    
    // strip all numbers
    $numberstring = preg_replace("/[^0-9]/", ' ', $searchstring);
    
    // translate into array
    $numbers = explode(" ", $numberstring." ");
    
    // find the number at $position
    $numbercounter = 0;    
    foreach ($numbers as $number) {
      if (is_numeric($number)) {
        $numbercounter++;
        if ($numbercounter == $position) {
          return (int) $number;
        }
      }
    }

    return NULL;
  }
?>
