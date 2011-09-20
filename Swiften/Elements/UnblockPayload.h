/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class UnblockPayload : public Payload {
		public:
			UnblockPayload() {
			}

			void addItem(const JID& item) {
				items.push_back(item);
			}

			const std::vector<JID>& getItems() const {
				return items;
			}

		private:
			std::vector<JID> items;
	};
}
