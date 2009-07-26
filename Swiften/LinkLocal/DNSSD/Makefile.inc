SWIFTEN_SOURCES += \
	Swiften/LinkLocal/DNSSD/DNSSDServiceID.cpp \
	Swiften/LinkLocal/DNSSD/DNSSDQuerier.cpp \
	Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.cpp \
	Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.cpp \
	Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.cpp \
	Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.cpp

ifeq ($(HAVE_BONJOUR),yes)
include Swiften/LinkLocal/DNSSD/Bonjour/Makefile.inc
endif
ifeq ($(HAVE_AVAHI),yes)
endif

include Swiften/LinkLocal/DNSSD/Fake/Makefile.inc
