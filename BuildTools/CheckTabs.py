#!/usr/bin/env python

import os, sys

foundExpandedTabs = False

filename = sys.argv[1]
if (filename.endswith(".cpp") or filename.endswith(".h")  or filename.endswith(".c") or filename.endswith(".mm") or filename.endswith(".ipp") or filename.endswith(".hpp") or filename.endswith(".py") or filename.endswith("SConscript") or filename.endswith("SConscript.boot") or filename.endswith("SConstruct")) and not "3rdParty" in filename : 
    file = open(filename, "r")
    contents = []
    contentsChanged = False
    for line in file.readlines() :
        if "\t" in line:
            print("File %s contains hard tabs. This is not allowed." % filename)
            file.close()
            sys.exit(-1)
    file.close()
    if contentsChanged :
        sys.exit(-1)
