/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/FileTransfer/OutgoingFileTransfer.h"

#include <boost/bind.hpp>

#include "Swiften/FileTransfer/StreamInitiationRequest.h"
#include "Swiften/FileTransfer/BytestreamsRequest.h"
#include "Swiften/FileTransfer/SOCKS5BytestreamServer.h"
#include "Swiften/FileTransfer/IBBSendSession.h"

namespace Swift {

OutgoingFileTransfer::OutgoingFileTransfer(const std::string& id, const JID& from, const JID& to, const std::string& name, int size, const std::string& description, boost::shared_ptr<ReadBytestream> bytestream, IQRouter* iqRouter, SOCKS5BytestreamServer* socksServer) : id(id), from(from), to(to), name(name), size(size), description(description), bytestream(bytestream), iqRouter(iqRouter), socksServer(socksServer) {
}

void OutgoingFileTransfer::start() {
	StreamInitiation::ref streamInitiation(new StreamInitiation());
	streamInitiation->setID(id);
	streamInitiation->setFileInfo(StreamInitiationFileInfo(name, description, size));
	//streamInitiation->addProvidedMethod("http://jabber.org/protocol/bytestreams");
	streamInitiation->addProvidedMethod("http://jabber.org/protocol/ibb");
	StreamInitiationRequest::ref request = StreamInitiationRequest::create(to, streamInitiation, iqRouter);
	request->onResponse.connect(boost::bind(&OutgoingFileTransfer::handleStreamInitiationRequestResponse, this, _1, _2));
	request->send();
}

void OutgoingFileTransfer::stop() {
}

void OutgoingFileTransfer::handleStreamInitiationRequestResponse(StreamInitiation::ref response, ErrorPayload::ref error) {
	if (error) {
		finish(FileTransferError());
	}
	else {
		if (response->getRequestedMethod() == "http://jabber.org/protocol/bytestreams") {
			socksServer->addBytestream(id, from, to, bytestream); 
			Bytestreams::ref bytestreams(new Bytestreams());
			bytestreams->setStreamID(id);
			HostAddressPort addressPort = socksServer->getAddressPort();
			bytestreams->addStreamHost(Bytestreams::StreamHost(addressPort.getAddress().toString(), from, addressPort.getPort()));
			BytestreamsRequest::ref request = BytestreamsRequest::create(to, bytestreams, iqRouter);
			request->onResponse.connect(boost::bind(&OutgoingFileTransfer::handleBytestreamsRequestResponse, this, _1, _2));
			request->send();
		}
		else if (response->getRequestedMethod() == "http://jabber.org/protocol/ibb") {
			ibbSession = boost::shared_ptr<IBBSendSession>(new IBBSendSession(id, to, bytestream, iqRouter));
			ibbSession->onFinished.connect(boost::bind(&OutgoingFileTransfer::handleIBBSessionFinished, this, _1));
			ibbSession->start();
		}
	}
}

void OutgoingFileTransfer::handleBytestreamsRequestResponse(Bytestreams::ref, ErrorPayload::ref error) {
	if (error) {
		finish(FileTransferError());
	}
	//socksServer->onTransferFinished.connect();
}

void OutgoingFileTransfer::finish(boost::optional<FileTransferError> error) {
	if (ibbSession) {
		ibbSession->onFinished.disconnect(boost::bind(&OutgoingFileTransfer::handleIBBSessionFinished, this, _1));
		ibbSession.reset();
	}
	socksServer->removeBytestream(id, from, to); 
	onFinished(error);
}

void OutgoingFileTransfer::handleIBBSessionFinished(boost::optional<FileTransferError> error) {
	finish(error);
}

}
