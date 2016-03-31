/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/boost_bsignals.h>

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

    boost::signal<void (FileTransferController*)> onNewFileTransferController;
    boost::signal<void ()> onFileTransferListChanged;

private:
    void handleIncomingFileTransfer(IncomingFileTransfer::ref transfer);
    void handleNewFileTransferController(FileTransferController* controller);
    void handleFileTransferStateChanged();

private:
    std::vector<FileTransferController*> fileTransfers;
    FileTransferManager *fileTransferManager;
};

}
