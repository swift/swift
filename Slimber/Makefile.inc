SLIMBER_TARGET = Slimber/Slimber.a
SLIMBER_SOURCES = \
	Slimber/FileVCardCollection.cpp \
	Slimber/VCardCollection.cpp \
	Slimber/Server.cpp
SLIMBER_OBJECTS = \
	$(SLIMBER_SOURCES:.cpp=.o)

DEPS += \
	$(SLIMBER_SOURCES:.cpp=.dep) \

include Slimber/CLI/Makefile.inc
ifeq ($(MACOSX),1)
include Slimber/Cocoa/Makefile.inc
endif

.PHONY: slimber
slimber: $(SLIMBER_TARGETS)

$(SLIMBER_TARGET): $(SLIMBER_OBJECTS)
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $(SLIMBER_OBJECTS)
