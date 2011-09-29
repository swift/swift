/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IBBReceiveSession.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/FileTransfer/IBBRequest.h>
#include <Swiften/FileTransfer/BytestreamException.h>
#include <Swiften/Queries/SetResponder.h>

#include <cassert>

namespace Swift {

class IBBReceiveSession::IBBResponder : public SetResponder<IBB> {
	public:
		IBBResponder(IBBReceiveSession* session, IQRouter* router) : SetResponder<IBB>(router), session(session), sequenceNumber(0), receivedSize(0) {
		}

		virtual bool handleSetRequest(const JID& from, const JID&, const std::string& id, IBB::ref ibb) {
			if (from == session->from && ibb->getStreamID() == session->id) {
				if (ibb->getAction() == IBB::Data) {
					if (sequenceNumber == ibb->getSequenceNumber()) {
						session->onDataReceived(ibb->getData());
						receivedSize += ibb->getData().size();
						sequenceNumber++;
						sendResponse(from, id, IBB::ref());
						if (receivedSize >= session->size) {
							if (receivedSize > session->size) {
								std::cerr << "Warning: Received more data than expected" << std::endl;
							}
							session->finish(boost::optional<FileTransferError>());
						}
					}
					else {
						SWIFT_LOG(warning) << "Received data out of order" << std::endl;
						sendError(from, id, ErrorPayload::NotAcceptable, ErrorPayload::Cancel);
						session->finish(FileTransferError(FileTransferError::ClosedError));
					}
				}
				else if (ibb->getAction() == IBB::Open) {
					SWIFT_LOG(debug) << "IBB open received" << std::endl;
					sendResponse(from, id, IBB::ref());
				}
				else if (ibb->getAction() == IBB::Close) {
					SWIFT_LOG(debug) << "IBB close received" << std::endl;
					sendResponse(from, id, IBB::ref());
					session->finish(FileTransferError(FileTransferError::ClosedError));
				}
				return true;
			}
			SWIFT_LOG(debug) << "wrong from/sessionID: " << from << " == " << session->from << " / " <<ibb->getStreamID() << " == " << session->id << std::endl;
			return false;
		}

	private:
		IBBReceiveSession* session;
		int sequenceNumber;
		size_t receivedSize;
};


IBBReceiveSession::IBBReceiveSession(
		const std::string& id, 
		const JID& from, 
		const JID& to, 
		size_t size, 
		IQRouter* router) : 
			id(id), 
			from(from), 
			to(to), 
			size(size), 
			router(router), 
			active(false) {
	assert(!id.empty());
	assert(from.isValid());
	responder = new IBBResponder(this, router);
}

IBBReceiveSession::~IBBReceiveSession() {
	if (active) {
		SWIFT_LOG(warning) << "Session still active" << std::endl;
	}
	delete responder;
}

void IBBReceiveSession::start() {
	SWIFT_LOG(debug) << "receive session started" << std::endl;
	active = true;
	responder->start();
}

void IBBReceiveSession::stop() {
	SWIFT_LOG(debug) << "receive session stopped" << std::endl;
	responder->stop();
	if (active) {
		if (router->isAvailable()) {
			IBBRequest::create(to, from, IBB::createIBBClose(id), router)->send();
		}
		finish(boost::optional<FileTransferError>());
	}
}

void IBBReceiveSession::finish(boost::optional<FileTransferError> error) {
	active = false;
	onFinished(error);
}

}
