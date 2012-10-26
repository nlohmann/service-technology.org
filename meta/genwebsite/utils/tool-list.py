#!/usr/bin/env python3

import sys
import json

# FUNCTIONS


def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

def linkThis(someString):
    return '<a href="../' + someString.lower() + '/index.html">' + someString + '</a>'
    
# INITIALIZATION

if (len(sys.argv) < 3):
    sys.stderr.write("Usage: " + sys.argv[0] + " toolfile template target" + "\n")
    sys.exit(1)

p = json.loads(open(sys.argv[1], 'r').read())

replDict = dict();

# BUILDING THE REPLACEMENT DICTIONARY

# People page

replDict['tool-list-unstructured'] = '<ul>'
toollist = list()
for tool in p['tools']:
    toollist.append(tool['toolname'])
for tool in p['legacy-tools']:
    toollist.append(tool['toolname'])

toollist.sort()

for tool in toollist:
    replDict['tool-list-unstructured'] = replDict['tool-list-unstructured'] + listThis(linkThis(tool))

replDict['tool-list-unstructured'] = replDict['tool-list-unstructured'] + '</ul>'

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






