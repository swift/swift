/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/FileTransfer/IBBSendSession.h>

namespace Swift {

class SWIFTEN_API IBBSendTransportSession : public TransportSession {
	public:
		IBBSendTransportSession(boost::shared_ptr<IBBSendSession> session);
		virtual ~IBBSendTransportSession();

		virtual void start() SWIFTEN_OVERRIDE;
		virtual void stop() SWIFTEN_OVERRIDE;

	private:
		boost::shared_ptr<IBBSendSession> session;
		boost::bsignals::scoped_connection finishedConnection;
		boost::bsignals::scoped_connection bytesSentConnection;
};

}
