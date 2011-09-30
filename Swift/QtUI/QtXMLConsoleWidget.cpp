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
#include <string>

namespace Swift {

QtXMLConsoleWidget::QtXMLConsoleWidget() {
	setWindowTitle(tr("Console"));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);

	textEdit = new QTextEdit(this);
	textEdit->setReadOnly(true);
	layout->addWidget(textEdit);

	QWidget* bottom = new QWidget(this);
	layout->addWidget(bottom);
	bottom->setAutoFillBackground(true);

	QHBoxLayout* buttonLayout = new QHBoxLayout(bottom);
	buttonLayout->setContentsMargins(10,0,20,0);
	buttonLayout->setSpacing(0);

	enabled = new QCheckBox(tr("Trace input/output"), bottom);
	enabled->setChecked(true);
	buttonLayout->addWidget(enabled);

	buttonLayout->addStretch();

	QPushButton* clearButton = new QPushButton(tr("Clear"), bottom);
	connect(clearButton, SIGNAL(clicked()), textEdit, SLOT(clear()));
	buttonLayout->addWidget(clearButton);

	setWindowTitle(tr("Debug Console"));
	emit titleUpdated();
}

QtXMLConsoleWidget::~QtXMLConsoleWidget() {
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

void QtXMLConsoleWidget::handleDataRead(const SafeByteArray& data) {
	appendTextIfEnabled(std::string(tr("<!-- IN -->").toUtf8()) + "\n" + safeByteArrayToString(data) + "\n", QColor(33,98,33));
}

void QtXMLConsoleWidget::handleDataWritten(const SafeByteArray& data) {
	appendTextIfEnabled(std::string(tr("<!-- OUT -->").toUtf8()) + "\n" + safeByteArrayToString(data) + "\n", QColor(155,1,0));
}

void QtXMLConsoleWidget::appendTextIfEnabled(const std::string& data, const QColor& color) {
	if (enabled->isChecked()) {
		QScrollBar* scrollBar = textEdit->verticalScrollBar();
		bool scrollToBottom = (!scrollBar || scrollBar->value() == scrollBar->maximum());

		QTextCursor cursor(textEdit->document());
		cursor.beginEditBlock();
		cursor.movePosition(QTextCursor::End);
		QTextCharFormat format;
		format.setForeground(QBrush(color));
		cursor.mergeCharFormat(format);
		cursor.insertText(P2QSTRING(data));
		cursor.endEditBlock();

		if (scrollToBottom) {
			scrollBar->setValue(scrollBar->maximum());
		}
	}
}

}
