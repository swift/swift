/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <ostream>

namespace Swift {
	class JID {
		public:
			enum CompareType {
				WithResource, WithoutResource
			};

			JID(const std::string& = std::string());
			JID(const char*);
			JID(const std::string& node, const std::string& domain);
			JID(const std::string& node, const std::string& domain, const std::string& resource);

			bool isValid() const {
				return !domain_.empty(); /* FIXME */
			}

			const std::string& getNode() const {
				return node_;
			}
			const std::string& getDomain() const {
				return domain_;
			}
			const std::string& getResource() const {
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

			std::string toString() const;

			bool equals(const JID& o, CompareType compareType) const {
				return compare(o, compareType) == 0;
			}

			int compare(const JID& o, CompareType compareType) const;

			operator std::string() const {
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
			void nameprepAndSetComponents(const std::string& node, const std::string& domain, const std::string& resource);
			void initializeFromString(const std::string&);
	
		private:
			std::string node_;
			std::string domain_;
			bool hasResource_;
			std::string resource_;
	};
}
