/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <map>

#include <Swiften/Base/API.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>
#include <Swiften/Whiteboard/IncomingWhiteboardSession.h>
#include <Swiften/Whiteboard/OutgoingWhiteboardSession.h>

namespace Swift {
	class IQRouter;
	class WhiteboardResponder;
	class PresenceOracle;
	class EntityCapsProvider;

	class SWIFTEN_API WhiteboardSessionManager {
		friend class WhiteboardResponder;
	public:
		WhiteboardSessionManager(IQRouter* router, StanzaChannel* stanzaChannel, PresenceOracle* presenceOracle, EntityCapsProvider* capsProvider);
		~WhiteboardSessionManager();

		WhiteboardSession::ref getSession(const JID& to);
		WhiteboardSession::ref requestSession(const JID& to);

	public:
		boost::signal< void (IncomingWhiteboardSession::ref)> onSessionRequest;

	private:
		JID getFullJID(const JID& bareJID);
		OutgoingWhiteboardSession::ref createOutgoingSession(const JID& to);
		void handleIncomingSession(IncomingWhiteboardSession::ref session);
		void handlePresenceReceived(Presence::ref presence);
		void handleAvailableChanged(bool available);
		void deleteSessionEntry(const JID& contact);

	private:
		std::map<JID, boost::shared_ptr<WhiteboardSession> > sessions_;
		IQRouter* router_;
		StanzaChannel* stanzaChannel_;
		PresenceOracle* presenceOracle_;
		EntityCapsProvider* capsProvider_;
		WhiteboardResponder* responder;
	};
}
