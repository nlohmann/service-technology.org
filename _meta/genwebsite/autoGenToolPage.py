#!/usr/bin/env python3

import sys
import json
import os

import utils.update_json
import utils.update_website

jsonfile = sys.argv[1]
targetDir = sys.argv[2] + "/"

# set the path to the jsonfile
path = os.path.dirname(jsonfile) + "/"

# find the shortname from the json
shortname = json.loads(open(jsonfile, "r").read())["shortname"]

print(path)

# generate the generic.json
utils.update_json.generate(shortname, path)

# mine everything
utils.update_website.mineAll(jsonfile, path+"generic.json", "data/people.json", "data/requirements.json", "data/variables.json")

# write each single file
for file in ["download", "getInvolved", "index",  "science", "support"]:
  utils.update_website.writeFile("template/tool/" + file + ".html", targetDir + file +".html")
