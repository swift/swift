/*
 * Copyright (c) 2017 Barun Parruck.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtXMLSenderWidget.h>

#include <string>

#include <QPushButton>
#include <QCloseEvent>
#include <QTextEdit>
#include <QVBoxLayout>

#include <Swiften/Base/format.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

	QtXMLSenderWidget::QtXMLSenderWidget() {
		setWindowTitle(tr("XML Sender"));
		QVBoxLayout* layout = new QVBoxLayout;
		QPushButton* sendXMLButton = new QPushButton(tr("Send XML"));
		textEdit = new QTextEdit;
		layout->addWidget(textEdit);
		layout->addWidget(sendXMLButton);
		connect(sendXMLButton, SIGNAL(clicked()), this, SLOT(readXML())); // Whats going wrong here?
		this->resize(640,400);
		this->setLayout(layout);
	};

	QtXMLSenderWidget::~QtXMLSenderWidget() {

	}

	std::string QtXMLSenderWidget::readXML() {
		return textEdit->toPlainText().toStdString();
	}
}