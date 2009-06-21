#include "QtSwift.h"

#include "QtLoginWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtMainWindowFactory.h"
#include "QtTreeWidgetFactory.h"
#include "QtSystemTray.h"

#include <boost/bind.hpp>
#include <QSplitter>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/Platform/PlatformApplication.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/ChatController.h"
#include "Swift/Controllers/MainController.h"

namespace Swift{

QtSwift::QtSwift() {
	QSplitter* splitter = new QSplitter();
	treeWidgetFactory_ = new QtTreeWidgetFactory(); 
	loginWindowFactory_ = new QtLoginWindowFactory(splitter);
	rosterWindowFactory_ = new QtMainWindowFactory(treeWidgetFactory_);
	chatWindowFactory_ = new QtChatWindowFactory(treeWidgetFactory_, splitter);
	systemTray_ = new QtSystemTray();
	QCoreApplication::setApplicationName("Swift");
	QCoreApplication::setOrganizationName("Swift");
	QCoreApplication::setOrganizationDomain("swift.im");
	settings_ = new QtSettingsProvider();
	application_ = new PlatformApplication("Swift");
	splitter->show();
	mainController_ = new MainController(chatWindowFactory_, rosterWindowFactory_, loginWindowFactory_, treeWidgetFactory_, settings_, application_, systemTray_);
}

QtSwift::~QtSwift() {
	delete chatWindowFactory_;
	delete rosterWindowFactory_;
	delete loginWindowFactory_;
	delete treeWidgetFactory_;
	delete mainController_;
	delete settings_;
	delete application_;
	delete systemTray_;
}

}
