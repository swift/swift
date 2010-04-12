CONFIG -= app_bundle

include(EventView.pri)
SOURCES += main.cpp

DEPENDPATH += ../. ../../.. ../../../3rdParty/Boost/src
INCLUDEPATH += ../. ../../.. ../../../3rdParty/Boost/src

DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals BOOST_ALL_NO_LIB

LIBS += ../../Controllers/libSwiftControllers.a 
LIBS += ../../../Swiften/libSwiften.a 
LIBS += ../../../3rdParty/Boost/libBoost.a
LIBS += ../../../3rdParty/LibIDN/libIDN.a

mac {
	DEFINES += SWIFT_PLATFORM_MACOSX
}

RESOURCES += ../Swift.qrc
