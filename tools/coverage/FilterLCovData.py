#!/usr/bin/env python

import sys, re

assert(len(sys.argv) == 2)

output = []
inputFile = open(sys.argv[1])
inIgnoredFile = False
for line in inputFile.readlines() :
  if inIgnoredFile :
    if line == "end_of_record\n" :
      inIgnoredFile = False
  else :
    if line.startswith("SF:") and (line.find("/Swift/") == -1 or line.find("/UnitTest/") != -1 or line.find("/QA/") != -1 or line.find("/3rdParty/") != -1):
      inIgnoredFile = True
    else :
      output.append(line)
inputFile.close()

outputFile = open(sys.argv[1], 'w')
outputFile.write(''.join(output))
outputFile.close()
