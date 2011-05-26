/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "FileTransferController.h"
#include "Swiften/FileTransfer/OutgoingJingleFileTransfer.h"
#include "Swiften/FileTransfer/FileTransferManager.h"
#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/bind.hpp>
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

FileTransferController::FileTransferController(const JID& receipient, const std::string& filename, FileTransferManager* fileTransferManager) :
	sending(true), otherParty(receipient), filename(filename), ftManager(fileTransferManager), ftProgressInfo(0), chatWindow(0), currentState(FileTransfer::State::WaitingForStart) {
	
}

FileTransferController::FileTransferController(IncomingFileTransfer::ref transfer) :
	sending(false), otherParty(transfer->getSender()), filename(transfer->filename), transfer(transfer), ftManager(0), ftProgressInfo(0), chatWindow(0), currentState(FileTransfer::State::WaitingForStart) {

}

FileTransferController::~FileTransferController() {
	delete ftProgressInfo;
}

const JID &FileTransferController::getOtherParty() const {
	return otherParty;
}

std::string FileTransferController::setChatWindow(ChatWindow* wnd, std::string nickname) {
	chatWindow = wnd;
	if (sending) {
		uiID = wnd->addFileTransfer("me", true, filename, boost::filesystem::file_size(boost::filesystem::path(filename)));
	} else {
		uiID = wnd->addFileTransfer(nickname, false, filename, transfer->fileSizeInBytes);
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
		return transfer->fileSizeInBytes;
	} else {
		return 0;
	}
}

void FileTransferController::start(std::string& description) {
	std::cout << "FileTransferController::start" << std::endl;
	fileReadStream = boost::make_shared<FileReadBytestream>(boost::filesystem::path(filename));
	OutgoingFileTransfer::ref outgoingTransfer = ftManager->createOutgoingFileTransfer(otherParty, boost::filesystem::path(filename), description, fileReadStream);
	if (outgoingTransfer) {
		ftProgressInfo = new FileTransferProgressInfo(outgoingTransfer->fileSizeInBytes);
		ftProgressInfo->onProgressPercentage.connect(boost::bind(&FileTransferController::handleProgressPercentageChange, this, _1));
		outgoingTransfer->onStateChange.connect(boost::bind(&FileTransferController::handleFileTransferStateChange, this, _1));
		outgoingTransfer->onProcessedBytes.connect(boost::bind(&FileTransferProgressInfo::setBytesProcessed, ftProgressInfo, _1));
		outgoingTransfer->start();
		transfer = outgoingTransfer;
	} else {
		std::cerr << "File transfer not supported!" << std::endl;
	}
}

void FileTransferController::accept(std::string& file) {
	std::cout << "FileTransferController::accept" << std::endl;
	IncomingFileTransfer::ref incomingTransfer = boost::dynamic_pointer_cast<IncomingFileTransfer>(transfer);
	if (incomingTransfer) {
		fileWriteStream = boost::make_shared<FileWriteBytestream>(boost::filesystem::path(file));

		ftProgressInfo = new FileTransferProgressInfo(transfer->fileSizeInBytes);
		ftProgressInfo->onProgressPercentage.connect(boost::bind(&FileTransferController::handleProgressPercentageChange, this, _1));
		transfer->onStateChange.connect(boost::bind(&FileTransferController::handleFileTransferStateChange, this, _1));
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
	onStateChage();
	switch(state.state) {
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
			return;
	}
	std::cerr << "Unhandled FileTransfer::State!" << std::endl;
}

void FileTransferController::handleProgressPercentageChange(int percentage) {
	onProgressChange();
	chatWindow->setFileTransferProgress(uiID, percentage);
}

}
