/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IBBSendSession.h>

#include <boost/bind.hpp>

#include <Swiften/Queries/IQRouter.h>
#include <Swiften/FileTransfer/IBBRequest.h>
#include <Swiften/FileTransfer/BytestreamException.h>

namespace Swift {

IBBSendSession::IBBSendSession(const std::string& id, const JID& from, const JID& to, boost::shared_ptr<ReadBytestream> bytestream, IQRouter* router) : id(id), from(from), to(to), bytestream(bytestream), router(router), blockSize(4096), sequenceNumber(0), active(false) {
}

IBBSendSession::~IBBSendSession() {
}

void IBBSendSession::start() {
	IBBRequest::ref request = IBBRequest::create(from, to, IBB::createIBBOpen(id, blockSize), router);
	request->onResponse.connect(boost::bind(&IBBSendSession::handleIBBResponse, this, _1, _2));
	active = true;
	request->send();
}

void IBBSendSession::stop() {
	if (active && router->isAvailable()) {
		IBBRequest::create(from, to, IBB::createIBBClose(id), router)->send();
	}
	finish(boost::optional<FileTransferError>());
}

void IBBSendSession::handleIBBResponse(IBB::ref, ErrorPayload::ref error) {
	if (!error && active) {
		if (!bytestream->isFinished()) {
			try {
				std::vector<unsigned char> data = bytestream->read(blockSize);
				IBBRequest::ref request = IBBRequest::create(from, to, IBB::createIBBData(id, sequenceNumber, data), router);
				sequenceNumber++;
				request->onResponse.connect(boost::bind(&IBBSendSession::handleIBBResponse, this, _1, _2));
				request->send();
				onBytesSent(data.size());
			}
			catch (const BytestreamException&) {
				finish(FileTransferError(FileTransferError::ReadError));
			}
		}
		else {
			finish(boost::optional<FileTransferError>());
		}
	}
	else {
		finish(FileTransferError(FileTransferError::PeerError));
	}
}

void IBBSendSession::finish(boost::optional<FileTransferError> error) {
	active = false;
	onFinished(error);
}

}
