TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../../.. ../../../Swiften/3rdParty/Boost
mac {
	ICON = ../../../../resources/MacOSX/Swift.icns
}

HEADERS += \
	../QtSwiftUtil.h
SOURCES += \
	main.cpp
LIBS += \
	../../../Swiften/Swift.a
