/*
 * Copyright (c) 2010-2012 Remko Tronçon
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
#include <Swift/QtUI/QtUISettingConstants.h>

namespace Swift {

QtNameWidget::QtNameWidget(SettingsProvider* settings, QWidget *parent) : QWidget(parent), settings(settings) {
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);

	mode = settings->getSetting(QtUISettingConstants::SHOW_NICK_IN_ROSTER_HEADER) ? ShowNick : ShowJID;

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
	bool hasNick = !nick.isEmpty();

	QAction* showAsNick = new QAction(hasNick ? tr("Show Nickname") : tr("(No Nickname Set)"), this);
	showAsNick->setCheckable(true);
	showAsNick->setEnabled(hasNick);
	if (mode == ShowNick && hasNick) {
		showAsNick->setChecked(true);
	}
	menu.addAction(showAsNick);

	QAction* showAsJID = new QAction(tr("Show Address"), this);
	showAsJID->setCheckable(true);
	if (mode == ShowJID || !hasNick) {
		showAsJID->setChecked(true);
	}
	menu.addAction(showAsJID);

	QAction* editProfile = new QAction(tr("Edit Profile"), this);
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
	settings->storeSetting(QtUISettingConstants::SHOW_NICK_IN_ROSTER_HEADER, mode == ShowNick);
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




