SWIFTEN_SOURCES += \
	Swiften/LinkLocal/DNSSDService.cpp \
	Swiften/LinkLocal/LinkLocalRoster.cpp \
	Swiften/LinkLocal/LinkLocalServiceInfo.cpp \
	Swiften/LinkLocal/IncomingLinkLocalSession.cpp \
	Swiften/LinkLocal/OutgoingLinkLocalSession.cpp \
	Swiften/LinkLocal/LinkLocalConnector.cpp

ifeq ($(MACOSX),1)
SWIFTEN_SOURCES += \
	Swiften/LinkLocal/AppleDNSSDService.cpp
endif

include Swiften/LinkLocal/UnitTest/Makefile.inc
