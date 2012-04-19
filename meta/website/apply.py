#!/usr/bin/env python3

import sys
import json
import misaka


if (len(sys.argv) < 4):
    sys.stderr.write("Usage: " + sys.argv[0] + " jsonfile peoplefile reqfile template target" + "\n")
    sys.exit(1)

j = json.loads(open(sys.argv[1], 'r').read())
p = json.loads(open(sys.argv[2], 'r').read())
r = json.loads(open(sys.argv[3], 'r').read())

t = open(sys.argv[4], 'r').read()
f = open(sys.argv[5], 'w')

replDict = dict();

if (len(j['authors'])) == 1:
    replDict['@AUTHORS@'] = j['authors'][0]

if (len(j['authors'])) == 2:
    replDict['@AUTHORS@'] = j['authors'][0] + ' and ' + j['authors'][1]

if (len(j['authors'])) > 2:
    replDict['@AUTHORS@']  = ''
    for i in range(0, len(j['authors'])-1):
        replDict['@AUTHORS@']  = replDict['@AUTHORS@'] + j['authors'][i] + ", "
    replDict['@AUTHORS@']  = replDict['@AUTHORS@'] + 'and ' + j['authors'][len(j['authors'])-1]

replDict['@THANKS@']  = ''
for thank in j['thanks']:
    replDict['@THANKS@'] = replDict['@THANKS@'] + '<li>' + thank + '</li>' + "\n"

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

replDict['@CONTRIBUTORS@'] = ''
replDict['@CONTRIBUTORIMAGES@'] = '' 
for cont in contribList:
    actname = cont 
    if cont in p['data']:
        actname = p['data'][cont]['name']

    replDict['@CONTRIBUTORS@'] = replDict['@CONTRIBUTORS@'] + '<li>' + actname + " (" + str(contribDict[cont])
    replDict['@CONTRIBUTORIMAGES@'] = replDict['@CONTRIBUTORIMAGES@'] + '<img width="75" src="../people/g/' + cont + '.jpg" title="' + actname + '" alt="' + actname +  '" class="portrait"> '
    if contribDict[cont] > 1:
        replDict['@CONTRIBUTORS@'] = replDict['@CONTRIBUTORS@'] + " commits"
    else: 
        replDict['@CONTRIBUTORS@'] = replDict['@CONTRIBUTORS@'] + " commit"
    replDict['@CONTRIBUTORS@'] = replDict['@CONTRIBUTORS@'] + ")" + '</li>' + "\n"

replDict['@PEOPLE@'] = ''
for username in p['people']:
    replDict['@PEOPLE@'] = replDict['@PEOPLE@'] + '<li><img src="g/' + username + '.jpg" height="150" class="portrait"><br><a href="' + p['data'][username]['url'] + '">' + p['data'][username]['name'] + '</a><br> ' + p['data'][username]['affiliation'] + '</li>'

replDict['@REQ_COMPILE@'] = 'none'
replDict['@REQ_COMPILEGIVEN@'] = 'none'
if 'req_compile' in j: 
    replDict['@REQ_COMPILE@'] = ''
    replDict['@REQ_COMPILEGIVEN@'] = 'normal'
    for req in j['req_compile']:
        curReq = req
        if req in r:
            curReq = '<a href="' + r[req]['url'] + '" title="' + r[req]['name'] + '">' + r[req]['name'] + '</a> - ' + r[req]['desc']
        replDict['@REQ_COMPILE@'] = replDict['@REQ_COMPILE@'] + '<li>' + curReq + '</li>'

replDict['@REQ_TESTS@'] = 'none'
replDict['@REQ_TESTSGIVEN@'] = 'none'
if 'req_tests' in j: 
    replDict['@REQ_TESTS@'] = ''
    replDict['@REQ_TESTSGIVEN@'] = 'normal'
    for req in j['req_tests']:
        curReq = req
        if req in r:
            curReq = '<a href="' + r[req]['url'] + '" title="' + r[req]['name'] + '">' + r[req]['name'] + '</a> - ' + r[req]['desc']
        replDict['@REQ_TESTS@'] = replDict['@REQ_TESTS@'] + '<li>' + curReq + '</li>'

replDict['@REQ_EDIT@'] = 'none'
replDict['@REQ_EDITGIVEN@'] = 'none'
if 'req_edit' in j: 
    replDict['@REQ_EDIT@'] = ''
    replDict['@REQ_EDITGIVEN@'] = 'normal'
    for req in j['req_edit']:
        curReq = req
        if req in r:
            curReq = '<a href="' + r[req]['url'] + '" title="' + r[req]['name'] + '">' + r[req]['name'] + '</a> - ' + r[req]['desc']
        replDict['@REQ_EDIT@'] = replDict['@REQ_EDIT@'] + '<li>' + curReq + '</li>'

replDict['@RUNTIME@'] = 'none'
replDict['@REQ_RUNTIMEGIVEN@'] = 'none'
if 'runtime' in j: 
    replDict['@RUNTIME@'] = ''
    replDict['@REQ_RUNTIMEGIVEN@'] = 'normal'
    for req in j['runtime']:
        curReq = req
        if req in r:
            curReq = '<a href="' + r[req]['url'] + '" title="' + r[req]['name'] + '">' + r[req]['name'] + '</a> - ' + r[req]['desc']
        replDict['@RUNTIME@'] = replDict['@RUNTIME@'] + '<li>' + curReq + '</li>'

replDict['@MAINTAINERUSERNAME@'] = ''
replDict['@MAINTAINERMAIL@'] = ''
replDict['@MAINTAINERURL@'] = ''
for username in p['data']:
    if p['data'][username]['name'] == j['maintainer']:
       replDict['@MAINTAINERUSERNAME@'] = username
       replDict['@MAINTAINERMAIL@'] = p['data'][username]['url']
       replDict['@MAINTAINERURL@'] = p['data'][username]['url']


replDict['@TOOLNAME@'] = j['toolname']

if 'shortname' in j:
    replDict['@SHORTNAME@'] = j['shortname']
else:
    replDict['@SHORTNAME@'] = replDict['@TOOLNAME@']

if 'svnname' in j:
    replDict['@SVNNAME@'] = j['svnname']
else:
    replDict['@SVNNAME@'] = replDict['@SHORTNAME@']

replDict['@TAGLINE@' ] = j['tagline']
replDict['@PURPOSE@']  =  misaka.html(j['purpose'])
replDict['@MAINTAINER@'] = j['maintainer']
replDict['@BUCKTRACKERLINK@'] = j['bugtracker']
replDict['@TASKTRACKERLINK@'] = j['tasktracker']
replDict['@LICENSE@'] = j['license']
replDict['@OFFICIALVERSION@'] = j['officialVersion']

if 'faq' in j:
    replDict['@FAQGIVEN@'] = 'normal'
    replDict['@FAQ@'] = misaka.html(j['faq'])
else:
    replDict['@FAQGIVEN@'] = 'none'

if 'screencast' in j:
    replDict['@SCREENCASTGIVEN@'] = 'normal'
    replDict['@SCREENCAST@'] = j['screencast']
else:
    replDict['@SCREENCASTGIVEN@'] = 'none'


if 'features' in j:
    replDict['@FEATURES@'] = misaka.html(j['features'])
else:
    replDict['@FEATURES@'] = ''


for someKey in replDict: 
    t = t.replace(someKey, replDict[someKey])    

f.write(t)
f.close()
