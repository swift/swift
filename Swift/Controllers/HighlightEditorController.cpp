/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/bind.hpp>

#include <Swift/Controllers/HighlightEditorController.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWidget.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWidgetFactory.h>
#include <Swift/Controllers/UIEvents/RequestHighlightEditorUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

namespace Swift {

HighlightEditorController::HighlightEditorController(UIEventStream* uiEventStream, HighlightEditorWidgetFactory* highlightEditorWidgetFactory, HighlightManager* highlightManager) : highlightEditorWidgetFactory_(highlightEditorWidgetFactory), highlightEditorWidget_(NULL), highlightManager_(highlightManager)
{
	uiEventStream->onUIEvent.connect(boost::bind(&HighlightEditorController::handleUIEvent, this, _1));
}

HighlightEditorController::~HighlightEditorController()
{
	delete highlightEditorWidget_;
	highlightEditorWidget_ = NULL;
}

void HighlightEditorController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent)
{
	boost::shared_ptr<RequestHighlightEditorUIEvent> event = boost::dynamic_pointer_cast<RequestHighlightEditorUIEvent>(rawEvent);
	if (event) {
		if (!highlightEditorWidget_) {
			highlightEditorWidget_ = highlightEditorWidgetFactory_->createHighlightEditorWidget();
			highlightEditorWidget_->setHighlightManager(highlightManager_);
		}
		highlightEditorWidget_->show();
	}
}

}
