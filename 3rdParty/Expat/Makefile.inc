EXPAT_TARGET = 3rdParty/Expat/Expat.a

EXPAT_SOURCES = \
  3rdParty/Expat/src/xmltok.c \
  3rdParty/Expat/src/xmlparse.c \
  3rdParty/Expat/src/xmlrole.c

EXPAT_OBJECTS = \
	$(EXPAT_SOURCES:.c=.o)

$(EXPAT_TARGET): $(EXPAT_OBJECTS)
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $(EXPAT_OBJECTS)

ifeq ($(USE_BUNDLED_EXPAT),yes)
CPPFLAGS += -isystem 3rdParty/Expat -isystem 3rdParty/Expat/src -DHAVE_EXPAT_CONFIG_H -DXML_STATIC
BUNDLED_LIBS += $(EXPAT_TARGET)
endif
