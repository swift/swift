/*
 * Copyright (c) 2015 Utkarsh Dixit
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */
#include "QtXMLMsgSender.h"

#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/Message.h"

#include <string>

#include <QCloseEvent>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QComboBox>
#include <QSignalMapper>
#include <QLineEdit>
#include <QLabel>
#include <Swiften/Base/format.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtXMLMsgSender::QtXMLMsgSender(XMLConsoleController* consoleController) : consoleController_(consoleController) {
	QWidget::setWindowTitle(tr("Message Sender"));

	signalMapper = new QSignalMapper(this);

	layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);

	sendToAddress = new QLineEdit(this);
	QLabel *jidLabel = new QLabel(sendToAddress);
	jidLabel->setText(tr("Send To JID : "));
	layout->addWidget(jidLabel);
	layout->addWidget(sendToAddress);

	textEdit = new QTextEdit(this);
	textLabel = new QLabel(textEdit);
	textLabel->setText(tr("Raw XML : "));
	textEdit->setReadOnly(false);
	layout->addWidget(textLabel);
	layout->addWidget(textEdit);

	comboBox = new QComboBox(this);
	comboBox->addItem("--- SELECT ---",0);
	comboBox->addItem("Iq Stanza",1);
	comboBox->addItem("Presence Stanza",2);
	comboBox->addItem("Message Stanza",3);
	
	connect(comboBox,SIGNAL(activated(const int &)),this,SLOT(attributesSelector(const int &)));

	layout->addWidget(comboBox);
		
	bottom = new QWidget(this);
	layout->addWidget(bottom);
	bottom->setAutoFillBackground(true);

	buttonLayout = new QHBoxLayout(bottom);
	buttonLayout->setContentsMargins(10,0,20,0);
	buttonLayout->setSpacing(0);

	sendButton = NULL;
	typeChooser = NULL;
	QWidget::setWindowTitle(tr("XML Stanza Sender"));
	emit titleUpdated();
}

QtXMLMsgSender::~QtXMLMsgSender() {
}

void QtXMLMsgSender::showEvent(QShowEvent* event) {
	emit windowOpening();
	emit titleUpdated(); /* This just needs to be somewhere after construction */
	QWidget::showEvent(event);
}

void QtXMLMsgSender::show() {
	QWidget::show();
	emit windowOpening();
}


void QtXMLMsgSender::activate() {
	emit wantsToActivate();
}

void QtXMLMsgSender::closeEvent(QCloseEvent* event) {
	emit windowClosing();
	event->accept();
}

void QtXMLMsgSender::attributesSelector(const int &id) {
	if(sendButton != NULL) {
		buttonLayout->removeWidget(sendButton);
		delete sendButton;
		sendButton = NULL;
	}
	if(typeChooser != NULL) {
		buttonLayout->removeWidget(typeChooser);
		delete typeChooser;
		typeChooser = NULL;
	}
	switch(id) {
		case 0:
			break;
		case 1:
			sendButton = new QPushButton(tr("Send IQ"),bottom);
			connect(sendButton,SIGNAL(clicked()),this,SLOT(sendRawIqStanza()));
			textLabel->setText(tr("Enter Raw XML :"));
			buttonLayout->addWidget(sendButton);

			typeChooser = new QComboBox(this);
			typeChooser->addItem("Get",0);
			typeChooser->addItem("Set",1);
			typeChooser->addItem("Result",2);
			typeChooser->addItem("Error",3);
			buttonLayout->addWidget(typeChooser);
			break;

		case 2:
			sendButton = new QPushButton(tr("Send Presence"),bottom);
			connect(sendButton,SIGNAL(clicked()),this,SLOT(sendRawPresenceStanza()));
			textLabel->setText(tr("Enter Status :"));
			buttonLayout->addWidget(sendButton);
		
			typeChooser = new QComboBox(this);
			typeChooser->addItem("Available",0);
			typeChooser->addItem("Error",1);
			typeChooser->addItem("Probe",2);
			typeChooser->addItem("Subscribe",3);
			typeChooser->addItem("Subscribed",4);
			typeChooser->addItem("Unavailable",5);
			typeChooser->addItem("Unsubscribe",6);
			typeChooser->addItem("Unsubscribed",7);
			buttonLayout->addWidget(typeChooser);
			break;
		case 3:
			sendButton = new QPushButton(tr("Send Message"),bottom);
			connect(sendButton,SIGNAL(clicked()),this,SLOT(sendRawMessageStanza()));
			textLabel->setText(tr("Enter Raw XML :"));
			buttonLayout->addWidget(sendButton);
			typeChooser = new QComboBox(this);
			typeChooser->addItem("Normal",0);
			typeChooser->addItem("Chat",1);
			typeChooser->addItem("Error",2);
			typeChooser->addItem("Groupchat",3);
			typeChooser->addItem("Headline",4);
			buttonLayout->addWidget(typeChooser);
			break;
	}
}

void QtXMLMsgSender::sendRawIqStanza() {
	/**
	 * Work left on adding an iq stanza.Control side is more or less complete though
	 * Only ned to connect the IQRouter in XMLConsoleController to the signal that is emitted here.
	*/
		
}
void QtXMLMsgSender::sendRawPresenceStanza() {
	Presence::ref presence = Presence::create();
	int typeId = typeChooser->currentIndex();

	switch(typeId) {
		case 0:
			presence->setType(presence->Available);
			break;
		case 1:
			presence->setType(presence->Error);
			break;
		case 2:
			presence->setType(presence->Probe);
			break;
		case 3:
			presence->setType(presence->Subscribe);
			break;
		case 4:
			presence->setType(presence->Subscribed);
			break;
		case 5:
			presence->setType(presence->Unavailable);
			break;
		case 6:
			presence->setType(presence->Unsubscribe);
			break;
		case 7:
			presence->setType(presence->Unsubscribed);
			break;

	}
		presence->setTo(Q2PSTRING(sendToAddress->text()));
		presence->setStatus(Q2PSTRING(textEdit->toPlainText()));
		consoleController_->sendRawPresencePayload(presence);
}
void QtXMLMsgSender::sendRawMessageStanza() {
	Message::ref message = boost::make_shared<Message>();
	int typeId = typeChooser->currentIndex();

	switch(typeId) {
		case 0:
			message->setType(message->Normal);
			break;
		case 1:
			message->setType(message->Chat);
			break;
		case 2:
			message->setType(message->Error);
			break;
		case 3:
			message->setType(message->Groupchat);
			break;
		case 4:
			message->setType(message->Headline);
			break;

	}
		message->setTo(Q2PSTRING(sendToAddress->text()));
		message->updatePayload(boost::make_shared<Body>(Q2PSTRING(textEdit->toPlainText())));
		consoleController_->sendRawMessagePayload(message);

}

void QtXMLMsgSender::handleDataRead(const SafeByteArray& data) {
}

void QtXMLMsgSender::handleDataWritten(const SafeByteArray& data) {
}
std::string QtXMLMsgSender::getID() const {
	return "QtXMLMsgSender";
}

}
