# How to include your tool <tool> in service-technology.org

## Prerequisites

* *Python 3*
* The *misaka* module (http://misaka.61924.nl/)
* *SVN*
* *GNA service-tech* repository *svn+ssh* access
* A *JavaScript* interpreter (named js) compatible with *jsawk*, e.g. *Mozilla Spidermonkey*

## Preparation

* Checkout your tool *<tool>* from GNA

* In the tool's *doc*-directory, create *<tool>.json* with at least the following objects (take *example.json* as template):
  * *toolname* -- the actual name of the tool
  * *svnname* -- the name of the tool in the SVN; required iff it differs from *toolname*
  * *authors* -- a list of authors, optionally with their specific accomplishments
  * *maintainer* -- a single person's real name who maintains *tools*
  * *tagline* -- a short mission statement of *tool*
  * *purpose* -- a description of the tool's purpose (markdown allowed)
  * *license* -- the license under which the tool is published

* Add *doc/<tool>.json* to version control and commit. 

* Optional: Create a picture *overview.png* visualizing the tool's purpose

## Create tool pages

You may choose to either use the automatic script or do the steps manually. Anyway, begin with creating an empty directory *<workspace>* 

### Automatic script

* Change to a *genwebsite* directory
* Call *./autoGenToolPage.sh <tool> <svnname> <username> <workspace>* where 
  * *<svnname>* is the name of the svn folder containing *<tool>* on GNA
  * *<username>* is your GNA username

### Manual steps

* Checkout your tool into *<workspace>/<tool>*
* Checkout *svn.gna.org/service-tech/trunk/meta/geninfo* into *<workspace>*
* Add *<workspace>/geninfo* to the PATH
* Create *<workspace>/<tool>/doc/generic.json* 
  * *cd <workspace>/<tool> && autoreconf -i && ./configure && cd doc*
  * *update_json.sh <tool>.json*

* Checkout *svn.gna.org/service-tech/website* into *<workspace>*
* Checkout *svn.gna.org/service-tech/trunk/meta/genwebsite* into *<workspace>*

* *cd <workspace>/genwebsite*
* Invoke *./createToolPages.sh <tool> <workspace>/<tool>/doc/ template/tool/ <workspace>/website/<tool>/ ../utils/ ../data/* 


## Change static content

* In *website/tools/index.html* add your tool in an appropriate subsection together with its tagline
* Optional: Copy *overview.png* to *website/<tool>/g/*

## Final steps

* Add the following files and folders to version control: 
  * *website/<tool>* and its contents
  * *<tool>/<tool>.json*
  * *<tool>/<generic>.json* 


