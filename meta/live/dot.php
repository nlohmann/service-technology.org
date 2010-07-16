<?php
  require 'resource/php/session.php';

  // putenv("PATH=/usr/local/bin:/opt/local/bin:/opt/local/sbin:/usr/bin");


  // set output filename
  $outfile = $_REQUEST['in'].'.png';


  // set size of thumbnails
  if (empty($_REQUEST['thumbnail_size']))
    $thumbnail_size = 300;
  else
    $thumbnail_size = $_REQUEST['thumbnail_size'];


  // create image using dot
  if (!file_exists($outfile)) {    
    // if graph has more than 300 edges, do not layout 
    // see http://ingeneur.wordpress.com/2008/04/29/count-occurrences-of-a-word-in-a-file/
    $call = 'tr [:blank:] [\'\n\'] < '.$_REQUEST['in'].' | grep -c "\->"';
    $edges_count = (int)`$call`;

    if ($edges_count > 300)
      exit('<div class="doterror"><img src="resource/images/error.png" align="left" /> Graph for '.$_REQUEST['label'].' is too big to render.</div>');
    
    system('dot -Glabel="" -Tpng '.$_REQUEST['in'].' -o '.$outfile.' &> /dev/null');
    system('convert -trim '.$outfile.' '.$outfile.' &> /dev/null');
  }

  
  // create thumbnail
  $thumbnail = dirname($outfile).'/thumb_'.basename($outfile);
  if (!file_exists($thumbnail)) {
    
    // check the size of the image: if 66% does not exceed the given
    // thumbnail size, resize it to 66%, otherwise to the thumbnail size
    list($width, $height) = getimagesize($outfile);
    if (($width * .66 <= 700) && ($height * .66 <= $thumbnail_size)) {
      system('convert -thumbnail "66%" '.$outfile.' '.$thumbnail);          
    } else {
      system('convert -thumbnail "700x'.$thumbnail_size.'>" '.$outfile.' '.$thumbnail);
    }    
  }
    
  $result = '<a href="'.$outfile.'" target="_blank"><img id="thumbnail" src="'.$thumbnail.'" alt="'.$_REQUEST['label'].'" /></a>';
  
  echo $result;
  echo '<div class="label">'.$_REQUEST['label'].'</div>';
?>
