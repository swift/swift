/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IBBSendSession.h>

#include <boost/bind.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/FileTransfer/IBBRequest.h>
#include <Swiften/FileTransfer/BytestreamException.h>

namespace Swift {

IBBSendSession::IBBSendSession(const std::string& id, const JID& from, const JID& to, boost::shared_ptr<ReadBytestream> bytestream, IQRouter* router) : id(id), from(from), to(to), bytestream(bytestream), router(router), blockSize(4096), sequenceNumber(0), active(false), waitingForData(false) {
	bytestream->onDataAvailable.connect(boost::bind(&IBBSendSession::handleDataAvailable, this));
}

IBBSendSession::~IBBSendSession() {
	bytestream->onDataAvailable.disconnect(boost::bind(&IBBSendSession::handleDataAvailable, this));
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
			sendMoreData();
		}
		else {
			finish(boost::optional<FileTransferError>());
		}
	}
	else {
		finish(FileTransferError(FileTransferError::PeerError));
	}
}

void IBBSendSession::sendMoreData() {
	try {
		boost::shared_ptr<ByteArray> data = bytestream->read(blockSize);
		if (!data->empty()) {
			waitingForData = false;
			IBBRequest::ref request = IBBRequest::create(from, to, IBB::createIBBData(id, sequenceNumber, *data), router);
			sequenceNumber++;
			request->onResponse.connect(boost::bind(&IBBSendSession::handleIBBResponse, this, _1, _2));
			request->send();
			onBytesSent(data->size());
		}
		else {
			waitingForData = true;
		}
	}
	catch (const BytestreamException&) {
		finish(FileTransferError(FileTransferError::ReadError));
	}
}

void IBBSendSession::finish(boost::optional<FileTransferError> error) {
	active = false;
	onFinished(error);
}

void IBBSendSession::handleDataAvailable() {
	if (waitingForData) {
		sendMoreData();
	}
}

}
