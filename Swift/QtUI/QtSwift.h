#ifndef SWIFT_QtSwift_H
#define SWIFT_QtSwift_H

#include "Swiften/Base/String.h"
#include "QtMainEventLoop.h"
#include "QtLoginWindowFactory.h"
#include "QtMainWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtSettingsProvider.h"

class QSplitter;

namespace Swift {
	class Application;
	class MainController;
	class QtChatWindowFactory;
	class QtMainWindowFactory;
	class QtLoginWindowFactory;
	class QtTreeWidgetFactory;
	class QtSystemTray;
	
	class QtSwift : public QObject {
		Q_OBJECT
		public:
			QtSwift(bool netbookMode);
			~QtSwift();
		private:
			MainController *mainController_;
			QtTreeWidgetFactory *treeWidgetFactory_;
			QtChatWindowFactory *chatWindowFactory_;
			QtMainWindowFactory *rosterWindowFactory_;
			QtLoginWindowFactory *loginWindowFactory_;
			QtMainEventLoop clientMainThreadCaller_;
			QtSettingsProvider *settings_;
			QtSystemTray* systemTray_;
			QSplitter* splitter_;
			Application* application_;
	};
}

#endif
