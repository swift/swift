/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>

#include "Swift/Controllers/FileTransfer/FileTransferController.h"

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {

class ChatsManager;
class FileTransferManager;

class FileTransferOverview {
public:
	FileTransferOverview(FileTransferManager*);
	~FileTransferOverview();
	
	void sendFile(const JID&, const std::string&);
	const std::vector<FileTransferController*>& getFileTransfers() const;

	boost::signal<void (FileTransferController*)> onNewFileTransferController;

private:
	void handleIncomingFileTransfer(IncomingFileTransfer::ref transfer);

private:
	std::vector<FileTransferController*> fileTransfers;
	FileTransferManager *fileTransferManager;
};

}
