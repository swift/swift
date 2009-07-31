SWIFTEN_SOURCES += \
	Swiften/LinkLocal/LinkLocalServiceBrowser.cpp \
	Swiften/LinkLocal/LinkLocalService.cpp \
	Swiften/LinkLocal/LinkLocalServiceInfo.cpp \
	Swiften/LinkLocal/IncomingLinkLocalSession.cpp \
	Swiften/LinkLocal/OutgoingLinkLocalSession.cpp \
	Swiften/LinkLocal/LinkLocalConnector.cpp

include Swiften/LinkLocal/DNSSD/Makefile.inc
include Swiften/LinkLocal/UnitTest/Makefile.inc
