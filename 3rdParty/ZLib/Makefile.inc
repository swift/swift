CXXFLAGS += -isystem 3rdParty/ZLib/src
CFLAGS += -isystem 3rdParty/ZLib/src

ZLIB_SOURCES += \
	3rdParty/ZLib/src/adler32.c \
	3rdParty/ZLib/src/compress.c \
	3rdParty/ZLib/src/crc32.c \
	3rdParty/ZLib/src/deflate.c \
	3rdParty/ZLib/src/gzio.c \
	3rdParty/ZLib/src/infback.c \
	3rdParty/ZLib/src/inffast.c \
	3rdParty/ZLib/src/inflate.c \
	3rdParty/ZLib/src/inftrees.c \
	3rdParty/ZLib/src/trees.c \
	3rdParty/ZLib/src/uncompr.c \
	3rdParty/ZLib/src/zutil.c

ZLIB_OBJECTS = \
	$(ZLIB_SOURCES:.c=.o)

CLEANFILES += \
	$(ZLIB_SOURCES:.c=.gcda) \
	$(ZLIB_SOURCES:.c=.gcno) \
	$(ZLIB_OBJECTS)
