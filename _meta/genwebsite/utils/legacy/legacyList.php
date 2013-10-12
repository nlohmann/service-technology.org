
<?php

$ll = file_get_contents('legacyList.txt');

$ll = explode("\n", $ll);

$base = 'http://www.service-technology.org/publications/';

foreach($ll as $i) {
    if(strlen($i) < 2) continue;
    shell_exec('mkdir publications/'.$i);
    shell_exec('php legacy.php '.$base.$i.' > publications/'.$i.'/index.html');
}



?>
