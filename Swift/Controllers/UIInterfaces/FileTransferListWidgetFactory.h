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

#include <Swift/Controllers/UIInterfaces/FileTransferListWidget.h>

namespace Swift {

class FileTransferListWidgetFactory {
public:
    virtual ~FileTransferListWidgetFactory() {}

    virtual FileTransferListWidget* createFileTransferListWidget() = 0;
};

}
