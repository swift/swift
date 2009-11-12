#!/usr/bin/env python

# TODO: Add uncovered non-ignored files

import sys, re, os.path

assert(len(sys.argv) == 2)

def isIgnored(file) :
  return (file.find("/Swiften/") == -1 and file.find("/Slimber/") == -1 and file.find("/Swift/") == -1) or (file.find("/UnitTest/") != -1 or file.find("/QA/") != -1)
  

output = []
inputFile = open(sys.argv[1])
inIgnoredFile = False
for line in inputFile.readlines() :
  if inIgnoredFile :
    if line == "end_of_record\n" :
      inIgnoredFile = False
  else :
    if line.startswith("SF:") and isIgnored(line) :
      inIgnoredFile = True
    else :
      m = re.match("SF:(.*)", line)
      if m :
        line = "SF:" + os.path.realpath(m.group(1)) + "\n"
      output.append(line)
inputFile.close()

outputFile = open(sys.argv[1], 'w')
outputFile.write(''.join(output))
outputFile.close()
