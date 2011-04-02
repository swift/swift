/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class RequestJoinMUCUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<RequestJoinMUCUIEvent> ref;

			RequestJoinMUCUIEvent(const JID& room = JID()) : room(room) {
			}

			const JID& getRoom() const {
				return room;
			}

		private:
			JID room;
	};
}
