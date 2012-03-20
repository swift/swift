/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/OutgoingSIFileTransfer.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/FileTransfer/StreamInitiationRequest.h>
#include <Swiften/FileTransfer/BytestreamsRequest.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/FileTransfer/IBBSendSession.h>

namespace Swift {

OutgoingSIFileTransfer::OutgoingSIFileTransfer(const std::string& id, const JID& from, const JID& to, const std::string& name, int size, const std::string& description, boost::shared_ptr<ReadBytestream> bytestream, IQRouter* iqRouter, SOCKS5BytestreamServer* socksServer) : id(id), from(from), to(to), name(name), size(size), description(description), bytestream(bytestream), iqRouter(iqRouter), socksServer(socksServer) {
}

void OutgoingSIFileTransfer::start() {
	StreamInitiation::ref streamInitiation(new StreamInitiation());
	streamInitiation->setID(id);
	streamInitiation->setFileInfo(StreamInitiationFileInfo(name, description, size));
	//streamInitiation->addProvidedMethod("http://jabber.org/protocol/bytestreams");
	streamInitiation->addProvidedMethod("http://jabber.org/protocol/ibb");
	StreamInitiationRequest::ref request = StreamInitiationRequest::create(to, streamInitiation, iqRouter);
	request->onResponse.connect(boost::bind(&OutgoingSIFileTransfer::handleStreamInitiationRequestResponse, this, _1, _2));
	request->send();
}

void OutgoingSIFileTransfer::stop() {
}

void OutgoingSIFileTransfer::handleStreamInitiationRequestResponse(StreamInitiation::ref response, ErrorPayload::ref error) {
	if (error) {
		finish(FileTransferError());
	}
	else {
		if (response->getRequestedMethod() == "http://jabber.org/protocol/bytestreams") {
			socksServer->addReadBytestream(id, from, to, bytestream);
			Bytestreams::ref bytestreams(new Bytestreams());
			bytestreams->setStreamID(id);
			HostAddressPort addressPort = socksServer->getAddressPort();
			bytestreams->addStreamHost(Bytestreams::StreamHost(addressPort.getAddress().toString(), from, addressPort.getPort()));
			BytestreamsRequest::ref request = BytestreamsRequest::create(to, bytestreams, iqRouter);
			request->onResponse.connect(boost::bind(&OutgoingSIFileTransfer::handleBytestreamsRequestResponse, this, _1, _2));
			request->send();
		}
		else if (response->getRequestedMethod() == "http://jabber.org/protocol/ibb") {
			ibbSession = boost::make_shared<IBBSendSession>(id, from, to, bytestream, iqRouter);
			ibbSession->onFinished.connect(boost::bind(&OutgoingSIFileTransfer::handleIBBSessionFinished, this, _1));
			ibbSession->start();
		}
	}
}

void OutgoingSIFileTransfer::handleBytestreamsRequestResponse(Bytestreams::ref, ErrorPayload::ref error) {
	if (error) {
		finish(FileTransferError());
	}
	//socksServer->onTransferFinished.connect();
}

void OutgoingSIFileTransfer::finish(boost::optional<FileTransferError> error) {
	if (ibbSession) {
		ibbSession->onFinished.disconnect(boost::bind(&OutgoingSIFileTransfer::handleIBBSessionFinished, this, _1));
		ibbSession.reset();
	}
	socksServer->removeReadBytestream(id, from, to);
	onFinished(error);
}

void OutgoingSIFileTransfer::handleIBBSessionFinished(boost::optional<FileTransferError> error) {
	finish(error);
}

}
