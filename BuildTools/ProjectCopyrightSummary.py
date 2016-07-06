#!/usr/bin/env python

import fnmatch
import os
import re
from sets import Set

projectSourceFiles = []
for directory in ['Documentation', 'Limber', 'Packages', 'QA', 'Slimber', 'Sluift', 'Swift', 'Swiften', 'SwifTools']:
    for root, dirnames, filenames in os.walk(directory):
        for filename in filenames:
            if any(fnmatch.fnmatch(filename, pattern) for pattern in ['*.cpp', '*.h', '*.mm', '*.m', '*.c']):
                projectSourceFiles.append(os.path.join(root, filename))


def CopyrightNames(filename):
    names = []
    with open(filename, 'r') as file:
        data = file.read()
        p = re.compile(ur'\* Copyright.*\d\d\d\d (.*?)\.?$', re.MULTILINE)
        names = re.findall(p, data)
    return names


names = Set()
for file in projectSourceFiles:
    copyrightNames = CopyrightNames(file)
    for name in copyrightNames:
        names.add(name)

for name in sorted(list(names)):
    print(name)
