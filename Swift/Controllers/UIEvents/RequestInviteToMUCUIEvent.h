/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class RequestInviteToMUCUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<RequestInviteToMUCUIEvent> ref;

			RequestInviteToMUCUIEvent(const JID& room, const std::vector<JID>& JIDsToInvite) : room_(room), invite_(JIDsToInvite) {
			}

			const JID& getRoom() const {
				return room_;
			}

			const std::vector<JID> getInvites() const {
				return invite_;
			}

		private:
			JID room_;
			std::vector<JID> invite_;
	};
}
