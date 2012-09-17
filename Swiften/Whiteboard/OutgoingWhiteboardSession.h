/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Whiteboard/WhiteboardSession.h>
#include <Swiften/Whiteboard/WhiteboardServer.h>
#include <boost/shared_ptr.hpp>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API OutgoingWhiteboardSession : public WhiteboardSession {
	public:
		typedef boost::shared_ptr<OutgoingWhiteboardSession> ref;

	public:
		OutgoingWhiteboardSession(const JID& jid, IQRouter* router);
		virtual ~OutgoingWhiteboardSession();
		void startSession();

	private:
		void handleRequestResponse(boost::shared_ptr<WhiteboardPayload> /*payload*/, ErrorPayload::ref error);
		void handleIncomingOperation(WhiteboardOperation::ref operation);
		void sendOperation(WhiteboardOperation::ref operation);

		WhiteboardServer server;
	};
}
