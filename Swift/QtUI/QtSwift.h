/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtSwift_H
#define SWIFT_QtSwift_H

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/EventLoop/Qt/QtEventLoop.h"
#include "QtLoginWindowFactory.h"
#include "QtMainWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtSettingsProvider.h"

namespace po = boost::program_options;

class QSplitter;

namespace Swift {
	class AutoUpdater;
	class Application;
	class MainController;
	class QtChatWindowFactory;
	class QtMainWindowFactory;
	class QtLoginWindowFactory;
	class QtXMLConsoleWidgetFactory;
	class QtSystemTray;
	class QtSoundPlayer;
	class QtEventWindowFactory;
	class QtChatListWindowFactory;
	class QtMUCSearchWindowFactory;
		
	class QtSwift : public QObject {
		Q_OBJECT
		public:
			QtSwift(po::variables_map options);
			static po::options_description getOptionsDescription();
			~QtSwift();
		private:
			std::vector<MainController*> mainControllers_;
			QtChatWindowFactory *chatWindowFactory_;
			std::vector<QtMainWindowFactory*> rosterWindowFactories_;
			std::vector<QtLoginWindowFactory*> loginWindowFactories_;
			std::vector<QtXMLConsoleWidgetFactory*> xmlConsoleWidgetFactories_;
			std::vector<QtEventWindowFactory*> eventWindowFactories_;
			std::vector<QtSystemTray*> systemTrays_;
			std::vector<QtChatListWindowFactory*> chatListWindowFactories_;
			std::vector<QtMUCSearchWindowFactory*> mucSearchWindowFactories_;
			QtEventLoop clientMainThreadCaller_;
			QtSettingsProvider *settings_;
			QSplitter* splitter_;
			QtSoundPlayer* soundPlayer_;
			QtChatTabs* tabs_;
			Application* application_;
			AutoUpdater* autoUpdater_;

	};
}

#endif
