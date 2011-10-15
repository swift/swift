#!/usr/bin/env python
#coding=utf-8

import os, re, datetime, sys, subprocess

DEFAULT_LICENSE = "gpl3"
CONTRIBUTOR_LICENSE = "mit"
LICENSE_DIR = "Documentation/Licenses"

class License :
  def __init__(self, name, file) :
    self.name = name
    self.file = file

licenses = {
    "gpl3" : License("GNU General Public License v3", "GPLv3.txt"),
    "mit" : License("MIT License", "MIT.txt"),
  }


class Copyright :
  def __init__(self, author, year, license) :
    self.author = author
    self.year = year
    self.license = license

  def to_string(self, comment_chars) :
    return "\n".join([
      comment_chars[0],
      comment_chars[1] + " Copyright (c) %(year)s %(name)s" % {"year" : self.year, "name" : self.author },
      comment_chars[1] + " Licensed under the " + licenses[self.license].name + ".",
      comment_chars[1] + " See " + LICENSE_DIR + "/" + licenses[self.license].file + " for more information.",
      comment_chars[2],
      "\n"])

def get_comment_chars_for_filename(filename) :
  return ("/*", " *", " */")

def get_comment_chars_re_for_filename(filename) :
  comment_chars = get_comment_chars_for_filename(filename)
  return "|".join(comment_chars).replace("*", "\\*")

def parse_file(filename) :
  file = open(filename)
  copyright_text = []
  prolog = ""
  epilog = ""
  inProlog = True
  inCopyright = False
  inEpilog = False
  for line in file.readlines() :
    if inProlog :
      if line.startswith("#!") or len(line.strip()) == 0 :
        prolog += line
        continue
      else :
        inProlog = False
        inCopyright = True

    if inCopyright :
      if re.match(get_comment_chars_re_for_filename(filename), line) != None :
        copyright_text.append(line.rstrip())
        continue
      else :
        inCopyright = False
        inEpilog = True
        if len(line.strip()) == 0 :
          continue

    if inEpilog :
      epilog += line
      continue

  file.close()

  # Parse the copyright
  copyright = None
  if len(copyright_text) == 5 :
    comment_chars = get_comment_chars_for_filename(filename)
    if copyright_text[0] == comment_chars[0] and copyright_text[4] == comment_chars[2] :
      matchstring = "(" + get_comment_chars_re_for_filename(filename) + ") Copyright \(c\) (?P<startYear>\d\d\d\d)(-(?P<endYear>\d\d\d\d))? (?P<author>.*)"
      m = re.match(matchstring, copyright_text[1])
      if m != None :
        # FIXME: Do better copyright reconstruction here
        copyright = True
  if not copyright :
    epilog = "\n".join(copyright_text) + epilog
  return (prolog, copyright, epilog)

def get_userinfo() :
  p = subprocess.Popen("git config user.name", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
  username = p.stdout.read().rstrip()
  p.stdin.close()
  if p.wait() != 0 :
    return None
  p = subprocess.Popen("git config user.email", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
  email = p.stdout.read().rstrip()
  p.stdin.close()
  if p.wait() != 0 :
    return None
  return (username, email)

def get_copyright(username, email) :
  if email in ["git@el-tramo.be", "git@kismith.co.uk"] :
    license = DEFAULT_LICENSE
  else :
    license = CONTRIBUTOR_LICENSE
  return Copyright(username, datetime.date.today().strftime("%Y"), license)

def check_copyright(filename) :
  (prolog, copyright, epilog) = parse_file(filename)
  if copyright == None :
    print "No copyright found in: " + filename
    #print "Please run '" + sys.argv[0] + " set-copyright " + filename + "'"
    return False
  else :
    return True

def set_copyright(filename, copyright) :
  (prolog, c, epilog) = parse_file(filename)
  comment_chars = get_comment_chars_for_filename(filename)
  copyright_text = copyright.to_string(comment_chars)
  file = open(filename, "w")
  if prolog != "":
    file.write(prolog)
  file.write(copyright_text)
  if epilog != "" :
    file.write(epilog)
  file.close()

if sys.argv[1] == "check-copyright" :
  if not check_copyright(sys.argv[2]) :
    sys.exit(-1)
elif sys.argv[1] == "check-all-copyrights" :
  ok = True
  for (path, dirs, files) in os.walk(".") :
    if "3rdParty" in path or ".sconf" in path or "Swift.app" in path or path.startswith("build") or "xmppbench" in path :
      continue
    for filename in [os.path.join(path, file) for file in files if (file.endswith(".cpp") or file.endswith(".h")) and not "ui_" in file and not "moc_" in file and not "qrc_" in file and not "BuildVersion.h" in file and not "Swiften.h" in file and not "Version.h" in file and not "swiften-config.h" in file and not "linit.cpp" in file ] :
      ok &= check_copyright(filename) 
  if not ok :
    sys.exit(-1)
elif sys.argv[1] == "set-copyright" :
  (username, email) = get_userinfo()
  copyright = get_copyright(username, email)
  set_copyright(sys.argv[2], copyright)
elif sys.argv[1] == "set-all-copyrights" :
  (username, email) = get_userinfo()
  copyright = get_copyright(username, email)
  for (path, dirs, files) in os.walk(".") :
    if "3rdParty" in path or ".sconf" in path or "Swift.app" in path :
      continue
    for filename in [os.path.join(path, file) for file in files if (file.endswith(".cpp") or file.endswith(".h")) and not "ui_" in file and not "moc_" in file and not "qrc_" in file and not "BuildVersion.h" in file and not "swiften-config.h" in file] :
      set_copyright(filename, copyright) 
else :
  print "Unknown command: " + sys.argv[1]
  sys.exit(-1)
