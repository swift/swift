#!/usr/bin/env python

import os, sys

FORBIDDEN_INCLUDES = [
	("iostream", ["Swiften/Base/format.h"]), 
	("Base/Log.h", []), 
	("Base/format.h", []),
	("algorithm", ["Swiften/Base/Algorithm.h", "Swiften/Base/SafeAllocator.h", "Swiften/Base/Listenable.h"]), 
	("boost/bind.hpp", ["Swiften/Base/Listenable.h"]), 
	("boost/filesystem.hpp", []), 
	("Base/foreach.h", []), 
	("boost/date_time/date_time.hpp", []), 
	("boost/filesystem/filesystem.hpp", []),

	# To avoid
	("Base/Algorithm.h", ["Swiften/StringCodecs/HMAC.h"]),
]

foundBadHeaders = False

filename = sys.argv[1]

if "3rdParty" in filename or ".sconf" in filename or ".framework" in filename or not filename.endswith(".h") :
	sys.exit(0)
if not "Swiften" in filename :
	sys.exit(0)
if filename.endswith("Swiften.h") :
	sys.exit(0)

file = open(filename, "r")
for line in file.readlines() :
	if not "#include" in line :
		continue
	if "Base/Log.h" in filename :
		continue
	for forbiddenInclude, ignores in FORBIDDEN_INCLUDES :
		if forbiddenInclude in line and len([x for x in ignores if x in filename]) == 0 :
			print "Found " + forbiddenInclude + " include in " + filename
			foundBadHeaders = True

sys.exit(foundBadHeaders)
