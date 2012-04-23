# How to include my tool *tool* in service-technology.org

## Preparation

* Create *tool*.json with at least the following objects:
  * *toolname* -- the actual name of the tool
  * *svnname* -- the name of the tool in the SVN; required iff it differs from *toolname*
  * *authors* -- a list of authors, optionally with their specific accomplishments
  * *maintainer* -- a single person's real name who maintains *tools*
  * *tagline* -- a short mission statement of *tool*
  * *purpose* -- a description of *tool*'s purpose (markdown allowed)
  * *license* -- the license under which *tool* is published

* Create *generic.json* by calling Niels' script (TODO)

* Optional: Create a picture *overview.png* visualizing *tool*'s purpose


## Create tool pages

* Aggregate *<tool>.json* and *generic.json* by invoking *cat <tool.json> generic.json complete.json*
* Go to *<script>* directory
* Invoke *createToolPages.sh complete.json <path/to/template/folder> <path/to/tool/folder>* where
  * *<path/to/template/folder>* is the directory holding the template html and style files
  * *<path/to/tool/folder>* is the directory where the tool website is supposed to be stored. If it does not exist yet, it will be created

## Change static content

* In *tools/index.html* add your tool in an appropriate subsection together with its tagline
* Optional: Copy *overview.png* to *<tool>/g/*


