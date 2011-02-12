/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"

namespace Swift {
	class JID {
		public:
			enum CompareType {
				WithResource, WithoutResource
			};

			explicit JID(const String& = String());
			explicit JID(const char*);
			JID(const String& node, const String& domain);
			JID(const String& node, const String& domain, const String& resource);

			bool isValid() const {
				return !domain_.isEmpty(); /* FIXME */
			}

			const String& getNode() const {
				return node_;
			}
			const String& getDomain() const {
				return domain_;
			}
			const String& getResource() const {
				return resource_;
			}
			bool isBare() const {
				return !hasResource_;
			}

			JID toBare() const {
				JID result(*this);
				result.hasResource_ = false;
				result.resource_ = "";
				return result;
			}

			String toString() const;

			bool equals(const JID& o, CompareType compareType) const {
				return compare(o, compareType) == 0;
			}

			int compare(const JID& o, CompareType compareType) const;

			operator String() const {
				return toString();
			}

			bool operator<(const Swift::JID& b) const {
				return compare(b, Swift::JID::WithResource) < 0;
			}

			friend std::ostream& operator<<(std::ostream& os, const Swift::JID& j) {
				os << j.toString();
				return os;
			}

			friend bool operator==(const Swift::JID& a, const Swift::JID& b) {
				return a.compare(b, Swift::JID::WithResource) == 0;
			}

			friend bool operator!=(const Swift::JID& a, const Swift::JID& b) {
				return a.compare(b, Swift::JID::WithResource) != 0;
			}

		private:
			void nameprepAndSetComponents(const String& node, const String& domain, const String& resource);
			void initializeFromString(const String&);
	
		private:
			String node_;
			String domain_;
			bool hasResource_;
			String resource_;
	};
}
