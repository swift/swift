#pragma once

namespace Swift {
	class String;
	class JID;

	class UserRegistry {
		public:
			virtual ~UserRegistry();

			virtual bool isValidUserPassword(const JID& user, const String& password) const = 0;
	};
}
