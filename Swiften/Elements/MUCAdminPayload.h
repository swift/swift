/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/MUCItem.h>

namespace Swift {
	class SWIFTEN_API MUCAdminPayload : public Payload {
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
