#!/usr/bin/env python

import os

TEMPLATE = """/*
 * Copyright (c) %(year)s %(author)s
 * %(license)s
 */"""

for (path, dirs, files) in os.walk("src") :
	if "3rdParty" in path :
		continue
	for filename in files :
		if not filename.endswith(".cpp") and not filename.endswith(".h") :
			continue
		if filename.startswith("moc_") :
			continue
		fullFilename = path + "/" + filename
		print fullFilename
