TEMPLATE = app
CONFIG += debug
TARGET = slimber

DEPENDPATH += . ../.. ../../3rdParty/Boost
INCLUDEPATH += . ../.. ../../3rdParty/Boost
LIBS += ../Slimber.a ../../Swiften/Swiften.a -lexpat -L. -lavahi-client -lxml2
include(config.pri)

DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals BOOST_ALL_NO_LIB

HEADERS += \
	QtMenulet.h

SOURCES += \
	main.cpp

RESOURCES += \
	Slimber.qrc

#win32 {
#	RC_FILE = ../Resources/Swift.rc
#}

