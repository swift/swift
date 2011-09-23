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

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/MUCItem.h>

namespace Swift {
	class MUCAdminPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCAdminPayload> ref;


			MUCAdminPayload() {
			}

			void addItem(const MUCItem& item) {items_.push_back(item);}

			const std::vector<MUCItem>& getItems() const {return items_;}

		private:
			std::vector<MUCItem> items_;
	};
}
