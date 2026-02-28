#!/bin/sh

cppcheck $@  \
    --enable=all \
    --inline-suppr \
    --suppress=postfixOperator:3rdParty/hippomocks.h \
    --suppress=stlSize:3rdParty/hippomocks.h \
    --suppress=noConstructor \
    --suppress=publicAllocationError:Swift/Controllers/Chat/UnitTest/ChatsManagerTest.cpp \
    -i 3rdParty -i .git -i .sconf_temp \
    -i Swift/QtUI/EventViewer/main.cpp \
    -i Swift/QtUI/ApplicationTest \
    -i Swift/QtUI/ChatView/main.cpp \
    -i Swift/QtUI/Roster/main.cpp \
    -i Swift/QtUI/NotifierTest/NotifierTest.cpp \
    -DSWIFTEN_BUILDING -DSWIFTEN_STATIC \
    -U__BEOS__ -U__CYGWIN__ -U__QNNXTO__ -U__amigaos__ -Uhpux -U__sgi \
    \
    -I . \
    -I Swift/QtUI \
    .
