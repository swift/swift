TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

INCLUDEPATH += . ../../../Swiften/3rdParty/Boost
INCLUDEPATH += . ../../..
SOURCES += NotifierTest.cpp
LIBS += ../../../Swiften/Swift.a -framework Growl
