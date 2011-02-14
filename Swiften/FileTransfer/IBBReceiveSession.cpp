/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/FileTransfer/IBBReceiveSession.h"

#include <boost/bind.hpp>

#include "Swiften/Queries/IQRouter.h"
#include "Swiften/FileTransfer/IBBRequest.h"
#include "Swiften/FileTransfer/BytestreamException.h"

namespace Swift {

IBBReceiveSession::IBBReceiveSession(const std::string& id, const JID& from, size_t size, WriteBytestream::ref bytestream, IQRouter* router) : SetResponder<IBB>(router), id(id), from(from), size(size), bytestream(bytestream), router(router), sequenceNumber(0), active(false), receivedSize(0) {
}

IBBReceiveSession::~IBBReceiveSession() {
}

void IBBReceiveSession::start() {
	active = true;
}

void IBBReceiveSession::stop() {
	if (active && router->isAvailable()) {
		IBBRequest::create(from, IBB::createIBBClose(id), router)->send();
	}
	finish(boost::optional<FileTransferError>());
}

void IBBReceiveSession::finish(boost::optional<FileTransferError> error) {
	active = false;
	onFinished(error);
}

bool IBBReceiveSession::handleSetRequest(const JID& from, const JID&, const std::string& id, IBB::ref ibb) {
	if (from == this->from && ibb->getStreamID() == id) {
		if (ibb->getAction() == IBB::Data) {
			if (sequenceNumber == ibb->getSequenceNumber()) {
				bytestream->write(ibb->getData());
				receivedSize += ibb->getData().getSize();
				if (receivedSize >= size) {
					if (receivedSize > size) {
						std::cerr << "Warning: Received more data than expected" << std::endl;
					}
					finish(boost::optional<FileTransferError>());
				}
			}
			else {
				sendError(from, id, ErrorPayload::NotAcceptable, ErrorPayload::Cancel);
				finish(FileTransferError(FileTransferError::ClosedError));
			}
		}
		else if (ibb->getAction() == IBB::Open) {
			sendResponse(from, id, IBB::ref());
		}
		else if (ibb->getAction() == IBB::Close) {
			sendResponse(from, id, IBB::ref());
			finish(FileTransferError(FileTransferError::ClosedError));
		}
		return true;
	}
	return false;
}

}
