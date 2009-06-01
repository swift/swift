SWIFTEN_SOURCES += \
	Swiften/Network/HostAddress.cpp \
	Swiften/Network/DomainNameResolver.cpp \
	Swiften/Network/ConnectionFactory.cpp \
	Swiften/Network/BoostConnection.cpp \
	Swiften/Network/BoostConnectionFactory.cpp \
	Swiften/Network/Timer.cpp

include Swiften/Network/UnitTest/Makefile.inc

ifneq ($(WIN32),1)
LIBS += -lresolv
endif
