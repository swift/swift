SWIFTEN_SOURCES += \
	Swiften/LinkLocal/DNSSDService.cpp \
	Swiften/LinkLocal/LinkLocalRoster.cpp


ifeq ($(MACOSX),1)
SWIFTEN_SOURCES += \
	Swiften/LinkLocal/AppleDNSSDService.cpp
endif
