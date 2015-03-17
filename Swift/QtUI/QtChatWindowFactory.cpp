/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatWindowFactory.h>

#include <QDesktopWidget>
#include <qdebug.h>

#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtChatTabsBase.h>
#include <Swift/QtUI/QtChatWindow.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtChatTheme.h>
#include <Swift/QtUI/QtSingleWindow.h>


namespace Swift {

static const QString SPLITTER_STATE = "mucSplitterState";
static const QString CHAT_TABS_GEOMETRY = "chatTabsGeometry";

QtChatWindowFactory::QtChatWindowFactory(QtSingleWindow* splitter, SettingsProvider* settings, QtSettingsProvider* qtSettings, QtChatTabsBase* tabs, const QString& themePath, const std::map<std::string, std::string>& emoticons) : themePath_(themePath), emoticons_(emoticons) {
	qtOnlySettings_ = qtSettings;
	settings_ = settings;
	tabs_ = tabs;
	theme_ = NULL;
	QtChatTabs* fullTabs = dynamic_cast<QtChatTabs*>(tabs_);
	if (splitter) {
		assert(fullTabs && "Netbook mode and no-tabs interface is not supported!");
		splitter->addWidget(fullTabs);
	} else if (fullTabs) {
		QVariant chatTabsGeometryVariant = qtOnlySettings_->getQSettings()->value(CHAT_TABS_GEOMETRY);
		if (chatTabsGeometryVariant.isValid()) {
			fullTabs->restoreGeometry(chatTabsGeometryVariant.toByteArray());
		}
		connect(fullTabs, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
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

	QtChatWindow* chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), theme_, eventStream, settings_, emoticons_);
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
