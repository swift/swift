/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
//#include <iosfwd>
#include <iostream>

namespace Swift {
	/**
	 * This represents the JID used in XMPP
	 * (RFC6120 - http://tools.ietf.org/html/rfc6120 ).
	 * For a description of format, see the RFC or page 14 of
	 * XMPP: The Definitive Guide (Saint-Andre et al.)
	 *
	 * Particularly - a Bare JID is a JID without a resource part.
	 *
	 * A JID can be invalid (when isValid() returns false). No member methods are 
	 * guaranteed to work correctly if they do.
	 */
	class JID {
		public:
			enum CompareType {
				WithResource, WithoutResource
			};

			/**
			 * Create a JID from its String representation.
			 *
			 * e.g.
			 * wonderland.lit
			 * wonderland.lit/rabbithole
			 * alice@wonderland.lit
			 * alice@wonderland.lit/TeaParty
			 *
			 * @param jid String representation. Invalid JID if empty or invalid.
			 */
			JID(const std::string& jid = std::string());

			/**
			 * See std::string constructor.
			 */
			JID(const char*);

			/**
			 * Create a bare JID from the node and domain parts.
			 *
			 * JID("node@domain") == JID("node", "domain")
			 * unless you pass in empty values.
			 *
			 * @param node JID node part.
			 * @param domain JID domain part.
			 */
			JID(const std::string& node, const std::string& domain);
			/**
			 * Create a bare JID from the node, domain and resource parts.
			 *
			 * JID("node@domain/resource") == JID("node", "domain", "resource")
			 * unless you pass in empty values.
			 *
			 * @param node JID node part.
			 * @param domain JID domain part.
			 * @param resource JID resource part.
			 */
			JID(const std::string& node, const std::string& domain, const std::string& resource);

			/**
			 * @return Is a correctly-formatted JID.
			 */
			bool isValid() const {
				return valid_;
			}

			/**
			 * e.g. JID("node@domain").getNode() == "node"
			 * @return could be empty.
			 */
			const std::string& getNode() const {
				return node_;
			}

			/**
			 * e.g. JID("node@domain").getDomain() == "domain"
			 */
			const std::string& getDomain() const {
				return domain_;
			}

			/**
			 * e.g. JID("node@domain/resource").getResource() == "resource"
			 * @return could be empty.
			 */
			const std::string& getResource() const {
				return resource_;
			}

			/**
			 * Is a bare JID, i.e. has no resource part.
			 */
			bool isBare() const {
				return !hasResource_;
			}

			/**
			 * Returns the given node, escaped according to XEP-0106.
			 * The resulting node is a valid node for a JID, whereas the input value can contain characters
			 * that are not allowed.
			 */
			static std::string getEscapedNode(const std::string& node);

			/**
			 * Returns the node of the current JID, unescaped according to XEP-0106.
			 */
			std::string getUnescapedNode() const;

			/**
			 * Get the JID without a resource.
			 * @return Invalid if the original is invalid.
			 */
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
			bool valid_;
			std::string node_;
			std::string domain_;
			bool hasResource_;
			std::string resource_;
	};
}
