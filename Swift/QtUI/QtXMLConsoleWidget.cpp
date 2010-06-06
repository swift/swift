/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtXMLConsoleWidget.h"

#include <QCloseEvent>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QCheckBox>

#include "QtSwiftUtil.h"
#include "Swiften/Base/String.h"

namespace Swift {

QtXMLConsoleWidget::QtXMLConsoleWidget() {
	setWindowTitle("Console");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);

	textEdit = new QTextEdit(this);
	textEdit->setReadOnly(true);
	layout->addWidget(textEdit);

	QWidget* bottom = new QWidget(this);
	layout->addWidget(bottom);

	QHBoxLayout* buttonLayout = new QHBoxLayout(bottom);
	buttonLayout->setContentsMargins(10,0,20,0);
	buttonLayout->setSpacing(0);

	enabled = new QCheckBox("Trace input/output", bottom);
	enabled->setChecked(true);
	buttonLayout->addWidget(enabled);

	buttonLayout->addStretch();

	QPushButton* clearButton = new QPushButton("Clear", bottom);
	connect(clearButton, SIGNAL(clicked()), textEdit, SLOT(clear()));
	buttonLayout->addWidget(clearButton);

	setWindowTitle("Debug Console");
	emit titleUpdated();
}

void QtXMLConsoleWidget::showEvent(QShowEvent* event) {
	emit windowOpening();
	emit titleUpdated(); /* This just needs to be somewhere after construction */
	QWidget::showEvent(event);
}

void QtXMLConsoleWidget::show() {
	QWidget::show();
	emit windowOpening();
}

void QtXMLConsoleWidget::activate() {
	emit wantsToActivate();
}

void QtXMLConsoleWidget::closeEvent(QCloseEvent* event) {
	emit windowClosing();
	event->accept();
}

void QtXMLConsoleWidget::handleDataRead(const String& data) {
	textEdit->setTextColor(QColor(33,98,33));
	appendTextIfEnabled(data);
}

void QtXMLConsoleWidget::handleDataWritten(const String& data) {
	textEdit->setTextColor(QColor(155,1,0));
	appendTextIfEnabled(data);
}

void QtXMLConsoleWidget::appendTextIfEnabled(const String& data) {
	if (enabled->isChecked()) {
		QScrollBar* scrollBar = textEdit->verticalScrollBar();
		bool scrollToBottom = (!scrollBar || scrollBar->value() == scrollBar->maximum());
		textEdit->append(P2QSTRING(data));
		if (scrollToBottom) {
			textEdit->ensureCursorVisible();
		}
	}
}

}
