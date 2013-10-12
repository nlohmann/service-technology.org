#!/usr/bin/env python3

import sys
import json
import os
import collections

# FUNCTIONS


def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

def linkThis(someString):
    return '<a href="../' + someString.lower() + '/index.html">' + someString + '</a>'

    
    
def getTagLine(svnname, toolname):
  return json.loads(os.popen("curl http://svn.gna.org/viewcvs/*checkout*/service-tech/trunk/" + svnname  + "/doc/" + toolname.lower() + ".json").read())['tagline']

  
if (len(sys.argv) < 4):
    sys.stderr.write("Usage: " + sys.argv[0] + " toolfile template target" + "\n")
    sys.exit(1)

p = json.loads(open(sys.argv[1], 'r').read(), object_pairs_hook=collections.OrderedDict)


replDict = dict();

# BUILDING THE REPLACEMENT DICTIONARY

tags = p['tags']
tags["misc"] = "Uncategorized"

tagMap = dict()

for tag in tags:
  print(tag)
  tagMap[tag] = list()


toollist = list()
taglines = dict()
for tool in p['tools']:
    taglines[tool['toolname']] = getTagLine(tool['svnname'],tool['toolname'])
    toollist.append(tool['toolname'])
    if "tags" in tool:
      taglist = tool["tags"]
      for tag in taglist:
        tagMap[tag].append(tool['toolname'])
    else:
      tagMap["misc"].append(tool['toolname'])

for tool in p['legacy-tools']:
    if "tagline" in tool:
      taglines[tool['toolname']] = tool['tagline']
    toollist.append(tool['toolname'])
    if "tags" in tool:
      taglist = tool["tags"]
      for tag in taglist:
        tagMap[tag].append(tool['toolname'])
    else:
      tagMap["misc"].append(tool['toolname'])

toollist.sort()

#          <ul id="tool-list">
#            <li  id="service-formalization">Service formalization
#              <ul>
#                <li>
#                  <a href="../bpel2owfn/index.html">BPEL2oWFN</a> <em>translating WS-BPEL processes into service nets</em>
#                </li>


print(tagMap)  

replDict['tool-list-unstructured'] = '<ul>'

for tool in toollist:
    linestring = ""
    if tool in taglines:
      linestring = " <em>" + taglines[tool] + "</em>"
    replDict['tool-list-unstructured'] = replDict['tool-list-unstructured'] + listThis(linkThis(tool) + linestring)

replDict['tool-list-unstructured'] = replDict['tool-list-unstructured'] + '</ul>'

replDict['tool-list'] = '<ul>'

for tag in tags:
  tagMap[tag].sort()
  replDict['tool-list'] = replDict['tool-list'] + "<li id='" + tag + "'>" + tags[tag] + "\n <ul>"
  for tool in tagMap[tag]:
    linestring = ""
    if tool in taglines:
      linestring = " <em>" + taglines[tool] + "</em>"
    replDict['tool-list'] = replDict['tool-list'] + listThis(linkThis(tool) + linestring)
  replDict['tool-list'] = replDict['tool-list'] + "</ul>\n"

replDict['tool-list'] = replDict['tool-list'] + "</ul>\n"




# READ AND CLOSE TEMPLATE FILE

tfile = open(sys.argv[2], 'r')
t = tfile.read()
tfile.close()

# REPLACE ALL OCCURRENCES IN THE TARGET FILE 

f = open(sys.argv[3], 'w')

for someKey in replDict: 
    t = t.replace('@' + someKey.upper() + '@', replDict[someKey])    

f.write(t)
f.close()






