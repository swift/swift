/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardOperation.h>

namespace Swift {
	class IQRouter;
	class ErrorPayload;
	class WhiteboardPayload;

	class SWIFTEN_API WhiteboardSession {
	public:
		typedef boost::shared_ptr<WhiteboardSession> ref;

	public:
		WhiteboardSession(const JID& jid, IQRouter* router);
		virtual ~WhiteboardSession();
		void handleIncomingAction(boost::shared_ptr<WhiteboardPayload> payload);
		void sendElement(const WhiteboardElement::ref element);
		virtual void sendOperation(WhiteboardOperation::ref operation) = 0;
		void cancel();
		const JID& getTo() const;

	public:
		boost::signal< void(const WhiteboardElement::ref element)> onElementReceived; 
		boost::signal< void(const WhiteboardOperation::ref operation)> onOperationReceived;
		boost::signal< void(const JID& contact)> onSessionTerminated;
		boost::signal< void(const JID& contact)> onRequestAccepted;
		boost::signal< void(const JID& contact)> onRequestRejected;

	private:
		virtual void handleIncomingOperation(WhiteboardOperation::ref operation) = 0;

	protected:
		void sendPayload(boost::shared_ptr<WhiteboardPayload> payload);

		JID toJID_;
		IQRouter* router_;
		std::string lastOpID;
		IDGenerator idGenerator;
	};
}
