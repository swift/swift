/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Form.h>
#include <string>

namespace Swift {
	/**
	 * XEP-0055 search payload.
	 */
	class SearchPayload : public Payload {
		public:
			typedef boost::shared_ptr<SearchPayload> ref;

			struct Item {
				std::string first;
				std::string last;
				std::string nick;
				std::string email;
				JID jid;
			};

			SearchPayload() {}

			Form::ref getForm() const { return form; }
			void setForm(Form::ref f) { form = f; }

			const boost::optional<std::string>& getInstructions() const {
				return instructions;
			}

			const boost::optional<std::string>& getNick() const {
				return nick;
			}

			const boost::optional<std::string>& getFirst() const {
				return first;
			}

			const boost::optional<std::string>& getLast() const {
				return last;
			}

			const boost::optional<std::string>& getEMail() const {
				return email;
			}

			void setInstructions(const std::string& v) {
				this->instructions = v;
			}

			void setNick(const std::string& v) {
				this->nick = v;
			}

			void setFirst(const std::string& v) {
				this->first = v;
			}

			void setLast(const std::string& v) {
				this->last = v;
			}

			void setEMail(const std::string& v) {
				this->email = v;
			}

			const std::vector<Item>& getItems() const {
				return items;
			}

			void addItem(const Item& item) {
				items.push_back(item);
			}

		private:
			Form::ref form;
			boost::optional<std::string> instructions;
			boost::optional<std::string> nick;
			boost::optional<std::string> first;
			boost::optional<std::string> last;
			boost::optional<std::string> email;
			std::vector<Item> items;
	};
}
