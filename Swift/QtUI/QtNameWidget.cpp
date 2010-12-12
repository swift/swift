/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtNameWidget.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtLineEdit.h>

namespace Swift {

QtNameWidget::QtNameWidget(QWidget *parent) : QWidget(parent) {
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);

	stack = new QStackedWidget(this);
	mainLayout->addWidget(stack);

	textLabel = new QtElidingLabel(this);
	QFont font = textLabel->font();
	font.setBold(true);
	textLabel->setFont(font);
	stack->addWidget(textLabel);

	nickEdit = new QtLineEdit(this);
	connect(nickEdit, SIGNAL(returnPressed()), this, SLOT(handleEditComplete()));
	connect(nickEdit, SIGNAL(escapePressed()), this, SLOT(handleEditCancelled()));
	stack->addWidget(nickEdit);
}

void QtNameWidget::mouseDoubleClickEvent(QMouseEvent* event) {
	if (stack->currentWidget() != nickEdit) {
		if (event->button() == Qt::LeftButton) {
			nickEdit->setText(nick);
			stack->setCurrentWidget(nickEdit);
		}
	}
}

void QtNameWidget::handleEditComplete() {
	stack->setCurrentWidget(textLabel);
	emit onChangeNickRequest(nickEdit->text());
}

void QtNameWidget::handleEditCancelled() {
	stack->setCurrentWidget(textLabel);
}

void QtNameWidget::setNick(const QString& nick) {
	this->nick = nick;
	updateText();
}

void QtNameWidget::setJID(const QString& jid) {
	this->jid = jid;
	updateText();
}

void QtNameWidget::updateText() {
	QString text;
	if (nick.isEmpty()) {
		text = jid;
	}
	else {
		text = nick + " (" + jid + ")";
	}
	text.replace("<","&lt;");
	textLabel->setText(text);
}

}




