#!/usr/bin/env python

import os, sys

foundExpandedTabs = False

filename = sys.argv[1]
if (filename.endswith(".cpp") or filename.endswith(".h") or filename.endswith(".py") or filename.endswith("SConscript") or filename.endswith("SConscript.boot") or filename.endswith("SConstruct")) and not "3rdParty" in filename : 
	file = open(filename, "r")
	contents = []
	contentsChanged = False
	for line in file.readlines() :
		newline = ""
		previousChar = None
		pastInitialSpace = False
		for char in line :
			if not pastInitialSpace :
				if char == ' ' and previousChar == ' ' :
					contentsChanged = True
					previousChar = '\t'
					continue
				pastInitialSpace = (char != ' ')
			if previousChar :
				newline += previousChar
			previousChar = char
		if previousChar :
			newline += previousChar
		contents.append(newline)
	file.close()
	if contentsChanged :
		sys.exit(-1)
