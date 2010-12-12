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

namespace Swift {

QtNameWidget::QtNameWidget(QWidget *parent) : QWidget(parent), mode(ShowNick) {
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);

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

	QAction* result = menu.exec(event->globalPos());
	if (result == showAsJID) {
		mode = ShowJID;
	}
	else {
		mode = ShowNick;
	}
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




