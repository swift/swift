#ifndef SWIFT_QtSwift_H
#define SWIFT_QtSwift_H

#include "Swiften/Base/String.h"
#include "Swiften/EventLoop/Qt/QtEventLoop.h"
#include "QtLoginWindowFactory.h"
#include "QtMainWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtSettingsProvider.h"

class QSplitter;

namespace Swift {
	class AutoUpdater;
	class Application;
	class MainController;
	class QtChatWindowFactory;
	class QtMainWindowFactory;
	class QtLoginWindowFactory;
	class QtTreeWidgetFactory;
	class QtXMLConsoleWidgetFactory;
	class QtSystemTray;
	class QtSoundPlayer;
	class QtEventWindowFactory;
	class QtChatListWindowFactory;
		
	class QtSwift : public QObject {
		Q_OBJECT
		public:
			QtSwift(bool netbookMode);
			~QtSwift();
		private:
			MainController *mainController_;
			QtTreeWidgetFactory *treeWidgetFactory_;
			QtChatWindowFactory *chatWindowFactory_;
			QtChatListWindowFactory *chatListWindowFactory_;
			QtMainWindowFactory *rosterWindowFactory_;
			QtLoginWindowFactory *loginWindowFactory_;
			QtXMLConsoleWidgetFactory* xmlConsoleWidgetFactory_;
			QtEventLoop clientMainThreadCaller_;
			QtSettingsProvider *settings_;
			QtSystemTray* systemTray_;
			QSplitter* splitter_;
			QtSoundPlayer* soundPlayer_;
			QtChatTabs* tabs_;
			QtEventWindowFactory* eventWindowFactory_;
			Application* application_;
			AutoUpdater* autoUpdater_;
	};
}

#endif
