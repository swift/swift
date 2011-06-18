#!/usr/bin/env python

import sys, re, email.utils

assert(len(sys.argv) == 3)

version = sys.argv[2]

changelog = open(sys.argv[1])
last_version_line = changelog.readline()
changelog.close()

project = ""
last_version = ""
m = re.match("([\w-]+) \((.*)-\d+\)", last_version_line)
if m :
  project = m.group(1)
  last_version = m.group(2)

if project == "" :
  project="swift-im"

if "dev" in version :
  distribution = "development"
elif "beta" in version or "rc" in version :
  distribution = "beta development"
else :
  distribution = "release beta development"

if last_version != version :
  changelog = open(sys.argv[1])
  changelog_data = changelog.read()
  changelog.close()
  changelog = open(sys.argv[1], "w")
  changelog.write(project + " (" + version + "-1)" + " " + distribution + "; urgency=low\n\n")
  changelog.write("  * Upstream development snapshot\n\n")
  changelog.write(" -- Swift Package Maintainer <packages@swift.im>  " + email.utils.formatdate() + "\n")
  changelog.write("\n")
  changelog.write(changelog_data)
  changelog.close()

