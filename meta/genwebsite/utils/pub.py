#!/usr/bin/env python3

import sys
import json

# FUNCTIONS


def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

def linkThis(someString, papercounter):
  return '<a href="#paper-modal-' + str(papercounter) + '" data-toggle="modal">' + someString + '</a>'
  
    
def makeModal(entry, template, papercounter):
  result = template
  result = result.replace('@BOOK@', entry['book'])
  result = result.replace('@AUTHORS@', entry['authors'])
  result = result.replace('@ADDITIONAL@', entry['additional'])
  result = result.replace('@TITLE@', entry['title'])
  result = result.replace('@MODALNAME@', "paper-modal-" + str(papercounter))
  return result
    
# INITIALIZATION

if (len(sys.argv) < 4):
    sys.stderr.write("Usage: " + sys.argv[0] + " pubfile template modaltemplate target" + "\n")
    sys.exit(1)
s = open(sys.argv[1], 'r').read()
print(s)
p = json.loads(s)
m = open(sys.argv[3], 'r').read()
replDict = dict();

# BUILDING THE REPLACEMENT DICTIONARY



replDict['paper-list-unstructured'] = ''
replDict['paper-modals'] = ''

papercounter = 1

for paper in p['publications']:
  replDict['paper-modals'] = replDict['paper-modals'] + makeModal(paper,m,papercounter)
  replDict['paper-list-unstructured'] = replDict['paper-list-unstructured'] + listThis(paper['authors'] + ". " + linkThis("<strong>" + paper['title'] + "</strong>",papercounter))
  papercounter = papercounter +1 
  

# READ AND CLOSE TEMPLATE FILE

tfile = open(sys.argv[2], 'r')
t = tfile.read()
tfile.close()

# REPLACE ALL OCCURRENCES IN THE TARGET FILE 

f = open(sys.argv[4], 'w')

for someKey in replDict: 
    t = t.replace('@' + someKey.upper() + '@', replDict[someKey])    

f.write(t)
f.close()






