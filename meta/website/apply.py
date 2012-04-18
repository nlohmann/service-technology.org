#!/usr/bin/env python3

import sys
import json

if (len(sys.argv) < 4):
    sys.stderr.write("Usage: " + sys.argv[0] + " jsonfile peoplefile template target" + "\n")
    sys.exit(1)

j = json.loads(open(sys.argv[1], 'r').read())
p = json.loads(open(sys.argv[2], 'r').read())

t = open(sys.argv[3], 'r').read()
f = open(sys.argv[4], 'w')

if (len(j['authors'])) == 1:
    authors = j['authors'][0]

if (len(j['authors'])) == 2:
    authors = j['authors'][0] + ' and ' + j['authors'][1]

if (len(j['authors'])) > 2:
    authors = ''
    for i in range(0, len(j['authors'])-1):
        authors = authors + j['authors'][i] + ", "
    authors = authors + 'and ' + j['authors'][len(j['authors'])-1]

thanks = ''
for thank in j['thanks']:
    thanks = thanks + '<li>' + thank + '</li>' + "\n"

contribDict = dict()
contributors = ''
contribImages = ''
for cont in j['commits']:
    curuser = cont['user']
    if cont['user'] in p['refs']:
        curuser = p['refs'][curuser]

    if curuser in contribDict: 
       contribDict[curuser] = contribDict[curuser] + cont['commits']
    else:
       contribDict[curuser] = cont['commits']

contribList = sorted(contribDict, key = contribDict.get, reverse=True)
 
for cont in contribList:
    actname = cont 
    if cont in p['data']:
        actname = p['data'][cont]['name']

    contributors = contributors + '<li>' + actname + " (" + str(contribDict[cont])
    contribImages = contribImages + '<img width="75" src="../people/g/' + cont + '.jpg" title="' + actname + '" alt="' + actname +  '" class="portrait"> '
    if contribDict[cont] > 1:
        contributors = contributors + " commits"
    else: 
        contributors = contributors + " commit"
    contributors = contributors + ")" + '</li>' + "\n"

people = ''
for username in p['people']:
    people = people + '<li><img src="g/' + username + '.jpg" height="150" class="portrait"><br><a href="' + p['data'][username]['url'] + '">' + p['data'][username]['name'] + '</a><br> ' + p['data'][username]['affiliation'] + '</li>'

maintainerUserName = ''
maintainerMail = ''
maintainerURL = ''
for username in p['data']:
    if p['data'][username]['name'] == j['maintainer']:
       maintainerUserName = username
       maintainerMail = p['data'][username]['url']
       maintainerURL = p['data'][username]['url']

t = t.replace('@SHORTNAME@', j['shortname'])
t = t.replace('@TOOLNAME@',  j['toolname'])
t = t.replace('@TAGLINE@',   j['tagline'])
t = t.replace('@PURPOSE@',   j['purpose'])
t = t.replace('@AUTHORS@',   authors)
t = t.replace('@MAINTAINER@', j['maintainer'])
t = t.replace('@MAINTAINERUSERNAME@', maintainerUserName)
t = t.replace('@MAINTAINERMAIL@', maintainerMail)
t = t.replace('@MAINTAINERURL@', maintainerURL)
t = t.replace('@THANKS@', thanks)
t = t.replace('@BUCKTRACKERLINK@', j['bugtracker'])
t = t.replace('@TASKTRACKERLINK@', j['tasktracker'])
t = t.replace('@LICENSE@', j['license'])
t = t.replace('@RUNTIME@', j['runtime'])    
t = t.replace('@OFFICIALVERSION@', j['officialVersion'])    
t = t.replace('@CONTRIBUTORS@', contributors)
t = t.replace('@CONTRIBUTORIMAGES@', contribImages)
t = t.replace('@PEOPLE@', people)
f.write(t)
f.close()
