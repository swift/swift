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
#include "Roster/QtTreeWidgetFactory.h"
#include "QtSystemTray.h"
#include "QtSoundPlayer.h"
#include "QtXMLConsoleWidgetFactory.h"
#include "ChatList/QtChatListWindowFactory.h"
#include "EventViewer/QtEventWindowFactory.h"
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
#include "Swift/QtUI/BuildVersion.h"
#include "SwifTools/AutoUpdater/AutoUpdater.h"
#include "SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h"

namespace Swift{

#if defined(SWIFTEN_PLATFORM_MACOSX)
#define SWIFT_APPCAST_URL "http://swift.im/appcast/swift-mac-dev.xml"
#else 
#define SWIFT_APPCAST_URL ""
#endif


QtSwift::QtSwift(bool netbookMode) : autoUpdater_(NULL) {
	if (netbookMode) {
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
	treeWidgetFactory_ = new QtTreeWidgetFactory(); 
	systemTray_ = new QtSystemTray();
	loginWindowFactory_ = new QtLoginWindowFactory(splitter_, systemTray_, settings_);
	chatWindowFactory_ = new QtChatWindowFactory(treeWidgetFactory_, splitter_, settings_, tabs_);
	rosterWindowFactory_ = new QtMainWindowFactory(treeWidgetFactory_);
	eventWindowFactory_ = new QtEventWindowFactory(rosterWindowFactory_);
	xmlConsoleWidgetFactory_ = new QtXMLConsoleWidgetFactory(tabs_);
	chatListWindowFactory_ = new QtChatListWindowFactory(rosterWindowFactory_);
	soundPlayer_ = new QtSoundPlayer();
	if (splitter_) {
		splitter_->show();
	}
	mainController_ = new MainController(chatWindowFactory_, rosterWindowFactory_, loginWindowFactory_, treeWidgetFactory_, eventWindowFactory_, settings_, application_, systemTray_, soundPlayer_, xmlConsoleWidgetFactory_, chatListWindowFactory_);

	PlatformAutoUpdaterFactory autoUpdaterFactory;
	if (autoUpdaterFactory.isSupported()) {
		autoUpdater_ = autoUpdaterFactory.createAutoUpdater(SWIFT_APPCAST_URL);
		autoUpdater_->checkForUpdates();
	}
}

QtSwift::~QtSwift() {
	delete autoUpdater_;
	delete chatWindowFactory_;
	delete rosterWindowFactory_;
	delete loginWindowFactory_;
	delete treeWidgetFactory_;
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
