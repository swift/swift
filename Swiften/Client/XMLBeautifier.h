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

#include <memory>
#include <sstream>
#include <stack>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/XMLParserFactory.h>

namespace Swift {

class SWIFTEN_API XMLBeautifier : public XMLParserClient {
public:

    XMLBeautifier(bool indention, bool coloring);

    std::string beautify(const std::string&);
    bool wasReset() const;
    int getLevel() const;

private:
    void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
    void handleEndElement(const std::string& element, const std::string& ns);
    void handleCharacterData(const std::string& data);

private:
    void indent();
    void reset();

private:
    std::string styleTag(const std::string& text) const;
    std::string styleNamespace(const std::string& text) const;
    std::string styleAttribute(const std::string& text) const;
    std::string styleValue(const std::string& text) const;

private:
    const bool doIndention_;
    const bool doColoring_;

    std::unique_ptr<XMLParserFactory> factory_;
    std::unique_ptr<XMLParser> parser_;

    bool wasReset_ = true;
    int intLevel_ = 0;
    bool lastWasStepDown_ = false;
    std::stringstream buffer_;
    std::stack<std::string> parentNSs_;
};
}
