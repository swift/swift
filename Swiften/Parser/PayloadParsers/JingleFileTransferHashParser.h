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
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class SWIFTEN_API JingleFileTransferHashParser : public GenericPayloadParser<JingleFileTransferHash> {
public:
    JingleFileTransferHashParser();

    virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
    virtual void handleEndElement(const std::string& element, const std::string&);
    virtual void handleCharacterData(const std::string& data);

private:
    int level;
    std::shared_ptr<PayloadParser> currentPayloadParser;
};

}
