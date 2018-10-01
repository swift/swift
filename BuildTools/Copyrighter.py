#!/usr/bin/env python
#coding=utf-8

import os, re, datetime, sys, subprocess

DEFAULT_LICENSE = "gpl3"
CONTRIBUTOR_LICENSE = "mit"
LICENSE_DIR = "Documentation/Licenses"

# The following regex parses license comment blocks and its part out of a complete source file.
reParseLicenseCommentBlocks = re.compile(r'(\/\*\n\s\*\sCopyright \(c\) (?P<startYear>\d\d\d\d)(-(?P<endYear>\d\d\d\d))? (?P<author>[^\n\.]*)\.?\n.\* (?P<license>[^\n]*)\n \* (?P<seeMore>[^\n]+)\n *\*\/)')

class License :
    def __init__(self, name, file) :
        self.name = name
        self.file = file

licenses = {
        "default": License("All rights reserved.", "See the COPYING file for more information."),
        "gpl3" : License("Licensed under the GNU General Public License v3.", "See " + LICENSE_DIR + "/" + "GPLv3.txt" + " for more information."),
        "mit" : License("Licensed under the MIT License.", "See " + LICENSE_DIR + "/" +  "MIT.txt" + " for more information."),
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
            comment_chars[1] + licenses[self.license].name,
            comment_chars[1] + licenses[self.license].file,
            comment_chars[2],
            "\n"])
    def __str__(self):
        return """/*
 * Copyright (c) %s %s.
 * %s
 * %s
 */
""" % (self.year, self.author, licenses[self.license].name, licenses[self.license].file)

class ContentRef :
    def __init__(self, begin, end, content):
        self.begin = begin
        self.end = end
        self.content = content

class CopyrightBlock :
    def __init__(self, yearBegin, yearEnd, author, license, seeMore, total):
        self.yearBegin = yearBegin
        self.yearEnd = yearEnd
        self.author = author
        self.license = license
        self.seeMore = seeMore
        self.total = total

def cref_from_group(match, group):
    if match.group(group):
        return ContentRef(match.start(group), match.end(group), match.group(group))
    else :
        return None

def parse_file_new(filename):
    copyrightBlocks = []
    with open(filename, 'r') as file:
        content = file.read()
        for match in re.finditer(reParseLicenseCommentBlocks, content):
            copyrightBlocks.append(CopyrightBlock(
                cref_from_group(match, "startYear"), 
                cref_from_group(match, "endYear"), 
                cref_from_group(match, "author"), 
                cref_from_group(match, "license"), 
                cref_from_group(match, "seeMore"), 
                cref_from_group(match, 0)))
    return copyrightBlocks

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

def get_copyright_setting(username, email) :
    config = os.getenv("SWIFT_LICENSE_CONFIG")
    if config :
        copyrightHolder, license = config.split("|")
    else :
        if email.endswith("isode.com") or email in ["git@el-tramo.be", "git@kismith.co.uk", "tm@ayena.de"] :
            copyrightHolder, license = "Isode Limited", "default"
        else :
            copyrightHolder, license = username, "mit"
    return Copyright(copyrightHolder, datetime.date.today().year, license)

def check_copyright(filename, hints) :
    copyrightBlocks = parse_file_new(filename)
    if copyrightBlocks :
        # looking for copyright block for current author
        username, email = get_userinfo()
        copyrightSetting = get_copyright_setting(username, email)
        for block in copyrightBlocks :
            if block.author.content == copyrightSetting.author:
                year = block.yearBegin.content if not block.yearEnd else block.yearEnd.content
                if int(year) == copyrightSetting.year:
                    return True
                else :
                    if hints :
                        print("Copyright block for " + copyrightSetting.author + " does not cover current year in: " + filename)
                    return False
        if hints :
            print("Missing copyright block for " + copyrightSetting.author + " in: " + filename)
        return False
    else :
        if hints :
            print("No copyright found in: " + filename)
        return False

def replace_data_in_file(filename, begin, end, replaceWith) :
    with open(filename, 'r') as file:
        content = file.read()
    with open(filename, 'w') as file:
        file.write(content[:begin] + replaceWith + content[end:])

def set_or_update_copyright(filename) :
    if check_copyright(filename, False) :
        print("No update required for file: " + filename)
    else :
        copyrightBlocks = parse_file_new(filename)
        username, email = get_userinfo()
        copyrightSetting = get_copyright_setting(username, email)
        lastBlock = 0
        for block in copyrightBlocks :
            if block.author.content == copyrightSetting.author :
                if not block.yearEnd :
                    # replace year with range
                    replace_data_in_file(filename, block.yearBegin.begin, block.yearBegin.end, "%s-%s" % (block.yearBegin.content, str(copyrightSetting.year)))
                else :
                    # replace end of range with current year
                    replace_data_in_file(filename, block.yearEnd.begin, block.yearEnd.end, "%s" % str(copyrightSetting.year))
                return
            lastBlock = block.total.end

        # No copyright block found. Append a new one.
        replace_data_in_file(filename, lastBlock+1, lastBlock+1, "\n" + str(copyrightSetting))

def print_help() :
    print("""Usage:
    Copyrighter.py check-copyright $filename
        Cheks for the existence of a copyright comment block.

    Copyrighter.py set-copyright $filename
        Adds or updates the existing copyright comment block.

    License setting:
        A users license configuration can be set via the SWIFT_LICENSE_CONFIG environment variable 
        in the format "$copyright holder|$license", e.g. "Jane Doe|mit". Possible values for 
        $license are default, mit and gpl.
    """)

if sys.argv[1] == "check-copyright" :
    file = sys.argv[2]
    if (file.endswith(".cpp") or file.endswith(".h")) and not "3rdParty" in file :
        if not check_copyright(file, True) :
            sys.exit(-1)
elif sys.argv[1] == "set-copyright" :
    file = sys.argv[2]
    set_or_update_copyright(file)
else :
    print("Unknown command: " + sys.argv[1])
    print_help()
    sys.exit(-1)
