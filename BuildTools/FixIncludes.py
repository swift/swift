#!/usr/bin/env python

import sys;
import os;
import re;
from sets import Set

filename = sys.argv[1]

inPlace = False
if "-i" in sys.argv:
	inPlace = True

filename_base = os.path.basename(filename)
(filename_name, filename_ext) = os.path.splitext(filename_base)

c_stdlib_headers = Set(["assert.h",  "limits.h",  "signal.h",  "stdlib.h", "ctype.h", "locale.h",  "stdarg.h", "string.h", "errno.h", "math.h", "stddef.h", "time.h", "float.h",  "setjmp.h", "stdio.h", "iso646.h", "wchar.h", "wctype.h", "complex.h", "inttypes.h", "stdint.h", "tgmath.h", "fenv.h", "stdbool.h"])

cpp_stdlib_headers = Set(["algorithm", "fstream", "list", "regex", "typeindex", "array", "functional", "locale", "set", "typeinfo", "atomic", "future", "map", "sstream", "type_traits", "bitset", "initializer_list", "memory", "stack", "unordered_map", "chrono", "iomanip", "mutex", "stdexcept", "unordered_set", "codecvt", "ios", "new", "streambuf", "utility", "complex", "iosfwd", "numeric", "string", "valarray", "condition_variable", "iostream", "ostream", "strstream", "vector", "deque", "istream", "queue", "system_error", "exception", "iterator", "random", "thread", "forward_list", "limits", "ratio", "tuple", "cassert", "ciso646", "csetjmp", "cstdio", "ctime", "cctype", "climits", "csignal", "cstdlib", "cwchar", "cerrno", "clocale", "cstdarg", "cstring", "cwctype", "cfloat", "cmath", "cstddef"])

class HeaderType:
		PRAGMA_ONCE, CORRESPONDING_HEADER, C_STDLIB, CPP_STDLIB, BOOST, QT, OTHER, SWIFTEN, SWIFT_CONTROLLERS, SWIFTOOLS, SWIFT = range(11)

def findHeaderBlock(lines):
	start = False
	end = False
	lastLine = None

	for idx, line in enumerate(lines):
		if not start and line.startswith("#"):
			start = idx
		elif start and (not end) and (not line.startswith("#")) and line.strip():
			end = idx-1
			break
	if not end:
		end = len(lines)
	return (start, end)

def lineToFileName(line):
	match = re.match( r'#include "(.*)"', line)
	if match:
		return match.group(1)
	match = re.match( r'#include <(.*)>', line)
	if match:
		return match.group(1)
	return False

def fileNameToHeaderType(name):
	if name.endswith(filename_name + ".h"):
		return HeaderType.CORRESPONDING_HEADER

	if name in c_stdlib_headers:
		return HeaderType.C_STDLIB

	if name in cpp_stdlib_headers:
		return HeaderType.CPP_STDLIB

	if name.startswith("boost"):
		return HeaderType.BOOST

	if name.startswith("Q"):
		return HeaderType.QT

	if name.startswith("Swiften"):
		return HeaderType.SWIFTEN

	if name.startswith("Swift/Controllers"):
		return HeaderType.SWIFT_CONTROLLERS

	if name.startswith("SwifTools"):
		return HeaderType.SWIFTOOLS

	if name.startswith("Swift"):
		return HeaderType.SWIFT

	return HeaderType.OTHER

def serializeHeaderGroups(groups):
	headerList = []
	for group in range(0, HeaderType.SWIFT + 1):
		if group in groups:
			# sorted and without duplicates
			headers = sorted(list(set(groups[group])))
			headerList.extend(headers)
			headerList.extend(["\n"])
	headerList.pop()
	return headerList

def overwriteFile(filename, content):
	with open(filename, 'w') as f:
		for line in content:
			f.write(line)

def cleanHeaderFile(content, headerStart, headerEnd, headerGroups):
	del content[headerStart:headerEnd]
	newHeaders = serializeHeaderGroups(headerGroups)
	content[headerStart:1] = newHeaders

	if inPlace :
		overwriteFile(filename, content)
	else :
		for line in content:
			print line,

def cleanImplementationFile(content, headerStart, headerEnd, headerGroups):
	del content[headerStart:headerEnd]
	newHeaders = serializeHeaderGroups(headerGroups)
	content[headerStart:1] = newHeaders

	if inPlace :
		overwriteFile(filename, content)
	else :
		for line in content:
			print line,


containsComplexPreprocessorDirectives = False

with open(filename) as f:
	content = f.readlines()

(headerStart, headerEnd) = findHeaderBlock(content)

headerGroups = {}

for line in content[headerStart:headerEnd]:
	if line.strip():
		if line.strip().startswith("#pragma once"):
			headerType = HeaderType.PRAGMA_ONCE
		elif line.strip().startswith("#if") or line.strip().startswith("#def") or line.strip().startswith("#undef") or line.strip().startswith("#pragma "):
			containsComplexPreprocessorDirectives = True
			break
		else:
			#print line
			headerType = fileNameToHeaderType(lineToFileName(line))

		#filename = lineToFileName(line)
		if headerType in headerGroups:
			headerGroups[headerType].append(line)
		else:
			headerGroups[headerType] = [line]

if containsComplexPreprocessorDirectives:
	print "Cannot format headers containing preprocessor #if, #pragma, #define or #undef statements!"
	exit(1)

if filename_base.endswith(".h"):
	if not HeaderType.PRAGMA_ONCE in headerGroups:
		print "Missing #pragma once!"
		exit(2)
	cleanHeaderFile(content, headerStart, headerEnd, headerGroups)
elif filename_base.endswith(".cpp"):
	cleanImplementationFile(content, headerStart, headerEnd, headerGroups)
