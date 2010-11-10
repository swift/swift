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
	class InBandRegistrationPayload : public Payload {
		public:
			typedef boost::shared_ptr<InBandRegistrationPayload> ref;

			InBandRegistrationPayload() : registered(false) {}

			Form::ref getForm() const { return form; }
			void setForm(Form::ref f) { form = f; }

			bool isRegistered() const {
				return registered;
			}

			void setRegistered(bool b) {
				registered = b;
			}

			const boost::optional<String>& getInstructions() const {
				return instructions;
			}

			const boost::optional<String>& getUsername() const {
				return username;
			}

			const boost::optional<String>& getNick() const {
				return nick;
			}

			const boost::optional<String>& getPassword() const {
				return password;
			}

			const boost::optional<String>& getName() const {
				return name;
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

			const boost::optional<String>& getAddress() const {
				return address;
			}

			const boost::optional<String>& getCity() const {
				return city;
			}

			const boost::optional<String>& getState() const {
				return state;
			}

			const boost::optional<String>& getZip() const {
				return zip;
			}

			const boost::optional<String>& getPhone() const {
				return phone;
			}

			const boost::optional<String>& getURL() const {
				return url;
			}

			const boost::optional<String>& getDate() const {
				return date;
			}

			const boost::optional<String>& getMisc() const {
				return misc;
			}

			const boost::optional<String>& getText() const {
				return text;
			}

			const boost::optional<String>& getKey() const {
				return key;
			}

			void setInstructions(const String& v) {
				this->instructions = v;
			}

			void setUsername(const String& v) {
				this->username = v;
			}

			void setNick(const String& v) {
				this->nick = v;
			}

			void setPassword(const String& v) {
				this->password = v;
			}

			void setName(const String& v) {
				this->name = v;
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

			void setAddress(const String& v) {
				this->address = v;
			}

			void setCity(const String& v) {
				this->city = v;
			}

			void setState(const String& v) {
				this->state = v;
			}

			void setZip(const String& v) {
				this->zip = v;
			}

			void setPhone(const String& v) {
				this->phone = v;
			}

			void setURL(const String& v) {
				this->url = v;
			}

			void setDate(const String& v) {
				this->date = v;
			}

			void setMisc(const String& v) {
				this->misc = v;
			}

			void setText(const String& v) {
				this->text = v;
			}

			void setKey(const String& v) {
				this->key = v;
			}


		private:
			Form::ref form;
			bool registered;
			boost::optional<String> instructions;
			boost::optional<String> username;
			boost::optional<String> nick;
			boost::optional<String> password;
			boost::optional<String> name;
			boost::optional<String> first;
			boost::optional<String> last;
			boost::optional<String> email;
			boost::optional<String> address;
			boost::optional<String> city;
			boost::optional<String> state;
			boost::optional<String> zip;
			boost::optional<String> phone;
			boost::optional<String> url;
			boost::optional<String> date;
			boost::optional<String> misc;
			boost::optional<String> text;
			boost::optional<String> key;
	};
}
