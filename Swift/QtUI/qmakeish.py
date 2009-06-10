#!/usr/bin/env python
# Run this from the Swift/QtUI dir with:
# ./qmakeish.py  ../../Makefile > Swiften.pri

import sys, re, os.path

def processSourcesLine(line) :
  strippedLine = line.rstrip("\n")
  sourceFile = re.sub("\\\\$", "", strippedLine).strip()
  if len(sourceFile) > 0 :
    print "SOURCES += $$PWD/../../" + sourceFile
  return strippedLine.endswith("\\")

def processFlags(name, flags) :
  flags = flags.replace("-isystem ", "-I")
  for flag in flags.split(" ") :
    if flag.startswith("-D") :
      print "DEFINES += " + flag[2:]
    elif flag.startswith("-I") :
      print "INCLUDEPATH += $$PWD/../../" + flag[2:]
    elif len(flag) > 0 :
      print name + " += " + flag


assert(len(sys.argv) == 2)

basedir = os.path.dirname(sys.argv[1])

# Flatten the makefile
makefile = []
files = [open(sys.argv[1])]
while len(files) > 0 :
  file = files[-1]
  line = file.readline()
  if line :
    match = re.match("include (.*)", line)
    if match and match.group(1) != "Makefile.config" :
      files.append(open(os.path.join(basedir, match.group(1))))
      makefile.append("## Begin File: " + match.group(1))
    else :
      makefile.append(line)
  else :
    makefile.append("## End file")
    file.close()
    files.pop()

# Process makefile
inSources = False
for line in makefile :
  if inSources :
    inSources = processSourcesLine(line)
  else :
    # Conditional
    match = re.match("if(n?)eq \(\$\((.*)\),(.*)\)", line)
    if match :
      conditional = match.group(2)
      if conditional == "WIN32" :
        conditional = "win32"
      elif conditional == "MACOSX" :
        conditional = "mac"
      elif match.group(2).startswith("HAVE_") or match.group(2).startswith("USE_") :
        conditional = "!isEmpty(" + match.group(2) + ")"
      else :
        conditional = "DUMMY"
      if (match.group(1) == "n") ^ (match.group(3) not in ["1", "yes"]) :
        conditional = "!" + conditional
      print conditional + " {"
      continue
    if re.match("^if(n?)def", line) :
      print "DUMMY {"
      continue
    elif re.match("^if(n?)eq", line) :
      print "DUMMY {"
      continue
    if re.match("^else$", line) :
      print "} else {"
      continue
    if re.match("^endif$", line) :
      print "}"
      continue
    
    match = re.match("(\w+)_SOURCES (\+?)= (.*)", line) 
    if match and match.group(1) in ["SWIFTEN", "ZLIB", "LIBIDN", "BOOST", "EXPAT"] :
      inSources = processSourcesLine(match.group(3))
      continue

    match = re.match("(LIBS|CXXFLAGS|CPPFLAGS|CFLAGS) \+= (.*)", line) 
    if match : 
      processFlags(match.group(1), match.group(2))

    if line.startswith("## ") :
      print line

"""
#print sourceFiles
sys.exit(0)

print files
pro = open ('swiftall.pri', 'w')
for sourceType in files.keys():
    pro.write("%s += \\\n" % sourceType)
    for sourceFile in files[sourceType]:
        pro.write("$$PWD/Swift/%s \\\n" % sourceFile)
    pro.write("\n")
pro.close()

"""
