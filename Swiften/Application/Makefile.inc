SWIFTEN_SOURCES += \
	Swiften/Application/Application.cpp \
	Swiften/Application/ApplicationMessageDisplay.cpp

ifeq ($(MACOSX),1)
include Swiften/Application/MacOSX/Makefile.inc
endif

include Swiften/Application/UnitTest/Makefile.inc
