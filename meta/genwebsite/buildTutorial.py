#!/usr/bin/env python3

import sys

try:
    import misaka
    if (len(sys.argv) == 1):
        outputname = "tutorial.html"
    else:
        outputname = sys.argv[1]

    g = open("tutorial.md", 'r')
    f = open(outputname, 'w')
    f.write(misaka.html(g.read(), render_flags=misaka.HTML_ESCAPE))
    g.close()
    f.close()
except ImportError: 
    print("The module 'Misaka' to convert markdown to html from python could not be loaded. Please find Misaka at http://misaka.61924.nl/")





