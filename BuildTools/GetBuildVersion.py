#!/usr/bin/env python

import sys, re
sys.path.append("SCons")
import Version, os.path

assert(len(sys.argv) >= 2)

only_major = False
if "--major" in sys.argv :
  only_major = True

if only_major :
  v = Version.getBuildVersion(os.path.dirname(sys.argv[0] + "/.."), sys.argv[1])
  version_match = re.match("(\d+)\.(\d+).*", v)
  if version_match :
    print version_match.group(1)
  else :
    print "0"
else :
  print Version.getBuildVersion(os.path.dirname(sys.argv[0] + "/.."), sys.argv[1])
