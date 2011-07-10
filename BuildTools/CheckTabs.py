#!/usr/bin/env python

import os, sys

foundExpandedTabs = False

for (path, dirs, files) in os.walk(".") :
	if not "3rdParty" in path and not ".sconf" in path and not ".framework" in path and not path.startswith("build") :
		for filename in [os.path.join(path, file) for file in files if (file.endswith(".cpp") or file.endswith(".h")) and not "ui_" in file and not "moc_" in file and not "qrc_" in file] :
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
				if len(sys.argv) > 1 and sys.argv[1] == "--fix" :
					print "Fixing tabs in " + filename
					file = open(filename, "w")
					file.write(''.join(contents))
					file.close()
				else :
					foundExpandedTabs = True
					print filename + " contains expanded tabs"

sys.exit(foundExpandedTabs)
