TEMPLATE = app
TARGET = ChatView
DEPENDPATH += .
INCLUDEPATH += . ../../../Swiften/3rdParty/Boost
QT += webkit network
HEADERS += ../QtChatView.h
SOURCES += main.cpp ../QtChatView.cpp ../ChatSnippet.cpp ../MessageSnippet.cpp ../SystemMessageSnippet.cpp
RESOURCES += ../DefaultTheme.qrc ../Swift.qrc
