/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEventStream.h"

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
	void handleUIEvent(boost::shared_ptr<UIEvent> event);

private:
	FileTransferListWidgetFactory* fileTransferListWidgetFactory;
	FileTransferListWidget* fileTransferListWidget;
	FileTransferOverview* fileTransferOverview;
};

}
