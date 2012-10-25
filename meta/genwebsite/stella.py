#!/usr/bin/env python3

import os
import sys
import json
import shutil

modes = {"prepare", "update", "check", "help"}

mode = sys.argv[1]
homeDir = os.path.dirname(__file__)
dataDir = homeDir + "/data/"
templateDir = homeDir + "/template/tool/"

import utils.update_json
import utils.update_website
import utils.stella_out

if mode not in modes:
  utils.stella_out.error("Unknown mode >" + mode +  "<. Invoke >stella.py help< for available modes.")
else:
  utils.stella_out.verbose("Entering mode >" + mode + "<.")

jsonfile = ""
shortname = ""
path = ""
targetDir = ""  

if mode == "check" or mode == "update":
  jsonfile = sys.argv[2]
  # set the path to the jsonfile
  path = os.path.dirname(jsonfile) + "/"

  # find the shortname from the json
  shortname = json.loads(open(jsonfile, "r").read())["shortname"]

  # generate the generic.json
  utils.update_json.generate(shortname, path)

  # mine everything
  utils.update_website.mineAll(jsonfile, path+"generic.json", dataDir + "/people.json", dataDir + "/requirements.json", dataDir + "/variables.json")
  
if mode == "update":
  targetDir = sys.argv[3] + "/"

  # write each single file
  for file in ["download", "getInvolved", "index",  "science", "support"]:
    utils.update_website.writeFile(templateDir  + file + ".html", targetDir + file +".html")

if mode == "prepare":
  toolname = sys.argv[2]
  targetDir = sys.argv[3]
  shutil.copytree(templateDir,targetDir + "/" + toolname, ignore=ignore_patterns('.svn'))
# copytree(source, destination, )
if mode == "help": 
  if len(sys.argv) == 2:
    print("Call syntax: \n\tstella.py <mode> <args> \n\twhere \n\t\t<mode> is in " + str(modes)+ "\n\t\t<args> is a sequence of arguments for the given mode.")
    print("For more information on a specific mode, invoke: \n\tstella.py help <mode>")
  else:
    hmode = sys.argv[2]
    if hmode not in modes:
      utils.stella_out.error("Unknown mode >" + hmode +  "<. Invoke >stella.py help< for available modes.")
    print("Help on mode >" + hmode + "<:")
    if hmode == "prepare":
      print("Creates a directory with all necessary interna")
      print("Call syntax: \n\tstella.py prepare <toolname> <dirname> \n\twhere \n\t\t<toolname> is the name of the created directory\n\t\t<dirname> is the parent directory of the new directory.")
    if hmode == "check":
      print("Checks a given JSON file for completeness and tries to retrieve all generic information.")
      print("Call syntax: \n\tstella.py check <jsonfile> \n\twhere \n\t\t<jsonfile> is the description of the tool in json format, presumably in the doc/ directory")      
    if hmode == "update":
      print("First invokes mode >check<, then creates all pages inside the given directory.")
      print("Call syntax: \n\tstella.py update <jsonfile> <dirname> \n\twhere \n\t\t<jsonfile> is the description of the tool in json format, presumably in the doc/ directory\n\t\t<dirname> is the directory to work in. Should be created by mode >prepare<.")
