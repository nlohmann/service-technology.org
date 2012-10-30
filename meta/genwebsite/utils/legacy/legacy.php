<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
  <head>
    <meta name="generator" content="HTML Tidy for Windows (vers 14 February 2006), see www.w3.org">
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
   
    <link href="../../main.css" rel="stylesheet" type="text/css">
    <link href="../legacy.css" rel="stylesheet" type="text/css">
    <title>
      service-technology.org/publications
    </title>
  </head>
  <body>
    <div id="mainframe">
      <div id="topspacer"></div>
      <div id="shadowbox">
        <div id="navigation">
          <div id="titlenav">
            <div id="pagetitle">
              <h1>
                <a href="../../index.html">service-technology.org</a>/<a href="../../pub/index.html">publications</a>
              </h1>
            </div>
            <div id="nav">
              <ul>
                <li>
                  <a href="../../people/index.html" id="people" class="ncurrent" name="people">People</a>
                </li>
                <li style="list-style: none">
                  <div class="seperator"></div>
                </li>
                <li>
                  <a href="../../pub/index.html" id="pub" class="current" name="pub">Publications</a>
                </li>
                <li style="list-style: none">
                  <div class="seperator"></div>
                </li>
                <li>
                  <a href="../../tools/index.html" id="tools" class="ncurrent" name="tools">Tools</a>
                </li>
              </ul>
            </div>
          </div>
        </div>

        <div id="content">

<?php

$out = file_get_contents($argv[1]);

$begin_hash = "<div class=\"dokuwiki\">";
$end_hash = "<div id=\"body_bottom\">";

$arr = explode($begin_hash, $out);
$out = $arr[1];
$arr = explode($end_hash, $out);
$out = $arr[0];
$out = $begin_hash . $out;

// remove detail links
$out = preg_replace( "/href=\\\"\/_detail[^\\\"]*\\\"/", "", $out);
$out = preg_replace( "/src=\\\"\/_media\/publications([^\\\"\\?]*)\\??[^\\\"]*\\\"/", "src=\"/publications/files$1\"", $out);
echo $out;

?>

<!-- END OLD CONTENT -->

        </div>
      </div>
    </div>
  </body>
</html>
