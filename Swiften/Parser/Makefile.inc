SWIFTEN_SOURCES += \
	Swiften/Parser/XMLParser.cpp \
	Swiften/Parser/XMLParserClient.cpp \
	Swiften/Parser/XMLParserFactory.cpp \
	Swiften/Parser/PlatformXMLParserFactory.cpp \
	Swiften/Parser/XMPPParser.cpp \
	Swiften/Parser/XMPPParserClient.cpp \
	Swiften/Parser/MessageParser.cpp \
	Swiften/Parser/IQParser.cpp \
	Swiften/Parser/PresenceParser.cpp \
	Swiften/Parser/StreamFeaturesParser.cpp \
	Swiften/Parser/CompressParser.cpp \
	Swiften/Parser/AuthRequestParser.cpp \
	Swiften/Parser/StanzaParser.cpp \
	Swiften/Parser/ElementParser.cpp \
	Swiften/Parser/PayloadParser.cpp \
	Swiften/Parser/PayloadParserFactory.cpp \
	Swiften/Parser/PayloadParserFactoryCollection.cpp \
	Swiften/Parser/SerializingParser.cpp

ifeq ($(HAVE_LIBXML),yes)
SWIFTEN_SOURCES += \
	Swiften/Parser/LibXMLParser.cpp
endif

ifeq ($(HAVE_EXPAT),yes)
SWIFTEN_SOURCES += \
	Swiften/Parser/ExpatParser.cpp
endif

include Swiften/Parser/PayloadParsers/Makefile.inc
include Swiften/Parser/UnitTest/Makefile.inc
