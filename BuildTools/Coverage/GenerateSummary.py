#!/usr/bin/env python

import sys, re

assert(len(sys.argv) == 3)

inputFile = open(sys.argv[1])
currentFile = ""
coverage = {}
for line in inputFile.readlines() :
  line = line.strip()
  m = re.match("^SF:(.*)", line)
  if m :
    currentFile = m.group(1)
  else :
    m = re.match("^DA:(\d+),(\d+)", line)
    if m :
      currentFileCoverage = coverage.get(currentFile, {})
      line = int(m.group(1))
      count = int(m.group(2))
      currentFileCoverage[line] = currentFileCoverage.get(line, 0) + count
      coverage[currentFile] = currentFileCoverage
inputFile.close()

totalLines = 0
coveredLines = 0
for c in coverage.values() :
  totalLines += len(c)
  for l in c.values() :
    if l > 0 :
      coveredLines += 1

outputFile = open(sys.argv[2], 'w')
outputFile.write(str(coveredLines) + "/" + str(totalLines))
outputFile.close()
