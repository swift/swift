/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Parser/PayloadParser.h>

namespace Swift {

class PayloadParserFactoryCollection;

class SWIFTEN_API JingleFileTransferDescriptionParser : public GenericPayloadParser<JingleFileTransferDescription> {
    public:
        JingleFileTransferDescriptionParser(PayloadParserFactoryCollection* factories);

        virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
        virtual void handleEndElement(const std::string& element, const std::string&);
        virtual void handleCharacterData(const std::string& data);

    private:
        PayloadParserFactoryCollection* factories;
        int level;
        std::shared_ptr<PayloadParser> currentPayloadParser;
};

}
