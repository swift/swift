/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class SWIFTEN_API S5BProxyRequestParser : public GenericPayloadParser<S5BProxyRequest> {
public:
    S5BProxyRequestParser();
    virtual ~S5BProxyRequestParser();

    virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
    virtual void handleEndElement(const std::string& element, const std::string&);
    virtual void handleCharacterData(const std::string& data);

private:
    bool parseActivate;
    std::string activateJID;
};

}
