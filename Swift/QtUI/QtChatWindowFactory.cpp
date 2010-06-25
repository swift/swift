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


namespace Swift {
QtChatWindowFactory::QtChatWindowFactory(QSplitter* splitter, QtSettingsProvider* settings, QtChatTabs* tabs) {
	settings_ = settings;
	tabs_ = tabs;
	if (splitter) {
		splitter->addWidget(tabs_);
	} else if (tabs_) {
		QVariant chatTabsGeometryVariant = settings_->getQSettings()->value("chatTabsGeometry");
		if (chatTabsGeometryVariant.isValid()) {
			tabs_->restoreGeometry(chatTabsGeometryVariant.toByteArray());
		}
		connect(tabs_, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
	}
}

ChatWindow* QtChatWindowFactory::createChatWindow(const JID &contact,UIEventStream* eventStream) {
	QtChatWindow *chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), eventStream);
	if (tabs_) {
		tabs_->addTab(chatWindow);
	} else {
		QVariant chatGeometryVariant = settings_->getQSettings()->value("chatTabsGeometry");
		if (chatGeometryVariant.isValid()) {
			chatWindow->restoreGeometry(chatGeometryVariant.toByteArray());
		}
		connect(chatWindow, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
	}
	return chatWindow;
}

void QtChatWindowFactory::handleWindowGeometryChanged() {
	settings_->getQSettings()->setValue("chatTabsGeometry", qobject_cast<QWidget*>(sender())->saveGeometry());
}

}
