/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/signals2.hpp>

#include <Swift/Controllers/FileTransfer/FileTransferController.h>

namespace Swift {

class ChatsManager;
class FileTransferManager;

class FileTransferOverview {
public:
    FileTransferOverview(FileTransferManager*);
    ~FileTransferOverview();

    void sendFile(const JID&, const std::string&);
    const std::vector<FileTransferController*>& getFileTransfers() const;
    void clearFinished();
    bool isClearable() const;

    boost::signals2::signal<void (FileTransferController*)> onNewFileTransferController;
    boost::signals2::signal<void ()> onFileTransferListChanged;

private:
    void handleIncomingFileTransfer(IncomingFileTransfer::ref transfer);
    void handleNewFileTransferController(FileTransferController* controller);
    void handleFileTransferStateChanged();

private:
    std::vector<FileTransferController*> fileTransfers;
    FileTransferManager *fileTransferManager;
};

}
