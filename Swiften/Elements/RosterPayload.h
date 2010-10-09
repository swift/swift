/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class RosterPayload : public Payload, public Shared<RosterPayload> {
		public:
			typedef std::vector<RosterItemPayload> RosterItemPayloads;

		public:
			RosterPayload() {}

			boost::optional<RosterItemPayload> getItem(const JID& jid) const;

			void addItem(const RosterItemPayload& item) {
				items_.push_back(item);
			}

			const RosterItemPayloads& getItems() const {
				return items_;
			}

		private:
			RosterItemPayloads items_;
	};
}
