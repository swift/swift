/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/Form.h"
#include "Swiften/Base/String.h"

namespace Swift {
	/**
	 * XEP-0055 search payload.
	 */
	class SearchPayload : public Payload {
		public:
			typedef boost::shared_ptr<SearchPayload> ref;

			struct Item {
				String first;
				String last;
				String nick;
				String email;
				JID jid;
			};

			SearchPayload() {}

			Form::ref getForm() const { return form; }
			void setForm(Form::ref f) { form = f; }

			const boost::optional<String>& getInstructions() const {
				return instructions;
			}

			const boost::optional<String>& getNick() const {
				return nick;
			}

			const boost::optional<String>& getFirst() const {
				return first;
			}

			const boost::optional<String>& getLast() const {
				return last;
			}

			const boost::optional<String>& getEMail() const {
				return email;
			}

			void setInstructions(const String& v) {
				this->instructions = v;
			}

			void setNick(const String& v) {
				this->nick = v;
			}

			void setFirst(const String& v) {
				this->first = v;
			}

			void setLast(const String& v) {
				this->last = v;
			}

			void setEMail(const String& v) {
				this->email = v;
			}

			const std::vector<Item> getItems() const {
				return items;
			}

			void addItem(const Item& item) {
				items.push_back(item);
			}

		private:
			Form::ref form;
			boost::optional<String> instructions;
			boost::optional<String> nick;
			boost::optional<String> first;
			boost::optional<String> last;
			boost::optional<String> email;
			std::vector<Item> items;
	};
}
