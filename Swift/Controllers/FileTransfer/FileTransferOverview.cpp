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

#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/FileTransfer/FileTransferManager.h>

namespace Swift {

FileTransferOverview::FileTransferOverview(FileTransferManager* ftm) : fileTransferManager(ftm) {
    fileTransferManager->onIncomingFileTransfer.connect(boost::bind(&FileTransferOverview::handleIncomingFileTransfer, this, _1));
    onNewFileTransferController.connect(boost::bind(&FileTransferOverview::handleNewFileTransferController, this, _1));
}

FileTransferOverview::~FileTransferOverview() {
    onNewFileTransferController.disconnect(boost::bind(&FileTransferOverview::handleNewFileTransferController, this, _1));
    fileTransferManager->onIncomingFileTransfer.disconnect(boost::bind(&FileTransferOverview::handleIncomingFileTransfer, this, _1));
    for (auto controller : fileTransfers) {
        controller->onStateChanged.disconnect(boost::bind(&FileTransferOverview::handleFileTransferStateChanged, this));
    }
}

void FileTransferOverview::sendFile(const JID& jid, const std::string& filename) {
    if (boost::filesystem::exists(filename) && boost::filesystem::file_size(filename) > 0) {
        FileTransferController* controller = new FileTransferController(jid, filename, fileTransferManager);
        onNewFileTransferController(controller);
    }
}

void FileTransferOverview::handleIncomingFileTransfer(IncomingFileTransfer::ref transfer) {
    FileTransferController* controller = new FileTransferController(transfer);
    onNewFileTransferController(controller);
}

void FileTransferOverview::handleNewFileTransferController(FileTransferController* controller) {
    fileTransfers.push_back(controller);
    controller->onStateChanged.connect(boost::bind(&FileTransferOverview::handleFileTransferStateChanged, this));
}

void FileTransferOverview::handleFileTransferStateChanged() {
    onFileTransferListChanged();
}

const std::vector<FileTransferController*>& FileTransferOverview::getFileTransfers() const {
    return fileTransfers;
}

void FileTransferOverview::clearFinished() {
    for (std::vector<FileTransferController*>::iterator it = fileTransfers.begin(); it != fileTransfers.end(); ) {
        if((*it)->getState().type == FileTransfer::State::Finished
            || (*it)->getState().type == FileTransfer::State::Failed
            || (*it)->getState().type == FileTransfer::State::Canceled) {
            FileTransferController* controller = *it;
            it = fileTransfers.erase(it);
            controller->onStateChanged.disconnect(boost::bind(&FileTransferOverview::handleFileTransferStateChanged, this));
            delete controller;
        } else {
            ++it;
        }
    }
    onFileTransferListChanged();
}

bool FileTransferOverview::isClearable() const {
    bool isClearable = false;
    for (auto controller : fileTransfers) {
        if(controller->getState().type == FileTransfer::State::Finished
            || controller->getState().type == FileTransfer::State::Failed
            || controller->getState().type == FileTransfer::State::Canceled) {
            isClearable = true;
            break;
        }
    }
    return isClearable;
}

}
