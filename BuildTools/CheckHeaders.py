#!/usr/bin/env python

import os, sys

foundBadHeaders = False

for (path, dirs, files) in os.walk(".") :
  if "3rdParty" in path or ".sconf" in path or ".framework" in path :
    continue
  if not "Swiften" in path :
    continue

  for filename in [os.path.join(path, file) for file in files if file.endswith(".h")] :
    file = open(filename, "r")
    for line in file.readlines() :
      for include in ["iostream", "algorithm", "cassert", "boost/bind.hpp", "boost/filesystem.hpp", "Base/foreach.h", "Base/Log.h", "boost/date_time/date_time.hpp", "boost/filesystem/filesystem.hpp"] :
        if "#include" in line and include in line and not "Base/Log" in filename :
          print "Found " + include + " include in " + filename
          foundBadHeaders = True

sys.exit(foundBadHeaders)
