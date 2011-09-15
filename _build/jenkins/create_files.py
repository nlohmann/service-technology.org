import re
import os
import shutil

from xml2dict import XML2Dict

# read data XML file
xml = XML2Dict()
data = xml.fromstring(open('../tools.xml', 'r').read())

templatedata = open('config.xml.template').read()

for t in data.tools.tool:
    try:
        os.makedirs('jobs/' + t.id)
    except OSError:
        pass

    o = open('jobs/' + t.id + '/config.xml', 'w+')
    r = re.sub('@DESCRIPTION@', 'Standard continuos integration job for ' + t.name, templatedata)
    r = re.sub('@PATH@', t.path, r)
    r = re.sub('@SHORTNAME@', t.shortname, r)

    o.write(r)
    o.close()
