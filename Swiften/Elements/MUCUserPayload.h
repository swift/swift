/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/MUC/MUCOccupant.h"

namespace Swift {
	class MUCUserPayload : public Payload, public Shared<MUCUserPayload> {
		public:
			struct Item {
				Item() : affiliation(MUCOccupant::NoAffiliation), role(MUCOccupant::NoRole) {}
				boost::optional<JID> realJID;
				boost::optional<String> nick;
				MUCOccupant::Affiliation affiliation;
				MUCOccupant::Role role;
			};

			struct StatusCode {
				StatusCode() {}
				int code;
			};

			// struct Password {

			// }

			// struct History {

			// }

			// struct Invite {

			// }

			MUCUserPayload() {
			}

			void addItem(Item item) {items_.push_back(item);}
		
			void addStatusCode(StatusCode code) {statusCodes_.push_back(code);}

			const std::vector<Item> getItems() const {return items_;}

			const std::vector<StatusCode> getStatusCodes() const {return statusCodes_;}

		private:
			std::vector<Item> items_;
			std::vector<StatusCode> statusCodes_;
	};
}
