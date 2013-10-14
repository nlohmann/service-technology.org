#!/usr/bin/env python

import urllib
import os
import sys
import json

BASEURL = 'http://download.gna.org/service-tech/'
INSTALLDIR = 'tools'

DEPS = json.load(open("dependencies.json", 'r'))

def install(TOOL, VERSION):
    if TOOL in DEPS and VERSION in DEPS[TOOL]:
        pass
    else:
        print "Version not found."
        sys.exit(1)

    PATHS = []

    if 'dependencies' in DEPS[TOOL][VERSION]:
        for (t,v) in DEPS[TOOL][VERSION]['dependencies']:
            install(t,v)
            PATHS.append(INSTALLDIR + "/" + t + "-" + v)

    print "%s %s" % (TOOL, VERSION)

    if os.path.exists(INSTALLDIR + '/' + TOOL + '-' + VERSION):
        print "- %s-%s already installed" % (TOOL, VERSION)
        return

    url = BASEURL + TOOL + '/' + TOOL + '-' + VERSION + '.tar.gz'
    filename = TOOL + '-' + VERSION + '.tar.gz'

    def path_line():
        result = "PATH="
        for entry in PATHS:
            result += os.path.abspath("./" + entry) + "/bin:"
        result += "$PATH"
        
        result += " LDFLAGS='"
        for entry in PATHS:
            result += "-L" + os.path.abspath("./" + entry) + "/lib "
        result += "'"

        return result

    def download():
        print "- Downloading..."
        try:
            urllib.urlretrieve(url, filename)
        except:
            print "- Download failed."
            sys.exit(1) 

    def untar():
        os.system("tar xfz %s" % filename)

    def configure_make():
        if 'build' in DEPS[TOOL][VERSION]:
            MAKE_CALL = DEPS[TOOL][VERSION]['build']
        else:
            MAKE_CALL = 'make all'

        print "- Building..."
        
        os.system("cd %s ; %s ./configure --prefix=%s > /dev/null 2>&1" % (TOOL + "-" + VERSION, path_line(), os.path.abspath("./%s/%s" % (INSTALLDIR, TOOL + "-" + VERSION))))
        os.system(MAKE_CALL + " install --directory=%s > /dev/null 2>&1" % (TOOL + "-" + VERSION))
        
        print

    def cleanup():
        os.remove(filename)
        os.system("rm -fr " + TOOL + "-" + VERSION)

    download()
    untar()
    configure_make()
    cleanup()

#install('lola', '1.16')
#install('lola', '1.17')
#install('mia', '1.2')
#install('marlene', '1.2')
install('wendy', '3.3')
#install('sara', '1.13')
