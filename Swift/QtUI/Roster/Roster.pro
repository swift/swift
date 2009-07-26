include(Roster.pri)
SOURCES += main.cpp

DEPENDPATH += ../. ../../.. ../../../3rdParty/Boost
INCLUDEPATH += ../. ../../.. ../../../3rdParty/Boost

DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals BOOST_ALL_NO_LIB

exists(../config.pri) {
	LIBS += ../../Controllers/Controllers.a ../../../Swiften/Swiften.a
	include(../config.pri)
}

mac {
	DEFINES += SWIFT_PLATFORM_MACOSX
}