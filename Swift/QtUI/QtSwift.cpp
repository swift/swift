#include "QtSwift.h"

#include "QtLoginWindowFactory.h"
#include "QtChatWindowFactory.h"
#include "QtMainWindowFactory.h"
#include "QtTreeWidgetFactory.h"

#include <boost/bind.hpp>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/Platform/PlatformApplication.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Client/Client.h"
#include "Swift/Controllers/ChatController.h"
#include "Swift/Controllers/MainController.h"

namespace Swift{

QtSwift::QtSwift() : 
	treeWidgetFactory_(new QtTreeWidgetFactory()), 
	chatWindowFactory_(new QtChatWindowFactory(treeWidgetFactory_)),
	rosterWindowFactory_(new QtMainWindowFactory(treeWidgetFactory_)),
	loginWindowFactory_(new QtLoginWindowFactory()) {
	QCoreApplication::setApplicationName("Swift");
	QCoreApplication::setOrganizationName("Swift");
	QCoreApplication::setOrganizationDomain("swift.im");
	settings_ = new QtSettingsProvider();
	application_ = new PlatformApplication("Swift");
	mainController_ = new MainController(chatWindowFactory_, rosterWindowFactory_, loginWindowFactory_, treeWidgetFactory_, settings_, application_);
}

QtSwift::~QtSwift() {
	delete chatWindowFactory_;
	delete rosterWindowFactory_;
	delete loginWindowFactory_;
	delete treeWidgetFactory_;
	delete mainController_;
	delete settings_;
	delete application_;
}

}
