#!/usr/bin/env python
# Note
# ----
# This script requires:
#   - cdb, the Windows command line debugger installed and available in PATH.
#   - the SWIFT_DIST environment variable set to a location that contains msi and pdb.gz files.

import sys
from subprocess import call
from subprocess import Popen, PIPE
import ntpath
import shutil
import re
import urllib2
import os
import gzip
import time


swiftWindowBuildsPathPrefix = os.getenv("SWIFT_DIST")
if swiftWindowBuildsPathPrefix == None :
    print("Please set the SWIFT_DIST environment variable to a location containing msi and pdb.gz files.")
    sys.exit(1)

if len(sys.argv) != 3:
    print("Usage: python WindowsMinidumpAnalyse.py VERSION MINIDUMP_FILE")
    sys.exit(1)

version = sys.argv[1]
minidump_file = sys.argv[2]
minidump_filename = ntpath.basename(minidump_file)
minidump_fullpath = os.path.abspath(minidump_file)
humantext_fullpath = os.path.splitext(minidump_fullpath)[0]+".txt"
symbol_cache_path = os.path.join(os.getenv("TEMP"), "\symbols")
working_folder = "tmp-crash-{0}".format(minidump_filename)
commit = ""

def downloadInstaller(version) :
    onlineFilename = "{0}.msi".format(version.capitalize())
    url = "{0}{1}".format(swiftWindowBuildsPathPrefix, onlineFilename)
    print("Download {0}.".format(url))
    file = urllib2.urlopen(url)
    with open(onlineFilename,'wb') as output:
        output.write(file.read())

def unpackInstaller(version) :
    msiFilename = "{0}.msi".format(version.capitalize())
    msiExtractDirectory = os.getcwd() + "\\msi"
    if not os.path.exists(msiExtractDirectory):
        os.makedirs(msiExtractDirectory)
    print("Unpack {0} to {1}.".format(msiFilename, os.getcwd()))
    call(["msiexec", "/a", msiFilename, "/qb", "TARGETDIR={0}".format(msiExtractDirectory)], shell=True)

def unpackDebugSymbols(version) :
    symbolsFilename = "{0}.pdb.gz".format(version.capitalize())
    print("Unpack {0}.".format(symbolsFilename))
    if not os.path.isdir(symbolsFilename):
        with gzip.open(symbolsFilename, 'rb') as in_file:
            s = in_file.read()

        path_to_store = symbolsFilename[:-3]

        with open("msi\PFiles\Swift\{0}".format("Swift.pdb"), 'wb') as f:
            f.write(s)

def downloadDebugSymbols(version) :
    onlineFilename = "{0}.pdb.gz".format(version.capitalize())
    url = "{0}{1}".format(swiftWindowBuildsPathPrefix, onlineFilename)
    print("Download {0}.".format(url))
    file = urllib2.urlopen(url)
    with open(onlineFilename,'wb') as output:
        output.write(file.read())

def copyMinidump(filename) :
    shutil.copyfile(filename, "msi\PFiles\Swift\{0}".format(minidump_filename))

def printHumanReadableReport():
    oldDir = os.getcwd()

    # change dir to Swift.exe dir
    os.chdir("msi\PFiles\Swift")

    # print all stacks and analyze crash for exceptions
    cdbCommand = ".symopt+0x40;.lines -e;.kframes 200;!analyze -v -p;!uniqstack -vp;.ecxr;k;q"

    symbolPath = "cache*{0};srv*https://msdl.microsoft.com/download/symbols;C:\\Qt\\Qt5.4.2\\5.4\\msvc2013_opengl\\bin;C:\\Qt\\Qt5.4.2\\5.4\\msvc2013_opengl\\lib;{1}".format(symbol_cache_path, os.getcwd())

    cdbFullCommand = ["cdb", "-i", os.getcwd(), "-y", symbolPath, "-z", minidump_filename, "-srcpath", oldDir, "-logo", humantext_fullpath, "-c", cdbCommand ]
    print("Run command: " + str(cdbFullCommand))
    call(cdbFullCommand)

# for testing, delete the old folder
try:
    shutil.rmtree(working_folder)
except:
    print("")

# clone local git repository into dedicated directory
call(["git", "clone",  ".", working_folder], shell=True)

# git version from swift version
match = re.match( r"(.*)-dev(\d+)", version)
if match:
    basetag = match.group(1)
    commits = int(match.group(2))
    process = Popen(["git", "-C", working_folder, "log", "--ancestry-path", "--format=%H", "{0}..HEAD".format(basetag)], stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()
    commit = output.splitlines()[-commits].strip()
else:
    basetag = version
    process = Popen(["git", "-C", working_folder, "log", "--format=%H", "-n", "1" "{0}".format(basetag)], stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()
    commit = output.strip()

assert(len(commit) > 0)

# Create symbol cache directory
if not os.path.exists(symbol_cache_path):
    os.makedirs(symbol_cache_path)

#print("Checking out commit {0}.".format(commit))
call(["git", "-C", working_folder, "checkout", commit])

os.chdir(working_folder)

downloadInstaller(version)
downloadDebugSymbols(version)
unpackInstaller(version)
unpackDebugSymbols(version)
copyMinidump(minidump_fullpath)
time.sleep(10)
printHumanReadableReport()
