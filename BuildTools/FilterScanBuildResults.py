#!/usr/bin/env python

import os, os.path, sys

resultsDir = sys.argv[1]
resultDirs = [ d for d in os.listdir(resultsDir) if os.path.isdir(os.path.join(resultsDir, d)) ]
resultDirs.sort()
if len(resultDirs) > 0 :
  resultFileName = os.path.join(resultsDir, resultDirs[-1], "index.html")
  resultData = []
  f = open(resultFileName, "r")
  skipLines = 0
  for line in f.readlines() :
    if skipLines > 0 :
      skipLines -= 1
    else :
      if "3rdParty" in line or "SHA1.cpp" in line or "lua.c" in line :
        skipLines = 2
      else :
        resultData.append(line)
  f.close()

  f = open(resultFileName, "w")
  f.writelines(resultData)
  f.close()
