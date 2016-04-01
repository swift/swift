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

#pragma once

#include <memory>

#include <Swift/Controllers/UIEvents/UIEventStream.h>

namespace Swift {

class FileTransferListWidgetFactory;
class FileTransferListWidget;
class FileTransferOverview;

class FileTransferListController {
public:
    FileTransferListController(UIEventStream* uiEventStream, FileTransferListWidgetFactory* fileTransferListWidgetFactory);
    ~FileTransferListController();

    void setFileTransferOverview(FileTransferOverview* overview);

private:
    void handleUIEvent(std::shared_ptr<UIEvent> event);

private:
    FileTransferListWidgetFactory* fileTransferListWidgetFactory;
    FileTransferListWidget* fileTransferListWidget;
    FileTransferOverview* fileTransferOverview;
};

}
