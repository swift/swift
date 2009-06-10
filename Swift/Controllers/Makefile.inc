SWIFT_CONTROLLERS_SOURCES += \
	Swift/Controllers/ChatController.cpp \
	Swift/Controllers/ChatControllerBase.cpp \
	Swift/Controllers/MainController.cpp \
	Swift/Controllers/NickResolver.cpp \
	Swift/Controllers/RosterController.cpp \
	Swift/Controllers/XMPPRosterController.cpp \
	Swift/Controllers/MUCController.cpp \
	Swift/Controllers/EventController.cpp

include Swift/Controllers/UnitTest/Makefile.inc

SWIFT_CONTROLLERS_TARGET = Swift/Controllers/Controllers.a
SWIFT_CONTROLLERS_OBJECTS = \
	$(SWIFT_CONTROLLERS_SOURCES:.cpp=.o)

TARGETS += $(SWIFT_CONTROLLERS_TARGET)
UNITTEST_LIBS += $(SWIFT_CONTROLLERS_TARGET)
CLEANFILES += $(SWIFT_CONTROLLERS_TARGET) $(SWIFT_CONTROLLERS_OBJECTS)

$(SWIFT_CONTROLLERS_TARGET): $(SWIFT_CONTROLLERS_OBJECTS)
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $(SWIFT_CONTROLLERS_OBJECTS)
