/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/FileTransferListController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/UIEvents/RequestFileTransferListUIEvent.h>
#include <Swift/Controllers/UIInterfaces/FileTransferListWidgetFactory.h>

namespace Swift {

FileTransferListController::FileTransferListController(UIEventStream* uiEventStream, FileTransferListWidgetFactory* fileTransferListWidgetFactory) : fileTransferListWidgetFactory(fileTransferListWidgetFactory), fileTransferListWidget(nullptr), fileTransferOverview(nullptr) {
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

void FileTransferListController::handleUIEvent(std::shared_ptr<UIEvent> rawEvent) {
    std::shared_ptr<RequestFileTransferListUIEvent> event = std::dynamic_pointer_cast<RequestFileTransferListUIEvent>(rawEvent);
    if (event != nullptr) {
        if (fileTransferListWidget == nullptr) {
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
