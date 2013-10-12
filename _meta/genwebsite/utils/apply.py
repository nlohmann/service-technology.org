#!/usr/bin/env python3

import sys
import json
import misaka

# FUNCTIONS

def condInsert(key,f,default=''):
    global j
    global replDict    
    if key in j:
        setFlag(key)
        f(key)
    else:
        replDict[key] = default
        remFlag(key)


def getCommitString(nrOfCommits):
    if nrOfCommits == 1:
        return str(nrOfCommits) + " commit"
    else:
        return str(nrOfCommits) + " commits"

def getHTML(md):
    return misaka.html(md)

def simpleInsert(key):
    global replDict
    global j
    replDict[key] = j[key]

def htmlInsert(key):
    global replDict
    global j
    replDict[key] = getHTML(j[key])

def setFlag(key):
    global replDict
    replDict[key+ 'GIVEN'] = 'normal'

def remFlag(key):
    global replDict
    replDict[key + 'GIVEN'] = 'none'

def buildReqList(key): 
    result = ''
    for req in j[key]:
        curReq = req
        if req in r:
            curReq = '<a href="' + r[req]['url'] + '" title="' + r[req]['name'] + '">' + r[req]['name'] + '</a> - ' + r[req]['desc']
        result += listThis(curReq)    
    return result

def setRequirements(key):
    if key in j:
        replDict[key] = buildReqList(key)
        if replDict[key] != '' :
            setFlag(key)
        else:
            remFlag(key)
    else:
        replDict['key'] = 'none'
        remFlag(key)

def setAuthors(): 
    global j
    global replDict
    if (len(j['authors'])) == 1:
        replDict['authors'] = j['authors'][0]

    if (len(j['authors'])) == 2:
        replDict['authors'] = j['authors'][0] + ' and ' + j['authors'][1]

    if (len(j['authors'])) > 2:
        replDict['authors']  = ''
        for i in range(0, len(j['authors'])-1):
            replDict['authors']  = replDict['authors'] + j['authors'][i] + ", "
        replDict['authors']  = replDict['authors'] + 'and ' + j['authors'][len(j['authors'])-1]

def setThanks():
    global j
    global replDict
    replDict['thanks']  = ''
    if 'thanks' in j:
        for thank in j['thanks']:
            replDict['thanks'] = replDict['thanks'] + listThis(thank)
        setFlag('thanks')
    else:
        remFlag('thanks')

def setContributors():
    global j
    global p
    global replDict
    contribDict = dict()

    for cont in j['commits']:
        curuser = cont['user']
        if cont['user'] in p['refs']:
            curuser = p['refs'][curuser]

        if curuser in contribDict: 
           contribDict[curuser] = contribDict[curuser] + cont['commits']
        else:
           contribDict[curuser] = cont['commits']

    contribList = sorted(contribDict, key = contribDict.get, reverse=True)

    replDict['contributors'] = ''
    replDict['contributorimages'] = '' 
    for cont in contribList:
        actname = cont 
        if cont in p['data']:
            actname = p['data'][cont]['name']
        if cont in p['people']:
            replDict['contributorimages'] = replDict['contributorimages'] + getImageTag('../people/g/' + cont + '.jpg', actname, actname, 'class="portrait" width="75"')
        else:
                replDict['contributorimages'] = replDict['contributorimages'] + getImageTag('../people/g/dummy.jpg', actname, actname, 'class="portrait" width="75"')
        replDict['contributors'] = replDict['contributors'] + listThis( actname + " (" + getCommitString(contribDict[cont]) + ")")


def getImageTag(filename, alt, title, additional = ''):
    return '<img src="' + filename + '" alt="' + alt + '" title="' + title + '" ' + additional + '>'

def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

# INITIALIZATION

if (len(sys.argv) < 6):
    sys.stderr.write("Usage: " + sys.argv[0] + " jsonfile genericfile peoplefile reqfile template target" + "\n")
    sys.exit(1)


j1 = json.loads(open(sys.argv[1], 'r').read())
j2 = json.loads(open(sys.argv[2], 'r').read())
j = dict(list(j1.items()) + list(j2.items()))
p = json.loads(open(sys.argv[3], 'r').read())
r = json.loads(open(sys.argv[4], 'r').read())
replDict = dict();

# BUILDING THE REPLACEMENT DICTIONARY

# Tool names
simpleInsert('toolname')
condInsert('shortname', simpleInsert, replDict['toolname'])
condInsert('svnname', simpleInsert, replDict['shortname'])

# Tagline, purpose, features

simpleInsert('tagline')
htmlInsert('purpose')
condInsert('features', htmlInsert)

# Help

condInsert('faq', htmlInsert)
condInsert('screencast', simpleInsert)
condInsert('quickstart', htmlInsert)

# Science

condInsert('science', htmlInsert)

# Maintainer and support 

simpleInsert('maintainer')
condInsert('bugtracker', simpleInsert, "http://gna.org/task/?group=service-tech")
condInsert('tasktracker', simpleInsert, "http://gna.org/bugs/?group=service-tech")
replDict['maintainerusername'] = ''
replDict['maintainermail'] = ''
replDict['maintainerurl'] = ''
for username in p['data']:
    if p['data'][username]['name'] == j['maintainer']:
       replDict['maintainerusername'] = username
       replDict['maintainermail'] = p['data'][username]['url']
       replDict['maintainerurl'] = p['data'][username]['url']


# Download / Live 

simpleInsert('officialVersion')
simpleInsert('license')
condInsert('livelink', simpleInsert, "http://esla.informatik.uni-rostock.de/service-tech/live/#" + replDict['shortname'])
setRequirements('req_compile')
setRequirements('req_tests')
setRequirements('req_edit')
setRequirements('runtime')



# Authors, Contributors, Ackknowledgements
setAuthors()
setThanks()
setContributors()


# READ AND CLOSE TEMPLATE FILE

tfile = open(sys.argv[5], 'r')
t = tfile.read()
tfile.close()

# REPLACE ALL OCCURRENCES IN THE TARGET FILE 

f = open(sys.argv[6], 'w')

for someKey in replDict: 
    t = t.replace('@' + someKey.upper() + '@', replDict[someKey])    

f.write(t)
f.close()






