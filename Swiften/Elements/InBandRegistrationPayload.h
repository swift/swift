/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <string>

#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Form.h>

namespace Swift {
	class InBandRegistrationPayload : public Payload {
		public:
			typedef boost::shared_ptr<InBandRegistrationPayload> ref;

			InBandRegistrationPayload() : registered(false), remove(false) {}

			Form::ref getForm() const { return form; }
			void setForm(Form::ref f) { form = f; }

			bool isRegistered() const {
				return registered;
			}

			void setRegistered(bool b) {
				registered = b;
			}

			bool isRemove() const {
				return remove;
			}

			void setRemove(bool b) {
				remove = b;
			}

			const boost::optional<std::string>& getInstructions() const {
				return instructions;
			}

			const boost::optional<std::string>& getUsername() const {
				return username;
			}

			const boost::optional<std::string>& getNick() const {
				return nick;
			}

			const boost::optional<std::string>& getPassword() const {
				return password;
			}

			const boost::optional<std::string>& getName() const {
				return name;
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

			const boost::optional<std::string>& getAddress() const {
				return address;
			}

			const boost::optional<std::string>& getCity() const {
				return city;
			}

			const boost::optional<std::string>& getState() const {
				return state;
			}

			const boost::optional<std::string>& getZip() const {
				return zip;
			}

			const boost::optional<std::string>& getPhone() const {
				return phone;
			}

			const boost::optional<std::string>& getURL() const {
				return url;
			}

			const boost::optional<std::string>& getDate() const {
				return date;
			}

			const boost::optional<std::string>& getMisc() const {
				return misc;
			}

			const boost::optional<std::string>& getText() const {
				return text;
			}

			const boost::optional<std::string>& getKey() const {
				return key;
			}

			void setInstructions(const std::string& v) {
				this->instructions = v;
			}

			void setUsername(const std::string& v) {
				this->username = v;
			}

			void setNick(const std::string& v) {
				this->nick = v;
			}

			void setPassword(const std::string& v) {
				this->password = v;
			}

			void setName(const std::string& v) {
				this->name = v;
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

			void setAddress(const std::string& v) {
				this->address = v;
			}

			void setCity(const std::string& v) {
				this->city = v;
			}

			void setState(const std::string& v) {
				this->state = v;
			}

			void setZip(const std::string& v) {
				this->zip = v;
			}

			void setPhone(const std::string& v) {
				this->phone = v;
			}

			void setURL(const std::string& v) {
				this->url = v;
			}

			void setDate(const std::string& v) {
				this->date = v;
			}

			void setMisc(const std::string& v) {
				this->misc = v;
			}

			void setText(const std::string& v) {
				this->text = v;
			}

			void setKey(const std::string& v) {
				this->key = v;
			}

		private:
			Form::ref form;
			bool registered;
			bool remove;
			boost::optional<std::string> instructions;
			boost::optional<std::string> username;
			boost::optional<std::string> nick;
			boost::optional<std::string> password;
			boost::optional<std::string> name;
			boost::optional<std::string> first;
			boost::optional<std::string> last;
			boost::optional<std::string> email;
			boost::optional<std::string> address;
			boost::optional<std::string> city;
			boost::optional<std::string> state;
			boost::optional<std::string> zip;
			boost::optional<std::string> phone;
			boost::optional<std::string> url;
			boost::optional<std::string> date;
			boost::optional<std::string> misc;
			boost::optional<std::string> text;
			boost::optional<std::string> key;
	};
}
