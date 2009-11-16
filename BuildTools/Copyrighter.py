#!/usr/bin/env python
#coding=utf-8

import os, re, datetime

TEMPLATE = """/*
 * Copyright (c) %(year)s  %(author)s.
 * See the included COPYING file for license details.
 */

"""

def updateCopyright(fileName) :
  file = open(fileName)
  fileData = ""

  author = ""
  startYear = ""
  endYear = ""
  previousCopyright = ""
  
  # Retrieve previous copyright information
  header = ""
  inHeader = False
  inSpaceBelowHeader = False
  lines = file.readlines()
  lines2 = lines
  for line in lines2 :
    lines.pop(0)
    if inSpaceBelowHeader :
      if line.strip() != "" :
        break
    elif inHeader :
      if line.startswith(" */") :
        inSpaceBelowHeader = True
      else :
        header += line
    else :
      if line.strip() == "" :
        continue
      elif line.startswith("/*") :
        inHeader = True
        header += line
      else :
        fileData += line
        break
  if "Copyright" in header :
    previousCopyright = header
    m = re.match("\* Copyright \(c\) (?P<startYear>\d\d\d\d)(-(?P<endYear>\d\d\d\d))? (?P<author>.*)", header)
    if m :
      author = m.group("author")
      startYear = m.group("startYear")
      endYear = m.group("endYear")
  elif header != "" :
    fileData = header
  file.close()

  # Read in the rest of the data
  fileData += "".join(lines)

  # Guess empty values
  if author == "" :
    if "Swift/" in fileName :
      author = "Kevin Smith"
    else :
      author = u"Remko Tronçon"
  if startYear == "" :
    startYear = datetime.date.today().strftime("%Y")
  elif endYear == "" :
    ## TODO: Guess end year by looking at git log --pretty=format:%ai -- <filename>
    pass

  # Generate a copyright
  year = startYear + "-" + endYear if len(endYear) > 0 else startYear
  copyright = TEMPLATE % {
      "author" : author,
      "year" : year
    }

  # Write the copyright to the file
  if copyright.encode("utf-8") != previousCopyright :
    file = open(fileName, "w")
    file.write(copyright.encode("utf-8"))
    file.write(fileData)
    file.close()

for (path, dirs, files) in os.walk("Swiften/JID") :
  if "3rdParty" in path :
    continue
  for filename in files :
    if not filename.endswith(".cpp") and not filename.endswith(".h") :
      continue
    if filename.startswith("moc_") :
      continue
    fullFilename = path + "/" + filename
    updateCopyright(fullFilename)
