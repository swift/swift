SWIFTEN_SOURCES += \
	Swiften/Notifier/Notifier.cpp

ifeq ($(HAVE_GROWL),yes)
	SWIFTEN_SOURCES += \
		Swiften/Notifier/GrowlNotifier.cpp
endif
