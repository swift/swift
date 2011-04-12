#!/usr/bin/env python

import os, sys, re, xml.dom.minidom

def getText(nodelist):
  text = ""
  for node in nodelist:
    if node.nodeType == node.TEXT_NODE:
      text += node.data
  return text


desktop_generic_names = set()
desktop_comments = set()
f = open("Swift/resources/swift.desktop", "r")
for l in f.readlines() :
  m = re.match("GenericName\[(\w+)\].*", l)
  if m :
    desktop_generic_names.add(m.group(1))
  m = re.match("Comment\[(\w+)\].*", l)
  if m :
    desktop_comments.add(m.group(1))
f.close()

for filename in os.listdir("Swift/Translations") :
  m = re.match("swift_(.*)\.ts", filename)
  if m :
    language = m.group(1)
    finished = True
    f = open("Swift/Translations/" + filename, "r")
    document = xml.dom.minidom.parse(f)
    f.close()

    for message in document.getElementsByTagName("message") :
      source = message.getElementsByTagName("source")[0]
      sourceText = getText(source.childNodes)
      sourcePlaceholders = set(re.findall("%\d+%?", sourceText))
      translation = message.getElementsByTagName("translation")[0]
      if "type" in translation.attributes.keys() and translation.attributes["type"]. value == "unfinished" :
        finished = False
      translationText = getText(translation.childNodes)
      translationPlaceholders = set(re.findall("%\d+%?", translationText))
      if translationPlaceholders != sourcePlaceholders :
        print "[Error] " + filename + ": Placeholder mismatch in translation '" + sourceText + "'"
    if not finished :
        print "[Warning] " + filename + ": Unfinished"
    if language not in desktop_generic_names :
        print "[Warning] GenericName field missing in desktop entry for " + language
    if language not in desktop_comments :
        print "[Warning] Comment field missing in desktop entry for " + language
