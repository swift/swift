#pragma once

#include <boost/signal.hpp>

#include "Swiften/Queries/SetResponder.h"
#include "Swiften/Elements/RosterPayload.h"

namespace Swift {
	class RosterPushResponder : public SetResponder<RosterPayload> {
		public:
			RosterPushResponder(IQRouter* router) : SetResponder<RosterPayload>(router) {}

		public:
			boost::signal<void (boost::shared_ptr<RosterPayload>)> onRosterReceived;

		private:
			virtual bool handleSetRequest(const JID& from, const String& id, boost::shared_ptr<RosterPayload> payload) {
				onRosterReceived(payload);
				sendResponse(from, id, boost::shared_ptr<Payload>());
				return true;
			}
	};
}
