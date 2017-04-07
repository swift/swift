/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/XMLConsoleController.h>

#include <Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h>
#include <Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h>

namespace Swift {

XMLConsoleController::XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory) : xmlConsoleWidgetFactory(xmlConsoleWidgetFactory), xmlConsoleWidget(nullptr) {
    uiEventStream->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIEvent, this, _1));
}

XMLConsoleController::~XMLConsoleController() {
    delete xmlConsoleWidget;
}


void XMLConsoleController::handleUIEvent(std::shared_ptr<UIEvent> rawEvent) {
    std::shared_ptr<RequestXMLConsoleUIEvent> event = std::dynamic_pointer_cast<RequestXMLConsoleUIEvent>(rawEvent);
    if (event != nullptr) {
        if (xmlConsoleWidget == nullptr) {
            xmlConsoleWidget = xmlConsoleWidgetFactory->createXMLConsoleWidget();
        }
        xmlConsoleWidget->show();
        xmlConsoleWidget->activate();
        xmlConsoleWidget->onXMLSend.connect(boost::bind(&XMLConsoleController::sendXML, this, _1));
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

void XMLConsoleController::setClient(std::shared_ptr<Client> client) {
    client_ = client;
}

void XMLConsoleController::sendXML(const std::string& data){
    if(client_) {
        client_->sendData(data);
    }
}

}
