#!/usr/bin/env python3

import sys
import json

import utils.update_json
import utils.update_website

jsonfile = sys.argv[1]
targetDir = sys.argv[2] + "/"

# set the path to the jsonfile
path = os.path.dirname(jsonfile) + "/"

# find the shortname from the json
shortname = json.loads(open(jsonfile, "r").read())[shortname]

# generate the generic.json
utils.update_json.generate(shortname, path)

def mineAll(jsonfile, genericfile, peoplefile, reqfile):

# mine everything
utils.update_website.mineAll(jsonfile, path+"generic.json", "data/people.json", "data/requirements.json")

# write each single file
for file in ["download", "getInvolved", "help", "index",  "science", "support"]:
  utils.update_website.writeFile("template/" + file + ".html", targetDir + file +".html")
