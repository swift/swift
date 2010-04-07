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
#include "Swiften/Application/PlatformApplication.h"
#include "Swiften/Application/PlatformApplicationPathProvider.h"
#include "Swiften/Avatars/AvatarFileStorage.h"
#include "Swiften/VCards/VCardFileStorageFactory.h"
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
		("no-tabs", "don't manage chat windows in tabs")
		("latency-debug", "use latency debugging")
		("multi-account", po::value<int>()->default_value(1), "number of accounts to open windows for")
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

	int numberOfAccounts = 1;
	try {
		numberOfAccounts = options["multi-account"].as<int>();
	} catch (...) {
		/* This seems to fail on a Mac when the .app is launched directly (the usual path).*/
		numberOfAccounts = 1;
	}

	tabs_ = options.count("no-tabs") && !(splitter_ > 0) ? NULL : new QtChatTabs();
	settings_ = new QtSettingsProvider();
	application_ = new PlatformApplication(SWIFT_APPLICATION_NAME);
	applicationPathProvider_ = new PlatformApplicationPathProvider(SWIFT_APPLICATION_NAME);
	avatarStorage_ = new AvatarFileStorage(applicationPathProvider_->getAvatarDir());
	vcardStorageFactory_ = new VCardFileStorageFactory(applicationPathProvider_->getDataDir());
	chatWindowFactory_ = new QtChatWindowFactory(splitter_, settings_, tabs_);
	soundPlayer_ = new QtSoundPlayer(applicationPathProvider_);
	if (splitter_) {
		splitter_->show();
	}

	for (int i = 0; i < numberOfAccounts; i++) {
		QtSystemTray* systemTray = new QtSystemTray();
		systemTrays_.push_back(systemTray);
		QtLoginWindowFactory* loginWindowFactory = new QtLoginWindowFactory(splitter_, systemTray, settings_);
		loginWindowFactories_.push_back(loginWindowFactory);
		QtMainWindowFactory* rosterWindowFactory = new QtMainWindowFactory();
		rosterWindowFactories_.push_back(rosterWindowFactory);
		QtEventWindowFactory* eventWindowFactory = new QtEventWindowFactory(rosterWindowFactory);
		eventWindowFactories_.push_back(eventWindowFactory);
		QtXMLConsoleWidgetFactory* xmlConsoleWidgetFactory = new QtXMLConsoleWidgetFactory(tabs_);
		xmlConsoleWidgetFactories_.push_back(xmlConsoleWidgetFactory);
		QtChatListWindowFactory* chatListWindowFactory = new QtChatListWindowFactory(rosterWindowFactory);
		chatListWindowFactories_.push_back(chatListWindowFactory);
		QtMUCSearchWindowFactory* mucSearchWindowFactory = new QtMUCSearchWindowFactory();
		mucSearchWindowFactories_.push_back(mucSearchWindowFactory);
		MainController* mainController = new MainController(
				chatWindowFactory_,
				rosterWindowFactory,
				loginWindowFactory,
				eventWindowFactory,
				settings_,
				systemTray,
				soundPlayer_,
				xmlConsoleWidgetFactory,
				chatListWindowFactory,
				mucSearchWindowFactory,
				avatarStorage_,
				vcardStorageFactory_,
				application_->getApplicationMessageDisplay(),
				options.count("latency-debug") > 0);
		mainControllers_.push_back(mainController);
	}


	// PlatformAutoUpdaterFactory autoUpdaterFactory;
	// if (autoUpdaterFactory.isSupported()) {
	// 	autoUpdater_ = autoUpdaterFactory.createAutoUpdater(SWIFT_APPCAST_URL);
	// 	autoUpdater_->checkForUpdates();
	// }
}

QtSwift::~QtSwift() {
	delete autoUpdater_;
	delete chatWindowFactory_;
	foreach (QtMainWindowFactory* factory, rosterWindowFactories_) {
		delete factory;
	}
	foreach (QtLoginWindowFactory* factory, loginWindowFactories_) {
		delete factory;
	}
	foreach (MUCSearchWindowFactory* factory, mucSearchWindowFactories_) {
		delete factory;
	}
	foreach (MainController* controller, mainControllers_) {
		delete controller;
	}
	delete settings_;
	delete application_;
	foreach (QtSystemTray* tray, systemTrays_) {
		delete tray;
	}
	delete tabs_;
	delete splitter_;
	delete soundPlayer_;
	foreach (QtXMLConsoleWidgetFactory* factory, xmlConsoleWidgetFactories_) {
		delete factory;
	}
	foreach (QtEventWindowFactory* factory, eventWindowFactories_) {
		delete factory;
	}
	foreach (QtChatListWindowFactory* factory, chatListWindowFactories_) {
		delete factory;
	}
	delete avatarStorage_;
	delete vcardStorageFactory_;
}

}
