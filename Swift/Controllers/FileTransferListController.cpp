/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swift/Controllers/FileTransferListController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/UIInterfaces/FileTransferListWidgetFactory.h"
#include "Swift/Controllers/UIEvents/RequestFileTransferListUIEvent.h"

namespace Swift {

FileTransferListController::FileTransferListController(UIEventStream* uiEventStream, FileTransferListWidgetFactory* fileTransferListWidgetFactory) : fileTransferListWidgetFactory(fileTransferListWidgetFactory), fileTransferListWidget(NULL), fileTransferOverview(0) {
	uiEventStream->onUIEvent.connect(boost::bind(&FileTransferListController::handleUIEvent, this, _1));
}

FileTransferListController::~FileTransferListController() {
	delete fileTransferListWidget;
}

void FileTransferListController::setFileTransferOverview(FileTransferOverview *overview) {
	fileTransferOverview = overview;
	if (fileTransferListWidget) {
		fileTransferListWidget->setFileTransferOverview(fileTransferOverview);
	}
}

void FileTransferListController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestFileTransferListUIEvent> event = boost::dynamic_pointer_cast<RequestFileTransferListUIEvent>(rawEvent);
	if (event != NULL) {
		if (fileTransferListWidget == NULL) {
			fileTransferListWidget = fileTransferListWidgetFactory->createFileTransferListWidget();
			if (fileTransferOverview) {
				fileTransferListWidget->setFileTransferOverview(fileTransferOverview);
			}
		}
		fileTransferListWidget->show();
		fileTransferListWidget->activate();
	}
}

}
