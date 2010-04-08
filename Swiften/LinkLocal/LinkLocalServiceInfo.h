/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {

	class LinkLocalServiceInfo {
		public:
			enum Status { Available, Away, DND };

			LinkLocalServiceInfo() : status(Available) {}

			const String& getFirstName() const { return firstName; }
			void setFirstName(const String& f) { firstName = f; }

			const String& getLastName() const { return lastName; }
			void setLastName(const String& l) { lastName = l; }

			const String& getEMail() const { return email; }
			void setEMail(const String& e) { email = e; }

			const JID& getJID() const { return jid; }
			void setJID(const JID& j) { jid = j; }

			const String& getMessage() const { return message; }
			void setMessage(const String& m) { message = m; }

			const String& getNick() const { return nick; }
			void setNick(const String& n) { nick = n; }

			Status getStatus() const { return status; }
			void setStatus(Status s) { status = s; }

			boost::optional<int> getPort() const { return port; }
			void setPort(int p) { port = p; }

			ByteArray toTXTRecord() const;

			static LinkLocalServiceInfo createFromTXTRecord(const ByteArray& record);

		private:
			static ByteArray getEncoded(const String&);
			static std::pair<String,String> readEntry(const ByteArray&, size_t*);

		private:
			String firstName;
			String lastName;
			String email;
			JID jid;
			String message;
			String nick;
			Status status;
			boost::optional<int> port;
	};
}
