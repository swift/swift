TEMPLATE = app
QT += webkit
CONFIG += debug
unix:!mac {
	TARGET = swift
}
else {
	TARGET = Swift
}

win32 {
	CONFIG += console

# Configuration
	HAVE_EXPAT=yes
	HAVE_LIBXML=
	HAVE_OPENSSL=yes
	DEFINES += HAVE_OPENSSL
	DEFINES += HAVE_EXPAT
	INCLUDEPATH += F:/OpenSSL/include
	INCLUDEPATH += "F:/Expat 2.0.1/Source/lib"
#LIBS += -L"F:/Expat 2.0.1/Bin"  -lexpat
	LIBS += "F:/Expat 2.0.1/Bin/libexpat.lib"
	LIBS += -LF:/OpenSSL/lib/VC -llibeay32MT -lssleay32MT

	include(Swiften.pri)
	LIBS += -ldnsapi -lws2_32 -lwsock32
	} else {
	DEPENDPATH += . ../.. ../../3rdParty/Boost
	INCLUDEPATH += . ../.. ../../3rdParty/Boost
	LIBS += ../../Swiften/Swiften.a -lexpat -lssl -lcrypto
	unix {
		LIBS += -lresolv
	}
}

# Resources
win32 {
	RC_FILE = ../../resources/Windows/Swift.rc
}
mac {
	ICON = ../../resources/MacOSX/Swift.icns
}

DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals BOOST_ALL_NO_LIB

HEADERS += QtChatWindow.h \
           QtChatWindowFactory.h \
           QtJoinMUCDialog.h \
           QtLoginWindow.h \
           QtLoginWindowFactory.h \
           QtMainEventLoop.h \
           QtMainWindow.h \
           QtMainWindowFactory.h \
           QtSettingsProvider.h \
           QtStatusWidget.h \
           QtSwift.h \
           QtTreeWidget.h \
           QtTreeWidgetFactory.h \
           QtTreeWidgetItem.h \
					 QtChatView.h \
					 ChatSnippet.h \
					 MessageSnippet.h \
					 SystemMessageSnippet.h

SOURCES += main.cpp \
           QtChatWindow.cpp \
           QtChatWindowFactory.cpp \
           QtJoinMUCDialog.cpp \
           QtLoginWindow.cpp \
           QtLoginWindowFactory.cpp \
           QtMainWindow.cpp \
           QtMainWindowFactory.cpp \
           QtSettingsProvider.cpp \
           QtStatusWidget.cpp \
           QtSwift.cpp \
           QtTreeWidget.cpp \
					 QtChatView.cpp  \
					 ChatSnippet.cpp \
					 MessageSnippet.cpp \
					 SystemMessageSnippet.cpp

FORMS += QtJoinMUCDialog.ui

RESOURCES += Swift.qrc DefaultTheme.qrc

win32 {
	DefaultThemeQRC.target = DefaultTheme.qrc
	DefaultThemeQRC.commands = ..\..\..\tools\ThemeQRC.py ../../../resources/themes/Default > DefaultTheme.qrc
	QMAKE_EXTRA_TARGETS = DefaultThemeQRC
}

LIBS += -fprofile-arcs -ftest-coverage 
