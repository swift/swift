/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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

			enum ImpromptuMode {
				Impromptu,
				NotImpromptu
			};

			RequestInviteToMUCUIEvent(const JID& room, const std::vector<JID>& JIDsToInvite, ImpromptuMode impromptu) : room_(room), invite_(JIDsToInvite) {
				isImpromptu_ = impromptu == Impromptu;
			}

			const JID& getRoom() const {
				return room_;
			}

			const std::vector<JID> getInvites() const {
				return invite_;
			}

			bool isImpromptu() const {
				return isImpromptu_;
			}

		private:
			JID room_;
			std::vector<JID> invite_;
			bool isImpromptu_;
	};
}
