# How to include your tool <tool> in service-technology.org

## Preparation

* Checkout your tool into the directory *<tool>*

* Create *<tool>/<tool>.json* with at least the following objects:
  * *toolname* -- the actual name of the tool
  * *svnname* -- the name of the tool in the SVN; required iff it differs from *toolname*
  * *authors* -- a list of authors, optionally with their specific accomplishments
  * *maintainer* -- a single person's real name who maintains *tools*
  * *tagline* -- a short mission statement of *tool*
  * *purpose* -- a description of the tool's purpose (markdown allowed)
  * *license* -- the license under which the tool is published

* Checkout *svn.gna.org/service-tech/trunk/meta/geninfo*
* Create *<tool>/generic.json* by calling *geninfo/update_json.sh*

* Optional: Create a picture *overview.png* visualizing the tool's purpose

* Checkout *svn.gna.org/service-tech/website*
* Checkout *svn.gna.org/service-tech/trunk/meta/genwebsite*


## Create tool pages

* Aggregate *<tool>/<tool>.json* and *<tool>/generic.json* by invoking *cat <tool>/<tool.json> <tool>/generic.json <tool>/complete.json*
* Invoke *genwebsite/createToolPages.sh <tool>/complete.json genwebsite/template/tool website/<tool>* 

## Change static content

* In *website/tools/index.html* add your tool in an appropriate subsection together with its tagline
* Optional: Copy *overview.png* to *website/<tool>/g/*

## Final steps

* Add the following files and folders to version control: 
  * *website/<tool>* and its contents
  * *<tool>/<tool>.json*
  * *<tool>/<generic>.json* 


