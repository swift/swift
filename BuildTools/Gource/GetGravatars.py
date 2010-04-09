#!/usr/bin/env python

import subprocess, os, sys, hashlib, urllib

GRAVATAR_URL = "http://www.gravatar.com/avatar/%(id)s?d=404"

if len(sys.argv) != 2 :
  print "Usage: " + sys.argv[0] + " <output-dir>"
  sys.exit(-1)

output_dir = sys.argv[1]

# Retrieve the list of authors
authors = {}
p = subprocess.Popen("git log --pretty=format:'%ae|%an'", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=(os.name != "nt"))
for line in p.stdout.readlines() :
  author_components = line.rstrip().split("|")
  authors[author_components[0]] = author_components[1]
p.stdin.close()
if p.wait() != 0 :
  print "Error"
  sys.exit(-1)

# Get & save the avatars
if not os.path.isdir(output_dir) :
  os.makedirs(output_dir)
for email, name in authors.items() :
  print "Processing avatar for " + name + " <" + email + ">"
  filename = os.path.join(output_dir, name + ".png")
  if os.path.isfile(filename) :
    print "-> Already there. Skipping."
    continue

  m = hashlib.md5()
  m.update(email)
  url = GRAVATAR_URL % {"id" : m.hexdigest()}
  print "- Downloading " + url
  f = urllib.urlopen(url)
  input = None
  if f.getcode() == 200 :
    input = f.read()
  f.close()
  if input :
    print "- Saving file " + filename
    f = open(filename, "w")
    f.write(input)
    f.close()
  else :
    print "- No Gravatar found"
