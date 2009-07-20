SWIFTEN_SOURCES += \
  Swiften/EventLoop/EventOwner.cpp \
  Swiften/EventLoop/EventLoop.cpp \
  Swiften/EventLoop/SimpleEventLoop.cpp \
  Swiften/EventLoop/MainEventLoop.cpp

ifeq ($(MACOSX),1)
include Swiften/EventLoop/Cocoa/Makefile.inc
endif
include Swiften/EventLoop/UnitTest/Makefile.inc
