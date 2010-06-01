#!/usr/bin/env python

import sys
sys.path.append("SCons")
import Version

assert(len(sys.argv) == 2)

print Version.getBuildVersion(sys.argv[1])
