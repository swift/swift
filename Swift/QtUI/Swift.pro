TEMPLATE = app
QT += webkit phonon
CONFIG += debug
unix:!mac {
	TARGET = swift
}
else {
	TARGET = Swift
}

DEPENDPATH += . ../.. ../../3rdParty/Boost
INCLUDEPATH += . ../.. ../../3rdParty/Boost
exists(config.pri) {
	LIBS += ../Controllers/Controllers.a ../../Swiften/Swiften.a
	include(config.pri)
}
else {
	# Legacy build type
	HAVE_EXPAT=yes
	USE_BUNDLED_EXPAT=yes
	DEFINES += HAVE_EXPAT

	HAVE_OPENSSL=yes
	DEFINES += HAVE_OPENSSL
	INCLUDEPATH += C:/OpenSSL/include
	LIBS += -LC:/OpenSSL/lib/VC -llibeay32MT -lssleay32MT

	include(Swiften.pri)
	include(fixwindows.pri)
}

win32 {
	CONFIG += console
}

# Resources
win32 {
	RC_FILE = ../resources/Windows/Swift.rc
}
mac {
	ICON = ../resources/MacOSX/Swift.icns
}

DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals BOOST_ALL_NO_LIB

HEADERS += \
	QtChatWindow.h \
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
	QtChatTabs.h \
	QtSoundPlayer.h \
	QtSystemTray.h \
	QtTabbable.h \
	ChatSnippet.h \
	MessageSnippet.h \
	SystemMessageSnippet.h

SOURCES += \
	main.cpp \
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
	QtChatView.cpp	\
	QtChatTabs.cpp \
	QtSoundPlayer.cpp \
	QtSystemTray.cpp \
	ChatSnippet.cpp \
	MessageSnippet.cpp \
	SystemMessageSnippet.cpp

FORMS += QtJoinMUCDialog.ui

RESOURCES += Swift.qrc DefaultTheme.qrc

win32 {
	DefaultThemeQRC.target = DefaultTheme.qrc
	DefaultThemeQRC.commands = ..\..\tools\ThemeQRC.py ../resources/themes/Default > DefaultTheme.qrc
	QMAKE_EXTRA_TARGETS = DefaultThemeQRC
}
