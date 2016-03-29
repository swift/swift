/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/XMLConsoleController.h>

#include <Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h>
#include <Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h>

namespace Swift {

XMLConsoleController::XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory) : xmlConsoleWidgetFactory(xmlConsoleWidgetFactory), xmlConsoleWidget(NULL) {
	uiEventStream->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIEvent, this, _1));
}

XMLConsoleController::~XMLConsoleController() {
	delete xmlConsoleWidget;
}

void XMLConsoleController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestXMLConsoleUIEvent> event = boost::dynamic_pointer_cast<RequestXMLConsoleUIEvent>(rawEvent);
	if (event != NULL) {
		if (xmlConsoleWidget == NULL) {
			xmlConsoleWidget = xmlConsoleWidgetFactory->createXMLConsoleWidget();
		}
		xmlConsoleWidget->show();
		xmlConsoleWidget->activate();
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
