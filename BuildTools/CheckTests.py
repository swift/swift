#!/usr/bin/env python

import os, sys, re

foundUnregisteredTests = False

for (path, dirs, files) in os.walk(".") :
	if not "3rdParty" in path :
		for filename in [os.path.join(path, file) for file in files if file.endswith("Test.cpp") and file != "IdleQuerierTest.cpp" and file != "NotifierTest.cpp" and file != "ClientTest.cpp" and file != "ConnectivityTest.cpp" and file != "ReconnectTest.cpp"] :
			file = open(filename, "r")
			isRegistered = False
			registeredTests = set()
			declaredTests = set()
			for line in file.readlines() :
				m = re.match("\s*CPPUNIT_TEST_SUITE_REGISTRATION\((.*)\)", line)
				if m :
					isRegistered = True
				m = re.match("\s*CPPUNIT_TEST\((.*)\)", line)
				if m :
					registeredTests.add(m.group(1))
					continue
				m = re.match("\s*void (test.*)\(\)", line)
				if m :
					declaredTests.add(m.group(1))
			if not isRegistered :
				print filename + ": Registration missing"
				foundUnregisteredTests = True
			if registeredTests - declaredTests != set([]) :
				print filename + ": " + str(registeredTests - declaredTests)
				foundUnregisteredTests = True
			file.close()

sys.exit(foundUnregisteredTests)
