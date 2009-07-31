CXXFLAGS += -isystem 3rdParty/LibIDN/src
CFLAGS += -isystem 3rdParty/LibIDN/src -isystem 3rdParty/LibIDN/stubs
ifeq ($(WIN32),1)
CPPFLAGS += -DIDNA_STATIC
CFLAGS += -isystem 3rdParty/LibIDN/stubs/win32
CFLAGS += -Dstrcasecmp=stricmp -Dstrncasecmp=strnicmp
CXXFLAGS += -isystem 3rdParty/LibIDN/stubs/win32
endif

LIBIDN_SOURCES += \
	3rdParty/LibIDN/src/stringprep.c \
	3rdParty/LibIDN/src/profiles.c \
	3rdParty/LibIDN/src/rfc3454.c \
	3rdParty/LibIDN/src/punycode.c \
	3rdParty/LibIDN/src/idna.c \
	3rdParty/LibIDN/src/toutf8.c \
	3rdParty/LibIDN/src/nfkc.c

LIBIDN_OBJECTS = \
	$(LIBIDN_SOURCES:.c=.o)
