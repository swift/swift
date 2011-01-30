/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtNameWidget.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QtDebug>

#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtSettingsProvider.h>

namespace Swift {

QtNameWidget::QtNameWidget(QtSettingsProvider* settings, QWidget *parent) : QWidget(parent), settings(settings) {
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);

	mode = settings->getBoolSetting("showNickInRosterHeader", true) ? ShowNick : ShowJID;

	textLabel = new QtElidingLabel(this);
	QFont font = textLabel->font();
	font.setBold(true);
	textLabel->setFont(font);
	mainLayout->addWidget(textLabel);
}

void QtNameWidget::setNick(const QString& nick) {
	this->nick = nick;
	updateText();
}

void QtNameWidget::setJID(const QString& jid) {
	this->jid = jid;
	updateText();
}

void QtNameWidget::mousePressEvent(QMouseEvent* event) {
	QMenu menu;

	QAction* showAsNick = new QAction("Show nickname", this);
	showAsNick->setCheckable(true);
	if (mode == ShowNick) {
		showAsNick->setChecked(true);
	}
	menu.addAction(showAsNick);

	QAction* showAsJID = new QAction("Show ID", this);
	showAsJID->setCheckable(true);
	if (mode == ShowJID) {
		showAsJID->setChecked(true);
	}
	menu.addAction(showAsJID);

	QAction* editProfile = new QAction("Edit Profile", this);
	menu.addAction(editProfile);

	QAction* result = menu.exec(event->globalPos());
	if (result == showAsJID) {
		mode = ShowJID;
	}
	else if (result == showAsNick) {
		mode = ShowNick;
	}
	else if (result == editProfile) {
		emit onChangeNickRequest();
	}
	settings->storeBool("showNickInRosterHeader", mode == ShowNick);
	updateText();
}

void QtNameWidget::updateText() {
	QString text;
	if (mode == ShowNick && !nick.isEmpty()) {
		text = nick;
	}
	else {
		text = jid;
	}
	text.replace("<","&lt;");
	textLabel->setText(text);
}

}




