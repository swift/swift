SWIFTEN_SOURCES += \
	Swiften/LinkLocal/DNSSDQuerier.cpp \
	Swiften/LinkLocal/DNSSDBrowseQuery.cpp \
	Swiften/LinkLocal/DNSSDRegisterQuery.cpp \
	Swiften/LinkLocal/DNSSDResolveServiceQuery.cpp \
	Swiften/LinkLocal/DNSSDResolveHostnameQuery.cpp \
	Swiften/LinkLocal/DNSSDServiceFactory.cpp \
	Swiften/LinkLocal/PlatformDNSSDServiceFactory.cpp \
	Swiften/LinkLocal/DNSSDService.cpp \
	Swiften/LinkLocal/LinkLocalRoster.cpp \
	Swiften/LinkLocal/LinkLocalServiceID.cpp \
	Swiften/LinkLocal/LinkLocalServiceInfo.cpp \
	Swiften/LinkLocal/LinkLocalServiceBrowser.cpp \
	Swiften/LinkLocal/IncomingLinkLocalSession.cpp \
	Swiften/LinkLocal/OutgoingLinkLocalSession.cpp \
	Swiften/LinkLocal/LinkLocalConnector.cpp

ifeq ($(HAVE_BONJOUR),yes)
SWIFTEN_SOURCES += \
	Swiften/LinkLocal/BonjourQuery.cpp \
	Swiften/LinkLocal/BonjourQuerier.cpp \
	Swiften/LinkLocal/AppleDNSSDService.cpp
endif
ifeq ($(HAVE_AVAHI),yes)
SWIFTEN_SOURCES += \
	Swiften/LinkLocal/AvahiDNSSDService.cpp
endif

include Swiften/LinkLocal/UnitTest/Makefile.inc
