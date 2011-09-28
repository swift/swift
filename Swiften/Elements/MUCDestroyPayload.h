/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class MUCDestroyPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCDestroyPayload> ref;

			MUCDestroyPayload() {
			}

			void setNewVenue(const JID& jid) {
				newVenue_ = jid;
			}

			const JID& getNewVenue() const {
				return newVenue_;
			}

			void setReason(const std::string& reason) {
				reason_ = reason;
			}

			const std::string& getReason() const {
				return reason_;
			}

		private:
			JID newVenue_;
			std::string reason_;
	};
}
