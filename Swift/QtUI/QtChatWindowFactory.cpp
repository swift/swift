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
#include "QtChatTheme.h"
#include <qdebug.h>


namespace Swift {

static const QString SPLITTER_STATE = "mucSplitterState";
static const QString CHAT_TABS_GEOMETRY = "chatTabsGeometry";

QtChatWindowFactory::QtChatWindowFactory(QSplitter* splitter, SettingsProvider* settings, QtSettingsProvider* qtSettings, QtChatTabs* tabs, const QString& themePath) : themePath_(themePath) {
	qtOnlySettings_ = qtSettings;
	settings_ = settings;
	tabs_ = tabs;
	theme_ = NULL;
	if (splitter) {
		splitter->addWidget(tabs_);
	} else if (tabs_) {
		QVariant chatTabsGeometryVariant = qtOnlySettings_->getQSettings()->value(CHAT_TABS_GEOMETRY);
		if (chatTabsGeometryVariant.isValid()) {
			tabs_->restoreGeometry(chatTabsGeometryVariant.toByteArray());
		}
		connect(tabs_, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
	}
}

QtChatWindowFactory::~QtChatWindowFactory() {
	delete theme_;
}

ChatWindow* QtChatWindowFactory::createChatWindow(const JID &contact,UIEventStream* eventStream) {
	if (!theme_) {
		theme_ = new QtChatTheme(themePath_);
		if (theme_->getIncomingContent().isEmpty()) {
			delete theme_;
			theme_ = new QtChatTheme(""); /* Use the inbuilt theme */
		}
	}

	QtChatWindow *chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), theme_, eventStream, settings_);
	connect(chatWindow, SIGNAL(splitterMoved()), this, SLOT(handleSplitterMoved()));
	connect(this, SIGNAL(changeSplitterState(QByteArray)), chatWindow, SLOT(handleChangeSplitterState(QByteArray)));

	QVariant splitterState = qtOnlySettings_->getQSettings()->value(SPLITTER_STATE);
	if(splitterState.isValid()) {
		chatWindow->handleChangeSplitterState(splitterState.toByteArray());
	}

	if (tabs_) {
		tabs_->addTab(chatWindow);
	} else {
		QVariant chatGeometryVariant = qtOnlySettings_->getQSettings()->value(CHAT_TABS_GEOMETRY);
		if (chatGeometryVariant.isValid()) {
			chatWindow->restoreGeometry(chatGeometryVariant.toByteArray());
		}
		connect(chatWindow, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
	}
	return chatWindow;
}

void QtChatWindowFactory::handleWindowGeometryChanged() {
	qtOnlySettings_->getQSettings()->setValue(CHAT_TABS_GEOMETRY, qobject_cast<QWidget*>(sender())->saveGeometry());
}

void QtChatWindowFactory::handleSplitterMoved() {
	QByteArray splitterState = qobject_cast<QtChatWindow*>(sender())->getSplitterState();
	qtOnlySettings_->getQSettings()->setValue(SPLITTER_STATE, QVariant(splitterState));
	emit changeSplitterState(splitterState);
}

}
