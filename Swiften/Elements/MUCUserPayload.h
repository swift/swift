/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/MUCOccupant.h"

namespace Swift {
	class MUCUserPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCUserPayload> ref;

			struct Item {
				Item(MUCOccupant::Affiliation affiliation = MUCOccupant::NoAffiliation, MUCOccupant::Role role = MUCOccupant::NoRole) : affiliation(affiliation), role(role) {}
				boost::optional<JID> realJID;
				boost::optional<std::string> nick;
				MUCOccupant::Affiliation affiliation;
				MUCOccupant::Role role;
			};

			struct StatusCode {
				StatusCode() : code(0) {}
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
