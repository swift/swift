#!/bin/sh

cppcheck $@  \
	--enable=all \
	-i 3rdParty -i .git -i .sconf_temp \
	-i 3rdParty/hippomocks.h \
	-i Swiftob/linit.cpp \
	-i Swift/QtUI/EventViewer/main.cpp \
	-i Swift/QtUI/ApplicationTest \
	-i Swift/QtUI/ChatView/main.cpp \
	-i Swift/QtUI/Roster/main.cpp \
	-i Swift/QtUI/NotifierTest/NotifierTest.cpp \
	\
	-I . \
	-I Swift/QtUI \
	. 2> cppcheck.tmp
cat cppcheck.tmp | grep -v "(style)" | grep -v "\[3rdParty/hippomocks.h" > cppcheck.log
rm cppcheck.tmp

echo "Result (cppcheck.log):"
cat cppcheck.log
