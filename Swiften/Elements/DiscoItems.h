/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <algorithm>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class DiscoItems : public Payload {
		public:
			class Item {
				public:
					Item(const String& name, const JID& jid, const String& node="") : name_(name), jid_(jid), node_(node) {
					}

					const String& getName() const {
						return name_;
					}

					const String& getNode() const {
						return node_;
					}					

					const JID& getJID() const {
						return jid_;
					}

				private:
					String name_;
					JID jid_;
					String node_;
			};

			DiscoItems() {
			}

			const String& getNode() const {
				return node_;
			}

			void setNode(const String& node) {
				node_ = node;
			}

			const std::vector<Item>& getItems() const {
				return items_;
			}

			void addItem(const Item& item) {
				items_.push_back(item);
			}

		private:
			String node_;
			std::vector<Item> items_;
	};
}
