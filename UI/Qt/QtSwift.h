#ifndef SWIFT_QtSwift_H
#define SWIFT_QtSwift_H

#include "Swiften/Base/String.h"
#include "QtMainEventLoop.h"
#include "QtLoginWindowFactory.h"
#include "QtMainWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtSettingsProvider.h"

namespace Swift {
	class Application;
	class MainController;
	class QtChatWindowFactory;
	class QtMainWindowFactory;
	class QtLoginWindowFactory;
	class QtTreeWidgetFactory;

	class QtSwift : public QObject {
		Q_OBJECT
		public:
			QtSwift();
			~QtSwift();
		private:
			MainController *mainController_;
			QtTreeWidgetFactory *treeWidgetFactory_;
			QtChatWindowFactory *chatWindowFactory_;
			QtMainWindowFactory *rosterWindowFactory_;
			QtLoginWindowFactory *loginWindowFactory_;
			QtMainEventLoop clientMainThreadCaller_;
			QtSettingsProvider *settings_;
			Application* application_;
	};
}

#endif
