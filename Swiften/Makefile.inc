include Swiften/Base/Makefile.inc
include Swiften/Application/Makefile.inc
include Swiften/EventLoop/Makefile.inc
include Swiften/StringCodecs/Makefile.inc
include Swiften/JID/Makefile.inc
include Swiften/Elements/Makefile.inc
include Swiften/Events/Makefile.inc
include Swiften/StreamStack/Makefile.inc
include Swiften/Serializer/Makefile.inc
include Swiften/Parser/Makefile.inc
include Swiften/MUC/Makefile.inc
include Swiften/Network/Makefile.inc
include Swiften/Client/Makefile.inc
include Swiften/TLS/Makefile.inc
include Swiften/SASL/Makefile.inc
include Swiften/Compress/Makefile.inc
include Swiften/Queries/Makefile.inc
include Swiften/Roster/Makefile.inc
include Swiften/Disco/Makefile.inc
include Swiften/Presence/Makefile.inc
include Swiften/Notifier/Makefile.inc
include Swiften/History/Makefile.inc

CPPFLAGS += $(SQLITE_CPPFLAGS)

SWIFTEN_TARGET = Swiften/Swiften.a
SWIFTEN_OBJECTS = \
	$(SWIFTEN_SOURCES:.cpp=.o) \
	$(SWIFTEN_OBJECTIVE_SOURCES:.mm=.o) \
	$(LIBIDN_OBJECTS) \
	$(BOOST_OBJECTS) \
	$(ZLIB_OBJECTS) \
	$(SQLITE_OBJECTS)
SWIFTEN_HEADERS += \
	Swiften/config.h

TARGETS += $(SWIFTEN_TARGET)
INSTALL_TARGETS += install-swiften
UNITTEST_LIBS += $(SWIFTEN_TARGET)
CLEANFILES += $(SWIFTEN_TARGET) $(SWIFTEN_OBJECTS)

.PHONY: lib
lib: $(SWIFTEN_TARGET)

$(SWIFTEN_TARGET): $(SWIFTEN_OBJECTS)
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $(SWIFTEN_OBJECTS)

.PHONY: install-swiften
install-swiften: $(SWIFTEN_TARGET) $(SWIFTEN_HEADERS)
	install -d $(includedir)/Swiften/3rdParty
	rsync -a 3rdParty/Boost/boost $(includedir)/Swiften/3rdParty/Boost
	for header in $(SWIFTEN_HEADERS); do headerdir=$(includedir)/`dirname $$header`; echo $$header install -d $$headerdir; install $$header $$headerdir; done
	install $(SWIFTEN_TARGET) $(libdir)

include Swiften/QA/Makefile.inc
include Swiften/Examples/Makefile.inc
