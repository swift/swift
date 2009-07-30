SWIFTEN_SOURCES += \
	Swiften/LinkLocal/LinkLocalServiceBrowser.cpp \
	Swiften/LinkLocal/DNSSDService.cpp \
	Swiften/LinkLocal/LinkLocalService.cpp \
	Swiften/LinkLocal/LinkLocalRoster.cpp \
	Swiften/LinkLocal/LinkLocalServiceInfo.cpp \
	Swiften/LinkLocal/IncomingLinkLocalSession.cpp \
	Swiften/LinkLocal/OutgoingLinkLocalSession.cpp \
	Swiften/LinkLocal/LinkLocalConnector.cpp

ifeq ($(HAVE_BONJOUR),yes)
SWIFTEN_SOURCES += \
	Swiften/LinkLocal/AppleDNSSDService.cpp
endif
ifeq ($(HAVE_AVAHI),yes)
SWIFTEN_SOURCES += \
	Swiften/LinkLocal/AvahiDNSSDService.cpp
endif

include Swiften/LinkLocal/DNSSD/Makefile.inc
include Swiften/LinkLocal/UnitTest/Makefile.inc
