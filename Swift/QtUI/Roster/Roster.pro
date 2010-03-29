include(Roster.pri)
SOURCES += main.cpp

DEPENDPATH += ../. ../../.. ../../../3rdParty/Boost/src
INCLUDEPATH += ../. ../../.. ../../../3rdParty/Boost/src

LIBS += ../../../Swiften/libSwiften.a 
LIBS += ../../../3rdParty/Boost/libBoost.a
LIBS += ../../../3rdParty/LibIDN/libIDN.a

mac {
	DEFINES += SWIFT_PLATFORM_MACOSX
}

CONFIG -= app_bundle

DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals BOOST_ALL_NO_LIB

exists(../config.pri) {
	LIBS += ../../Controllers/Controllers.a ../../../Swiften/Swiften.a
	include(../config.pri)
}

mac {
	DEFINES += SWIFT_PLATFORM_MACOSX
}

RESOURCES += ../Swift.qrc
