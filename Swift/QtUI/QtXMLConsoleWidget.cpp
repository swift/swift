/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "QtXMLConsoleWidget.h"
#include "QtXMLMsgSender.h"
#include <string>

#include <QCloseEvent>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QCheckBox>
#include <QComboBox>
#include <Swiften/Base/format.h>
#include <Swift/Controllers/UIEvents/RequestXMLMsgConsoleUIEvent.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtXMLConsoleWidget::QtXMLConsoleWidget(UIEventStream* uiEventStream) {
	uiEventStream_ = uiEventStream;
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
	
	buttonLayout->addStretch();
	QPushButton* sendMsgButton = new QPushButton(tr("Send Message"), bottom);
	connect(sendMsgButton, SIGNAL(clicked()),this, SLOT(launchXMLMsgWindow()));
	buttonLayout->addWidget(sendMsgButton);

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

void QtXMLConsoleWidget::launchXMLMsgWindow() {
	uiEventStream_->send(boost::make_shared<RequestXMLMsgConsoleUIEvent>());
}

void QtXMLConsoleWidget::activate() {
	emit wantsToActivate();
}

void QtXMLConsoleWidget::closeEvent(QCloseEvent* event) {
	emit windowClosing();
	event->accept();
}

void QtXMLConsoleWidget::handleDataRead(const SafeByteArray& data) {
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	std::string tag = Q2PSTRING(tr("<!-- IN %1 -->").arg(P2QSTRING(boost::posix_time::to_iso_extended_string(now))));
	appendTextIfEnabled(tag + "\n" + safeByteArrayToString(data) + "\n", QColor(33,98,33));
}

void QtXMLConsoleWidget::handleDataWritten(const SafeByteArray& data) {
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	std::string tag = Q2PSTRING(tr("<!-- OUT %1 -->").arg(P2QSTRING(boost::posix_time::to_iso_extended_string(now))));
	appendTextIfEnabled(tag + "\n" + safeByteArrayToString(data) + "\n", QColor(155,1,0));
}

std::string QtXMLConsoleWidget::getID() const {
	return "QtXMLConsoleWidget";
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

		// Checking for the scrollbar again, because it could have appeared after inserting text.
		// In practice, I suspect that the scrollbar is always there, but hidden, but since we were
		// explicitly testing for this already above, I'm leaving the code in.
		scrollBar = textEdit->verticalScrollBar();
		if (scrollToBottom && scrollBar) {
			scrollBar->setValue(scrollBar->maximum());
		}
	}
}




}



