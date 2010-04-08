/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatWindowFactory.h"

#include <QDesktopWidget>

#include "QtChatTabs.h"
#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "Roster/QtTreeWidgetFactory.h"


namespace Swift {
QtChatWindowFactory::QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory, QSplitter* splitter, QtSettingsProvider* settings, QtChatTabs* tabs) : treeWidgetFactory_(treeWidgetFactory) {
	settings_ = settings;
	tabs_ = tabs;
	if (splitter) {
		splitter->addWidget(tabs_);
	} else {
		QVariant chatTabsGeometryVariant = settings_->getQSettings()->value("chatTabsGeometry");
		if (chatTabsGeometryVariant.isValid()) {
			tabs_->restoreGeometry(chatTabsGeometryVariant.toByteArray());
		}
		connect(tabs_, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
	}
}

ChatWindow* QtChatWindowFactory::createChatWindow(const JID &contact) {
	QtChatWindow *chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), treeWidgetFactory_);
	tabs_->addTab(chatWindow);
	//chatWindow->show();
	return chatWindow;
}

void QtChatWindowFactory::handleWindowGeometryChanged() {
	settings_->getQSettings()->setValue("chatTabsGeometry", tabs_->saveGeometry());
}

}
