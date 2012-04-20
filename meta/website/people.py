#!/usr/bin/env python3

import sys
import json

# FUNCTIONS

def getImageTag(filename, alt, title, additional = ''):
    return '<img src="' + filename + '" alt="' + alt + '" title="' + title + '" ' + additional + '>'

def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

# INITIALIZATION

if (len(sys.argv) < 3):
    sys.stderr.write("Usage: " + sys.argv[0] + " peoplefile template target" + "\n")
    sys.exit(1)

p = json.loads(open(sys.argv[1], 'r').read())

replDict = dict();

# BUILDING THE REPLACEMENT DICTIONARY

# People page

replDict['people'] = ''
for username in p['people']:
    replDict['people'] = replDict['people'] + listThis( getImageTag('g/' + username + '.jpg', p['data'][username]['name'], p['data'][username]['name'], 'height="150" class="portrait"') + '<br><a href="' + p['data'][username]['url'] + '">' + p['data'][username]['name'] + '</a><br> ' + p['data'][username]['affiliation'])


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






