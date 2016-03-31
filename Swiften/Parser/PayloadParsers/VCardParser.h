/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SerializingParser;

    class SWIFTEN_API VCardParser : public GenericPayloadParser<VCard> {
        public:
            VCardParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            std::string getElementHierarchy() const;

        private:
            std::vector<std::string> elementStack_;
            VCard::EMailAddress currentEMailAddress_;
            VCard::Telephone currentTelephone_;
            VCard::Address currentAddress_;
            VCard::AddressLabel currentAddressLabel_;
            VCard::Organization currentOrganization_;
            SerializingParser* unknownContentParser_;
            std::string currentText_;
    };
}
