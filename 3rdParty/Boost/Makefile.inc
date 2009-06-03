CPPFLAGS += -DBOOST_ALL_NO_LIB
CXXFLAGS += -isystem 3rdParty/Boost -DBOOST_SIGNALS_NAMESPACE=bsignals

BOOST_SOURCES += \
	3rdParty/Boost/libs/date_time/src/gregorian/date_generators.cpp \
	3rdParty/Boost/libs/date_time/src/gregorian/greg_month.cpp \
	3rdParty/Boost/libs/date_time/src/gregorian/greg_weekday.cpp \
	3rdParty/Boost/libs/date_time/src/gregorian/gregorian_types.cpp \
	3rdParty/Boost/libs/date_time/src/posix_time/posix_time_types.cpp \
	3rdParty/Boost/libs/system/src/error_code.cpp \
	3rdParty/Boost/libs/thread/src/tss_null.cpp \
	3rdParty/Boost/libs/signals/src/connection.cpp \
	3rdParty/Boost/libs/signals/src/named_slot_map.cpp \
	3rdParty/Boost/libs/signals/src/signal_base.cpp \
	3rdParty/Boost/libs/signals/src/slot.cpp \
	3rdParty/Boost/libs/signals/src/trackable.cpp \
	3rdParty/Boost/libs/filesystem/src/operations.cpp \
	3rdParty/Boost/libs/filesystem/src/path.cpp \
	3rdParty/Boost/libs/filesystem/src/portability.cpp \
	3rdParty/Boost/libs/filesystem/src/utf8_codecvt_facet.cpp

ifeq ($(WIN32),1)
# The WINNT flag is the boost default (Windows XP Target)
CXXFLAGS +=  -D_WIN32_WINNT=0x0501
# Only needed for CygWin
CXXFLAGS +=  -D__USE_W32_SOCKETS
BOOST_SOURCES += \
	3rdParty/Boost/win32_stubs.cpp \
	3rdParty/Boost/libs/thread/src/win32/exceptions.cpp \
	3rdParty/Boost/libs/thread/src/win32/thread.cpp \
	3rdParty/Boost/libs/thread/src/win32/tss_dll.cpp \
	3rdParty/Boost/libs/thread/src/win32/tss_pe.cpp
else
LIBS += -lpthread
BOOST_SOURCES += \
	3rdParty/Boost/libs/thread/src/pthread/exceptions.cpp \
	3rdParty/Boost/libs/thread/src/pthread/once.cpp \
	3rdParty/Boost/libs/thread/src/pthread/thread.cpp
endif

BOOST_OBJECTS = \
	$(BOOST_SOURCES:.cpp=.o)

CLEANFILES += \
	$(BOOST_OBJECTS)
