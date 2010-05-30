/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSwift.h"

#include "QtLoginWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtLoginWindow.h"
#include "QtChatTabs.h"
#include "QtMainWindowFactory.h"
#include "QtSystemTray.h"
#include "QtSoundPlayer.h"
#include "QtXMLConsoleWidgetFactory.h"
#include "ChatList/QtChatListWindowFactory.h"
#include "EventViewer/QtEventWindowFactory.h"
#include "MUCSearch/QtMUCSearchWindowFactory.h"
#include <boost/bind.hpp>
#include <QSplitter>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/Platform/PlatformApplication.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Platform.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/MainController.h"
#include "Swift/Controllers/ApplicationInfo.h"
#include "Swift/Controllers/BuildVersion.h"
#include "SwifTools/AutoUpdater/AutoUpdater.h"
#include "SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h"

namespace Swift{

#if defined(SWIFTEN_PLATFORM_MACOSX)
#define SWIFT_APPCAST_URL "http://swift.im/appcast/swift-mac-dev.xml"
#else 
#define SWIFT_APPCAST_URL ""
#endif

po::options_description QtSwift::getOptionsDescription() {
	po::options_description result("Options");
	result.add_options()
		("help", "produce help message")
		("netbook-mode", "use netbook mode display")
		("latency-debug", "use latency debugging")
		;
	return result;
}


QtSwift::QtSwift(po::variables_map options) : autoUpdater_(NULL) {
	if (options.count("netbook-mode")) {
		splitter_ = new QSplitter();
	} else {
		splitter_ = NULL;
	}
	QCoreApplication::setApplicationName(SWIFT_APPLICATION_NAME);
	QCoreApplication::setOrganizationName(SWIFT_ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(SWIFT_ORGANIZATION_DOMAIN);
	QCoreApplication::setApplicationVersion(buildVersion);

	tabs_ = new QtChatTabs();
	settings_ = new QtSettingsProvider();
	application_ = new PlatformApplication(SWIFT_APPLICATION_NAME);
	systemTray_ = new QtSystemTray();
	loginWindowFactory_ = new QtLoginWindowFactory(splitter_, systemTray_, settings_);
	chatWindowFactory_ = new QtChatWindowFactory(splitter_, settings_, tabs_);
	rosterWindowFactory_ = new QtMainWindowFactory();
	eventWindowFactory_ = new QtEventWindowFactory(rosterWindowFactory_);
	xmlConsoleWidgetFactory_ = new QtXMLConsoleWidgetFactory(tabs_);
	chatListWindowFactory_ = new QtChatListWindowFactory(rosterWindowFactory_);
	mucSearchWindowFactory_ = new QtMUCSearchWindowFactory();
	soundPlayer_ = new QtSoundPlayer();
	if (splitter_) {
		splitter_->show();
	}
	mainController_ = new MainController(chatWindowFactory_, rosterWindowFactory_, loginWindowFactory_, eventWindowFactory_, settings_, application_, systemTray_, soundPlayer_, xmlConsoleWidgetFactory_, chatListWindowFactory_, mucSearchWindowFactory_, options.count("latency-debug") > 0);

	// PlatformAutoUpdaterFactory autoUpdaterFactory;
	// if (autoUpdaterFactory.isSupported()) {
	// 	autoUpdater_ = autoUpdaterFactory.createAutoUpdater(SWIFT_APPCAST_URL);
	// 	autoUpdater_->checkForUpdates();
	// }
}

QtSwift::~QtSwift() {
	delete autoUpdater_;
	delete chatWindowFactory_;
	delete rosterWindowFactory_;
	delete loginWindowFactory_;
	delete mucSearchWindowFactory_;
	delete mainController_;
	delete settings_;
	delete application_;
	delete systemTray_;
	delete splitter_;
	delete soundPlayer_;
	delete tabs_;
	delete xmlConsoleWidgetFactory_;
	delete eventWindowFactory_;
	delete chatListWindowFactory_;
}

}
