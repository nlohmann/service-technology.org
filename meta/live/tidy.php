<?php
  // stop all running services
  system('killall dot');
  system('killall fiona');
  system('killall mia');
  system('killall wendy');
  system('killall bpel2owfn');
  system('killall rachel');
  system('killall wendy');
  system('killall lola-statespace');


  // delete generated files
  system('rm -f bpel2owfn/uploadedfile_*.bpel');
  system('rm -f bpel2owfn/downloadedfile_*.bpel');
  system('rm -f bpel2owfn/*.png bpel2owfn/*.owfn bpel2owfn/*.lola bpel2owfn/*.dot bpel2owfn/*.task');

  system('rm -f fiona/uploadedfile_*.owfn');
  system('rm -f fiona/downloadedfile_*.owfn');  
  system('rm -f fiona/*.png fiona/*.out fiona/*.og');

  system('rm -f wendy/*.png wendy/*.og wendy/*.dot wendy/*.sa');

  system('rm -f pnog/*.png pnog/*.og pnog/*-partner.owfn pnog/*.pn pnog/*.stg pnog/*.out pnog/*.dot');

  system('rm -f migration/*.png migration/*.dot migration/*.jt');

  system('rm -f rachel/*.png rachel/*.dot');  
?>

<html>
  <head>
    <meta http-equiv="REFRESH" content="0;url=.">
  </head>
</html>
