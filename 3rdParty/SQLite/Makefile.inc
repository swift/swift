SQLITE_CPPFLAGS += -isystem 3rdParty/SQLite

SQLITE_SOURCES = \
	3rdParty/SQLite/sqlite3.c

SQLITE_OBJECTS = \
	$(SQLITE_SOURCES:.c=.o)
