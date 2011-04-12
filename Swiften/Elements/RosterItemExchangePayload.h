/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>
#include <string>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Payload.h"
#include "Swiften/JID/JID.h"


namespace Swift {
	class RosterItemExchangePayload : public Payload {
		public:
			typedef boost::shared_ptr<RosterItemExchangePayload> ref;

			enum Action { Add, Modify, Delete };

			struct Item {
				Action action;
				JID jid;
				std::string name;
				std::vector<std::string> groups;
			};

			typedef std::vector<RosterItemExchangePayload::Item> RosterItemExchangePayloadItems;

		public:
			RosterItemExchangePayload();

			void addItem(const RosterItemExchangePayload::Item& item) {
				items_.push_back(item);
			}

			const RosterItemExchangePayloadItems& getItems() const {
				return items_;
			}

		private:
			RosterItemExchangePayloadItems items_;
	};
}
