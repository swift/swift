/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSwift.h"

#include <string>
#include <QSplitter>
#include <QFile>
#include <boost/bind.hpp>
#include <QMessageBox>
#include <QApplication>

#include <QtLoginWindow.h>
#include <QtChatTabs.h>
#include <QtSystemTray.h>
#include <QtSoundPlayer.h>
#include <QtSwiftUtil.h>
#include <QtUIFactory.h>
#include <QtChatWindowFactory.h>
#include <Swiften/Base/Log.h>
#include <Swift/Controllers/Storages/CertificateFileStorageFactory.h>
#include <Swift/Controllers/Storages/FileStoragesFactory.h>
#include <SwifTools/Application/PlatformApplicationPathProvider.h>
#include <string>
#include <Swiften/Base/Platform.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Client/Client.h>
#include <Swift/Controllers/Settings/XMLSettingsProvider.h>
#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>
#include <Swift/Controllers/MainController.h>
#include <Swift/Controllers/ApplicationInfo.h>
#include <Swift/Controllers/BuildVersion.h>
#include <SwifTools/AutoUpdater/AutoUpdater.h>
#include <SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h>
#include "Swiften/Base/Paths.h"

#if defined(SWIFTEN_PLATFORM_WINDOWS)
#include "WindowsNotifier.h"
#elif defined(HAVE_GROWL)
#include "SwifTools/Notifier/GrowlNotifier.h"
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

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "QtURIHandler.h"
#elif defined(SWIFTEN_PLATFORM_WIN32)
#include <SwifTools/URIHandler/NullURIHandler.h>
#else
#include "QtDBUSURIHandler.h"
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
		("help", "Show this help message")
		("version", "Show version information")
		("netbook-mode", "Use netbook mode display (unsupported)")
		("no-tabs", "Don't manage chat windows in tabs (unsupported)")
		("latency-debug", "Use latency debugging (unsupported)")
		("multi-account", po::value<int>()->default_value(1), "Number of accounts to open windows for (unsupported)")
		("start-minimized", "Don't show the login/roster window at startup")
		;
	return result;
}

XMLSettingsProvider* QtSwift::loadSettingsFile(const QString& fileName) {
	QFile configFile(fileName);
	if (configFile.exists() && configFile.open(QIODevice::ReadOnly)) {
		QString xmlString;
		while (!configFile.atEnd()) {
			QByteArray line = configFile.readLine();
			xmlString += line + "\n";
		}
		return new XMLSettingsProvider(Q2PSTRING(xmlString));
	}
	return new XMLSettingsProvider("");
}

QtSwift::QtSwift(const po::variables_map& options) : networkFactories_(&clientMainThreadCaller_), autoUpdater_(NULL), idleDetector_(&idleQuerier_, networkFactories_.getTimerFactory(), 1000) {
	if (options.count("netbook-mode")) {
		splitter_ = new QSplitter();
	} else {
		splitter_ = NULL;
	}
	QCoreApplication::setApplicationName(SWIFT_APPLICATION_NAME);
	QCoreApplication::setOrganizationName(SWIFT_ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(SWIFT_ORGANIZATION_DOMAIN);
	QCoreApplication::setApplicationVersion(buildVersion);

	qtSettings_ = new QtSettingsProvider();
	xmlSettings_ = loadSettingsFile(P2QSTRING((Paths::getExecutablePath() / "system-settings.xml").string()));
	settingsHierachy_ = new SettingsProviderHierachy();
	settingsHierachy_->addProviderToTopOfStack(xmlSettings_);
	settingsHierachy_->addProviderToTopOfStack(qtSettings_);

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
	applicationPathProvider_ = new PlatformApplicationPathProvider(SWIFT_APPLICATION_NAME);
	storagesFactory_ = new FileStoragesFactory(applicationPathProvider_->getDataDir());
	certificateStorageFactory_ = new CertificateFileStorageFactory(applicationPathProvider_->getDataDir(), tlsFactories_.getCertificateFactory());
	chatWindowFactory_ = new QtChatWindowFactory(splitter_, settingsHierachy_, qtSettings_, tabs_, "");
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

#if defined(SWIFTEN_PLATFORM_MACOSX)
	uriHandler_ = new QtURIHandler();
#elif defined(SWIFTEN_PLATFORM_WIN32)
	uriHandler_ = new NullURIHandler();
#else
	uriHandler_ = new QtDBUSURIHandler();
#endif

	if (splitter_) {
		splitter_->show();
	}

	for (int i = 0; i < numberOfAccounts; i++) {
		if (i > 0) {
			// Don't add the first tray (see note above)
			systemTrays_.push_back(new QtSystemTray());
		}
		QtUIFactory* uiFactory = new QtUIFactory(settingsHierachy_, qtSettings_, tabs_, splitter_, systemTrays_[i], chatWindowFactory_, startMinimized);
		uiFactories_.push_back(uiFactory);
		MainController* mainController = new MainController(
				&clientMainThreadCaller_,
				&networkFactories_,
				uiFactory,
				settingsHierachy_,
				systemTrays_[i],
				soundPlayer_,
				storagesFactory_,
				certificateStorageFactory_,
				dock_,
				notifier_,
				uriHandler_,
				&idleDetector_,
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
	delete settingsHierachy_;
	delete qtSettings_;
	delete xmlSettings_;
	foreach (QtSystemTray* tray, systemTrays_) {
		delete tray;
	}
	delete tabs_;
	delete splitter_;
	delete uriHandler_;
	delete dock_;
	delete soundPlayer_;
	delete chatWindowFactory_;
	delete certificateStorageFactory_;
	delete storagesFactory_;
}

}
