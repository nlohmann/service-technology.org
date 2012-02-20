#!/usr/bin/env python
import re
import os
import shutil
import sys

from xml2dict import XML2Dict

# read data XML file
xml = XML2Dict()
data = xml.fromstring(open('../tools.xml', 'r').read())

# standard jobs
templatedata = open('config.xml.template').read()

for t in data.tools.tool:
    try:
        os.makedirs('jobs/' + t.id)
    except OSError:
        pass

    o = open('jobs/' + t.id + '/config.xml', 'w+')
    r = re.sub('@DESCRIPTION@', 'Standard continuos integration job for ' + t.name, templatedata)
    r = re.sub('@PATH@', t.path, r)
    r = re.sub('@NAME@', t.name, r)
    r = re.sub('@SHORTNAME@', t.shortname, r)

    o.write(r)
    o.close()

sys.exit()

# Windows jobs
templatedata = open('config-win.xml-template').read()

for t in data.tools.tool:
    try:
        os.makedirs('jobs/' + t.id + '-win')
    except OSError:
        pass

    o = open('jobs/' + t.id + '-win/config.xml', 'w+')
    r = re.sub('@DESCRIPTION@', 'Standard continuos integration job for ' + t.name, templatedata)
    r = re.sub('@PATH@', t.path, r)
    r = re.sub('@NAME@', t.name, r)
    r = re.sub('@SHORTNAME@', t.shortname, r)

    o.write(r)
    o.close()
