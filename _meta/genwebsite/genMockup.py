#!/usr/bin/env python3

import json
import sys
import os

toollist = json.loads(open("data/tools.json").read())

scriptname = "./autoGenToolPage-mockup.sh"
username = sys.argv[1]
workspace = sys.argv[2]


for tool in toollist["tools"]:
  call = scriptname + " " + tool["toolname"] + "  " + tool["svnname"] + " " + username + " " + workspace
  os.system(call)