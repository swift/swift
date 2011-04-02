/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <map>

#include <Swiften/JID/JID.h>

namespace Swift {
	class XMPPURI {
		public:
			XMPPURI();

			const JID& getAuthority() const {
				return authority;
			}

			void setAuthority(const JID& j) {
				authority = j;
			}

			const JID& getPath() const {
				return path;
			}

			void setPath(const JID& j) {
				path = j;
			}

			const std::string& getQueryType() const {
				return queryType;
			}

			void setQueryType(const std::string& q) {
				queryType = q;
			}

			const std::map<std::string, std::string>& getQueryParameters() const {
				return queryParameters;
			}

			void addQueryParameter(const std::string& key, const std::string& path) {
				queryParameters[key] = path;
			}

			const std::string& getFragment() const {
				return fragment;
			}

			void setFragment(const std::string& f) {
				fragment = f;
			}

			bool isNull() const {
				return !authority.isValid() && !path.isValid();
			}

			static XMPPURI fromString(const std::string&);

		private:
			JID authority;
			JID path;
			std::string fragment;
			std::string queryType;
			std::map<std::string, std::string> queryParameters;
	};
}
