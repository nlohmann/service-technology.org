#!/usr/bin/env python3

import sys
import json

# FUNCTIONS


def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

def linkThis(someString, key):
  return '<a href="#paper-modal-' + key + '" data-toggle="modal">' + someString + '</a>'
  
    
def makeModal(entry, template):
  result = template
  result = result.replace('@BOOK@', entry['book'])
  result = result.replace('@AUTHORS@', entry['authors'])
  result = result.replace('@ADDITIONAL@', entry['additional'])
  result = result.replace('@TITLE@', entry['title'])
  result = result.replace('@MODALNAME@', "paper-modal-" + entry['key'])
  return result
    
# INITIALIZATION

if (len(sys.argv) < 5):
    sys.stderr.write("Usage: " + sys.argv[0] + " pubfile template modaltemplate noscripttemplate target" + "\n")
    sys.exit(1)
s = open(sys.argv[1], 'r').read()
print(s)
p = json.loads(s)
m = open(sys.argv[3], 'r').read()
mns = open(sys.argv[4], 'r').read()

replDict = dict();

# BUILDING THE REPLACEMENT DICTIONARY



replDict['paper-list-unstructured'] = ''
replDict['paper-modals'] = ''


for paper in p['publications']:
  replDict['paper-modals'] = replDict['paper-modals'] + makeModal(paper,m)
  replDict['paper-modals-noscript'] = replDict['paper-modals'] + makeModal(paper,mns)
  replDict['paper-list-unstructured'] = replDict['paper-list-unstructured'] + listThis(paper['authors'] + ". " + linkThis("<strong>" + paper['title'] + "</strong>",paper['key']))
  

# READ AND CLOSE TEMPLATE FILE

tfile = open(sys.argv[2], 'r')
t = tfile.read()
tfile.close()

# REPLACE ALL OCCURRENCES IN THE TARGET FILE 

f = open(sys.argv[5], 'w')

for someKey in replDict: 
    t = t.replace('@' + someKey.upper() + '@', replDict[someKey])    

f.write(t)
f.close()






