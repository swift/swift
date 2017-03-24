/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtXMLSenderWidget.h>

#include <string>
#include <QString>

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
		connect(sendXMLButton, SIGNAL(clicked()), textEdit, SLOT(clear()));
		this->resize(640,400);
		this->setLayout(layout); // I'm aware that this is really shitty code.
	};

	QtXMLSenderWidget::~QtXMLSenderWidget() {

	}

}