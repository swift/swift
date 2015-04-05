/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Queries/IQRouter.h>
#include "Swift/Controllers/XMLConsoleController.h"
#include "Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h"
#include "Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestXMLMsgConsoleUIEvent.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Presence/StanzaChannelPresenceSender.h"
#include "Swiften/Presence/DirectedPresenceSender.h"
namespace Swift {

XMLConsoleController::XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory) : xmlConsoleWidgetFactory(xmlConsoleWidgetFactory), xmlConsoleWidget(NULL), xmlConsoleMsgWidget(NULL) {
	uiEventStream_ = uiEventStream;
	clientStanzaChannel_ = NULL;
	stanzaChannelPresenceSender_ = NULL;
	stanzaChannelDirectedPresenceSender_ = NULL;
	uiEventStream_->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIEvent, this, _1));
	uiEventStream_->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIMsgEvent, this, _1));
}

void XMLConsoleController::setClientStanzaChannel(StanzaChannel* clientStanzaChannel, IQRouter* clientIQRouter) {
	clientStanzaChannel_ = clientStanzaChannel;
	stanzaChannelPresenceSender_ = new StanzaChannelPresenceSender(clientStanzaChannel_);
	stanzaChannelDirectedPresenceSender_ = new DirectedPresenceSender(stanzaChannelPresenceSender_);
	this->sendRawPresencePayload.connect(boost::bind(&DirectedPresenceSender::sendPresence,stanzaChannelDirectedPresenceSender_,_1));
	this->sendRawMessagePayload.connect(boost::bind(&StanzaChannel::sendMessage,clientStanzaChannel_,_1));
}
XMLConsoleController::XMLConsoleController() {
}
XMLConsoleController::~XMLConsoleController() {
	delete xmlConsoleWidget;
	delete xmlConsoleMsgWidget;
}

void XMLConsoleController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestXMLConsoleUIEvent> event = boost::dynamic_pointer_cast<RequestXMLConsoleUIEvent>(rawEvent);
	if (event != NULL) {
		if (xmlConsoleWidget == NULL) {
			xmlConsoleWidget = xmlConsoleWidgetFactory->createXMLConsoleWidget(uiEventStream_);
		}
		xmlConsoleWidget->show();
		xmlConsoleWidget->activate();
	}
}

void XMLConsoleController::handleUIMsgEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestXMLMsgConsoleUIEvent> event = boost::dynamic_pointer_cast<RequestXMLMsgConsoleUIEvent>(rawEvent);
	if (event != NULL) {
		if(xmlConsoleMsgWidget == NULL) {
			xmlConsoleMsgWidget = xmlConsoleWidgetFactory->createXMLConsoleMsgWidget(this);
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
