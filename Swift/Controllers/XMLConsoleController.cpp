#include "Swift/Controllers/XMLConsoleController.h"

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h"

#include "Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h"

namespace Swift {
XMLConsoleController::XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory) {
	uiEventStream_ = uiEventStream;
	xmlConsoleWidgetFactory_ = xmlConsoleWidgetFactory;
	uiEventStream_->onUIEvent.connect(boost::bind(&XMLConsoleController::handleUIEvent, this, _1));
	xmlConsoleWidget_ = NULL;
}

void XMLConsoleController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestXMLConsoleUIEvent> event = boost::dynamic_pointer_cast<RequestXMLConsoleUIEvent>(rawEvent);
	if (event != NULL) {
		if (xmlConsoleWidget_ == NULL) {
			xmlConsoleWidget_ = xmlConsoleWidgetFactory_->createXMLConsoleWidget();
		}
		xmlConsoleWidget_->show();
		xmlConsoleWidget_->activate();
	}
}

}
