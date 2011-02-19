/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSwift.h"

#include <string>
#include <QSplitter>
#include <boost/bind.hpp>

#include "QtLoginWindow.h"
#include "QtChatTabs.h"
#include "QtSystemTray.h"
#include "QtSoundPlayer.h"
#include "QtSwiftUtil.h"
#include "QtUIFactory.h"
#include "QtChatWindowFactory.h"
#include <Swiften/Base/Log.h>
#include <Swift/Controllers/CertificateFileStorageFactory.h>
#include "SwifTools/Application/PlatformApplicationPathProvider.h"
#include "Swiften/Avatars/AvatarFileStorage.h"
#include "Swiften/Disco/CapsFileStorage.h"
#include <string>
#include "Swiften/Base/Platform.h"
#include "Swift/Controllers/FileStoragesFactory.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/MainController.h"
#include "Swift/Controllers/ApplicationInfo.h"
#include "Swift/Controllers/BuildVersion.h"
#include "SwifTools/AutoUpdater/AutoUpdater.h"
#include "SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h"
#if defined(SWIFTEN_PLATFORM_WINDOWS)
#include "WindowsNotifier.h"
#endif
#if defined(HAVE_GROWL)
#include "SwifTools/Notifier/GrowlNotifier.h"
#elif defined(SWIFTEN_PLATFORM_LINUX)
#include "FreeDesktopNotifier.h"
#else
#include "SwifTools/Notifier/NullNotifier.h"
#endif
#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "SwifTools/Dock/MacOSXDock.h"
#endif
#include "SwifTools/Dock/NullDock.h"

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
		("netbook-mode", "use netbook mode display (unsupported)")
		("no-tabs", "don't manage chat windows in tabs (unsupported)")
		("latency-debug", "use latency debugging (unsupported)")
		("multi-account", po::value<int>()->default_value(1), "number of accounts to open windows for (unsupported)")
		("start-minimized", "don't show the login/roster window at startup")
		;
	return result;
}


QtSwift::QtSwift(const po::variables_map& options) : networkFactories_(&clientMainThreadCaller_), autoUpdater_(NULL) {
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
	bool startMinimized = options.count("start-minimized") > 0;
	settings_ = new QtSettingsProvider();
	applicationPathProvider_ = new PlatformApplicationPathProvider(SWIFT_APPLICATION_NAME);
	storagesFactory_ = new FileStoragesFactory(applicationPathProvider_->getDataDir());
	certificateStorageFactory_ = new CertificateFileStorageFactory(applicationPathProvider_->getDataDir(), tlsFactories_.getCertificateFactory());
	chatWindowFactory_ = new QtChatWindowFactory(splitter_, settings_, tabs_, "");
	soundPlayer_ = new QtSoundPlayer(applicationPathProvider_);

	// Ugly, because the dock depends on the tray, but the temporary
	// multi-account hack creates one tray per account.
	QtSystemTray* systemTray = new QtSystemTray();
	systemTrays_.push_back(systemTray);

#if defined(HAVE_GROWL)
	notifier_ = new GrowlNotifier(SWIFT_APPLICATION_NAME);
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
	notifier_ = new WindowsNotifier(SWIFT_APPLICATION_NAME, applicationPathProvider_->getResourcePath("/images/logo-icon-32.png"), systemTray->getQSystemTrayIcon());
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
		if (i > 0) {
			// Don't add the first tray (see note above)
			systemTrays_.push_back(new QtSystemTray());
		}
		QtUIFactory* uiFactory = new QtUIFactory(settings_, tabs_, splitter_, systemTrays_[i], chatWindowFactory_, startMinimized);
		uiFactories_.push_back(uiFactory);
		MainController* mainController = new MainController(
				&clientMainThreadCaller_,
				&networkFactories_,
				uiFactory,
				settings_,
				systemTrays_[i],
				soundPlayer_,
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
	foreach (QtUIFactory* factory, uiFactories_) {
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
	delete chatWindowFactory_;
	delete certificateStorageFactory_;
	delete storagesFactory_;
}

}
