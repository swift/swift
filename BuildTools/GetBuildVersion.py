#!/usr/bin/env python

import sys
sys.path.append("SCons")
import Version, os.path

assert(len(sys.argv) == 2)

print Version.getBuildVersion(os.path.dirname(sys.argv[0] + "/.."), sys.argv[1])
