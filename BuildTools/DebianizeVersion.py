#!/usr/bin/env python

import sys

assert(len(sys.argv) == 2)

version = sys.argv[1]
version = version.replace("beta", "~beta")
version = version.replace("rc", "~rc")
version = version.replace("-dev", "+dev")

print version
