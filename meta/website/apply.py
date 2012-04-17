#!/usr/bin/env python3

import sys
import json

if (len(sys.argv) < 4):
    sys.stderr.write("Usage: " + sys.argv[0] + " jsonfile template target" + "\n")
    sys.exit(1)

j = json.loads(open(sys.argv[1], 'r').read())
t = open(sys.argv[2], 'r').read()
f = open(sys.argv[3], 'w')

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

t = t.replace('@SHORTNAME@', j['shortname'])
t = t.replace('@TOOLNAME@',  j['toolname'])
t = t.replace('@TAGLINE@',   j['tagline'])
t = t.replace('@PURPOSE@',   j['purpose'])
t = t.replace('@AUTHORS@',   authors)
t = t.replace('@MAINTAINER@', j['maintainer'])
t = t.replace('@THANKS@', thanks)
t = t.replace('@BUCKTRACKERLINK@', j['bugtracker'])
t = t.replace('@TASKTRACKERLINK@', j['tasktracker'])
t = t.replace('@LICENSE@', j['license'])
t = t.replace('@RUNTIME@', j['runtime'])
f.write(t)
f.close()
