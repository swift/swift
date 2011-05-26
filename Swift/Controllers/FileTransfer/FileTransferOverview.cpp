/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "FileTransferOverview.h"

#include <boost/bind.hpp>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/FileTransfer/FileTransferManager.h>

namespace Swift {

FileTransferOverview::FileTransferOverview(FileTransferManager* ftm) : fileTransferManager(ftm) {
	fileTransferManager->onIncomingFileTransfer.connect(boost::bind(&FileTransferOverview::handleIncomingFileTransfer, this, _1));
}

FileTransferOverview::~FileTransferOverview() {
	
}
	
void FileTransferOverview::sendFile(const JID& jid, const std::string& filename) {
	FileTransferController* controller = new FileTransferController(jid, filename, fileTransferManager);
	fileTransfers.push_back(controller);
	
	onNewFileTransferController(controller);
}

void FileTransferOverview::handleIncomingFileTransfer(IncomingFileTransfer::ref transfer) {
	FileTransferController* controller = new FileTransferController(transfer);
	fileTransfers.push_back(controller);
	onNewFileTransferController(controller);
}

const std::vector<FileTransferController*>& FileTransferOverview::getFileTransfers() const {
	return fileTransfers;
}

}
