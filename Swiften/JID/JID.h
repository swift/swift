/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <iosfwd>
#include <string>

#include <boost/optional/optional.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class IDNConverter;

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
    class SWIFTEN_API JID {
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
             *
             * Must not be NULL.
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

            /**
             * Get the full JID with the supplied resource.
             */
            JID withResource(const std::string& resource) const {
                JID result(this->getNode(), this->getDomain(), resource);
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

            SWIFTEN_API friend std::ostream& operator<<(std::ostream& os, const Swift::JID& j);

            friend bool operator==(const Swift::JID& a, const Swift::JID& b) {
                return a.compare(b, Swift::JID::WithResource) == 0;
            }

            friend bool operator!=(const Swift::JID& a, const Swift::JID& b) {
                return a.compare(b, Swift::JID::WithResource) != 0;
            }

            /**
             * Returns an empty optional if the JID is invalid, and an
             * optional with a value if the JID is valid.
             */
            static boost::optional<JID> parse(const std::string&);

            /**
             * If Swiften was compiled with SWIFTEN_JID_NO_DEFAULT_IDN_CONVERTER (not default), use this method at
             * the beginning of the program to set an IDN converter to use for JID IDN conversions.
             * By default, this method shouldn't be used.
             */
            static void setIDNConverter(IDNConverter*);

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

    SWIFTEN_API std::ostream& operator<<(std::ostream& os, const Swift::JID& j);
}
