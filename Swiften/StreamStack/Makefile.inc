SWIFTEN_SOURCES += \
	Swiften/StreamStack/XMPPLayer.cpp \
	Swiften/StreamStack/StreamStack.cpp \
	Swiften/StreamStack/LowLayer.cpp \
	Swiften/StreamStack/HighLayer.cpp \
	Swiften/StreamStack/WhitespacePingLayer.cpp \
	Swiften/StreamStack/TLSLayerFactory.cpp \
	Swiften/StreamStack/PlatformTLSLayerFactory.cpp

ifeq ($(HAVE_OPENSSL),yes)
SWIFTEN_SOURCES += \
	Swiften/StreamStack/OpenSSLLayer.cpp
endif

include Swiften/StreamStack/UnitTest/Makefile.inc
