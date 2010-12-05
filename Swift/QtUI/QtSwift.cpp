/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSwift.h"

#include <string>

#include "QtLoginWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtLoginWindow.h"
#include "QtChatTabs.h"
#include "QtMainWindowFactory.h"
#include "QtSystemTray.h"
#include "QtSoundPlayer.h"
#include "QtSwiftUtil.h"
#include "QtXMLConsoleWidgetFactory.h"
#include "ChatList/QtChatListWindowFactory.h"
#include "EventViewer/QtEventWindowFactory.h"
#include "MUCSearch/QtMUCSearchWindowFactory.h"
#include <boost/bind.hpp>
#include <QSplitter>

#include <Swiften/Base/Log.h>
#include <Swift/Controllers/CertificateFileStorageFactory.h>
#include "SwifTools/Application/PlatformApplicationPathProvider.h"
#include "Swiften/Avatars/AvatarFileStorage.h"
#include "Swiften/Disco/CapsFileStorage.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Platform.h"
#include "Swift/Controllers/FileStoragesFactory.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/MainController.h"
#include "Swift/Controllers/ApplicationInfo.h"
#include "Swift/Controllers/BuildVersion.h"
#include "SwifTools/AutoUpdater/AutoUpdater.h"
#include "SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h"
#if defined(HAVE_GROWL)
#include "SwifTools/Notifier/GrowlNotifier.h"
#elif defined(HAVE_SNARL)
#include "QtWin32NotifierWindow.h"
#include "SwifTools/Notifier/SnarlNotifier.h"
#elif defined(SWIFTEN_PLATFORM_LINUX)
#include "FreeDesktopNotifier.h"
#else
#include "SwifTools/Notifier/NullNotifier.h"
#endif
#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "SwifTools/Dock/MacOSXDock.h"
#else
#include "SwifTools/Dock/NullDock.h"
#endif

namespace Swift{

#if defined(SWIFTEN_PLATFORM_MACOSX)
#define SWIFT_APPCAST_URL "http://swift.im/appcast/swift-mac-dev.xml"
#else 
#define SWIFT_APPCAST_URL ""
#endif

po::options_description QtSwift::getOptionsDescription() {
	po::options_description result("Options");
	result.add_options()
		("debug", "Turn on debug logging")
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

	if (options.count("debug")) {
		Swift::logging = true;
	}

	tabs_ = options.count("no-tabs") && !(splitter_ > 0) ? NULL : new QtChatTabs();
	settings_ = new QtSettingsProvider();
	applicationPathProvider_ = new PlatformApplicationPathProvider(SWIFT_APPLICATION_NAME);
	storagesFactory_ = new FileStoragesFactory(applicationPathProvider_->getDataDir());
	certificateStorageFactory_ = new CertificateFileStorageFactory(applicationPathProvider_->getDataDir(), tlsFactories_.getCertificateFactory());
	chatWindowFactory_ = new QtChatWindowFactory(splitter_, settings_, tabs_, "");
	soundPlayer_ = new QtSoundPlayer(applicationPathProvider_);
#if defined(HAVE_GROWL)
	notifier_ = new GrowlNotifier(SWIFT_APPLICATION_NAME);
#elif defined(HAVE_SNARL)
	notifierWindow_ = new QtWin32NotifierWindow();
	notifier_ = new SnarlNotifier(SWIFT_APPLICATION_NAME, notifierWindow_, applicationPathProvider_->getResourcePath("/images/logo-icon-32.png"));
#elif defined(SWIFTEN_PLATFORM_LINUX)
	notifier_ = new FreeDesktopNotifier(SWIFT_APPLICATION_NAME);
#else
	notifier_ = new NullNotifier();
#endif

#if defined(SWIFTEN_PLATFORM_MACOSX)
	dock_ = new MacOSXDock(&cocoaApplication_);
#else
	dock_ = new NullDock();
#endif

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
				&clientMainThreadCaller_,
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
				storagesFactory_,
				certificateStorageFactory_,
				dock_,
				notifier_,
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
	delete notifier_;
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
	foreach (QtSystemTray* tray, systemTrays_) {
		delete tray;
	}
	delete tabs_;
	delete splitter_;
	delete dock_;
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
	delete certificateStorageFactory_;
	delete storagesFactory_;
}

}
