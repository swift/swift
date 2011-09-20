/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Client/BlockList.h>

namespace Swift {
	class BlockListImpl : public BlockList {
		public:
			BlockListImpl();

			virtual State getState() const {
				return state;
			}

			void setState(State state);

			virtual const std::set<JID>& getItems() const {
				return items;
			}

			void setItems(const std::vector<JID>& items);
			void addItem(const JID& item);
			void removeItem(const JID& item);
			void addItems(const std::vector<JID>& items);
			void removeItems(const std::vector<JID>& items);
			void removeAllItems();

		private:
			State state;
			std::set<JID> items;
	};
}
