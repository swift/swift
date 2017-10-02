/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/FileTransfer/FileTransferController.h>

#include <memory>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/FileTransfer/OutgoingJingleFileTransfer.h>

#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {

FileTransferController::FileTransferController(const JID& receipient, const std::string& filename, FileTransferManager* fileTransferManager) :
    sending(true), otherParty(receipient), filename(filename), ftManager(fileTransferManager), ftProgressInfo(nullptr), chatWindow(nullptr), currentState(FileTransfer::State::WaitingForStart) {

}

FileTransferController::FileTransferController(IncomingFileTransfer::ref transfer) :
    sending(false), otherParty(transfer->getSender()), filename(transfer->getFileName()), transfer(transfer), ftManager(nullptr), ftProgressInfo(nullptr), chatWindow(nullptr), currentState(FileTransfer::State::WaitingForStart) {
    transfer->onStateChanged.connect(boost::bind(&FileTransferController::handleFileTransferStateChange, this, _1));
}

FileTransferController::~FileTransferController() {
    delete ftProgressInfo;
    transfer->onStateChanged.disconnect(boost::bind(&FileTransferController::handleFileTransferStateChange, this, _1));
}

const JID &FileTransferController::getOtherParty() const {
    return otherParty;
}

std::string FileTransferController::setChatWindow(ChatWindow* wnd, const std::string& nickname, const std::string& avatarPath) {
    chatWindow = wnd;
    if (sending) {
        uiID = wnd->addFileTransfer(QT_TRANSLATE_NOOP("", "me"), avatarPath, true, filename, boost::filesystem::file_size(boost::filesystem::path(filename)), "");
    } else {
        uiID = wnd->addFileTransfer(nickname, avatarPath, false, filename, transfer->getFileSizeInBytes(), transfer->getDescription());
    }
    return uiID;
}

void FileTransferController::setReceipient(const JID& receipient) {
    this->otherParty = receipient;
}

bool FileTransferController::isIncoming() const {
    return !sending;
}

FileTransfer::State FileTransferController::getState() const {
    return currentState;
}

int FileTransferController::getProgress() const {
    return ftProgressInfo ? ftProgressInfo->getPercentage() : 0;
}

boost::uintmax_t FileTransferController::getSize() const {
    if (transfer) {
        return transfer->getFileSizeInBytes();
    } else {
        return 0;
    }
}

void FileTransferController::start(std::string& description) {
    SWIFT_LOG(debug) << "FileTransferController::start" << std::endl;
    fileReadStream = std::make_shared<FileReadBytestream>(boost::filesystem::path(filename));
    OutgoingFileTransfer::ref outgoingTransfer = ftManager->createOutgoingFileTransfer(otherParty, boost::filesystem::path(filename), description, fileReadStream);
    if (outgoingTransfer) {
        ftProgressInfo = new FileTransferProgressInfo(outgoingTransfer->getFileSizeInBytes());
        ftProgressInfo->onProgressPercentage.connect(boost::bind(&FileTransferController::handleProgressPercentageChange, this, _1));
        outgoingTransfer->onStateChanged.connect(boost::bind(&FileTransferController::handleFileTransferStateChange, this, _1));
        outgoingTransfer->onProcessedBytes.connect(boost::bind(&FileTransferProgressInfo::setBytesProcessed, ftProgressInfo, _1));
        outgoingTransfer->start();
        transfer = outgoingTransfer;
    } else {
        std::cerr << "File transfer not supported!" << std::endl;
    }
}

void FileTransferController::accept(std::string& file) {
    SWIFT_LOG(debug) << "FileTransferController::accept" << std::endl;
    IncomingFileTransfer::ref incomingTransfer = std::dynamic_pointer_cast<IncomingFileTransfer>(transfer);
    if (incomingTransfer) {
        fileWriteStream = std::make_shared<FileWriteBytestream>(boost::filesystem::path(file));

        ftProgressInfo = new FileTransferProgressInfo(transfer->getFileSizeInBytes());
        ftProgressInfo->onProgressPercentage.connect(boost::bind(&FileTransferController::handleProgressPercentageChange, this, _1));
        transfer->onProcessedBytes.connect(boost::bind(&FileTransferProgressInfo::setBytesProcessed, ftProgressInfo, _1));
        incomingTransfer->accept(fileWriteStream);
    } else {
        std::cerr << "Expected an incoming transfer in this situation!" << std::endl;
    }
}

void FileTransferController::cancel() {
    if (transfer) {
        transfer->cancel();
    } else {
        chatWindow->setFileTransferStatus(uiID, ChatWindow::Canceled);
    }
}

void FileTransferController::handleFileTransferStateChange(FileTransfer::State state) {
    currentState = state;
    onStateChanged();
    switch(state.type) {
        case FileTransfer::State::Initial:
            assert(false);
            return;
        case FileTransfer::State::Negotiating:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::Negotiating);
            return;
        case FileTransfer::State::Transferring:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::Transferring);
            return;
        case FileTransfer::State::Canceled:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::Canceled);
            return;
        case FileTransfer::State::Finished:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::Finished);
            if (fileWriteStream) {
                fileWriteStream->close();
            }
            return;
        case FileTransfer::State::Failed:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::FTFailed);
            return;
        case FileTransfer::State::WaitingForAccept:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::WaitingForAccept);
            return;
        case FileTransfer::State::WaitingForStart:
            chatWindow->setFileTransferStatus(uiID, ChatWindow::Initialisation);
            return;
    }
    assert(false);
}

void FileTransferController::handleProgressPercentageChange(int percentage) {
    onProgressChange();
    chatWindow->setFileTransferProgress(uiID, percentage);
}

}
