/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "Swift/Controllers/XMLConsoleController.h"

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h"
#include "Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestXMLMsgConsoleUIEvent.h"
#include "Swift/Controllers/XMLSendRawPayload.h"
#include "Swiften/Client/StanzaChannel.h"
namespace Swift {

XMLConsoleController::XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory) : xmlConsoleWidgetFactory(xmlConsoleWidgetFactory), xmlConsoleWidget(NULL), xmlConsoleMsgWidget(NULL) {
	uiEventStream_ = uiEventStream;
	uiEventStream_->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIEvent, this, _1));
	uiEventStream_->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIMsgEvent, this, _1));
}

void XMLConsoleController::setClientStanzaChannel(StanzaChannel* clientStanzaChannel) {
	clientStanzaChannel_ = clientStanzaChannel;	
	this->sendRawMessagePayload.connect(boost::bind(&StanzaChannel::sendMessage,clientStanzaChannel_,_1));
	this->sendRawPresencePayload.connect(boost::bind(&StanzaChannel::sendPresence,clientStanzaChannel_,_1));
}
XMLConsoleController::XMLConsoleController() {
}
XMLConsoleController::~XMLConsoleController() {
	delete xmlConsoleWidget;
}
void XMLConsoleController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestXMLConsoleUIEvent> event = boost::dynamic_pointer_cast<RequestXMLConsoleUIEvent>(rawEvent);
	if (event != NULL) {
		if (xmlConsoleWidget ==NULL) {
			xmlConsoleWidget = xmlConsoleWidgetFactory->createXMLConsoleWidget(uiEventStream_);
		}
		xmlConsoleWidget->show();
		xmlConsoleWidget->activate();
	}
}

void XMLConsoleController::handleUIMsgEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestXMLMsgConsoleUIEvent> event = boost::dynamic_pointer_cast<RequestXMLMsgConsoleUIEvent>(rawEvent);
	if (event != NULL) {
		if (xmlConsoleMsgWidget == NULL) {
			xmlConsoleMsgWidget = xmlConsoleWidgetFactory->createXMLConsoleMsgWidget();
		}
		xmlConsoleMsgWidget->show();
		xmlConsoleMsgWidget->activate();
	}
}

void XMLConsoleController::handleDataRead(const SafeByteArray& data) {
	if (xmlConsoleWidget) {
		xmlConsoleWidget->handleDataRead(data);
	}
}

void XMLConsoleController::handleDataWritten(const SafeByteArray& data) {
	if (xmlConsoleWidget) {
		xmlConsoleWidget->handleDataWritten(data);
	}
}

}
