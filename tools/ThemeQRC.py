#!/usr/bin/env python

import os, sys, os.path

print "<RCC version =\"1.0\">"
print "<qresource prefix=\"/themes/Default\">"
for (path, dirs, files) in os.walk(sys.argv[1]) :
	for file in files :
		filePath = os.path.join(path,file)
		print "<file alias=\"%(alias)s\">%(path)s</file>" % { 
				"alias": filePath[len(sys.argv[1])+1:],
				"path": filePath
			}

print "</qresource>"
print "</RCC>"

