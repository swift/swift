/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleFileTransferFileInfo.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class SWIFTEN_API JingleFileTransferFileInfoParser : public GenericPayloadParser<JingleFileTransferFileInfo> {
    public:
        JingleFileTransferFileInfoParser();

        virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
        virtual void handleEndElement(const std::string& element, const std::string&);
        virtual void handleCharacterData(const std::string& data);

    private:
        int level;
        std::string charData;
        std::string hashAlg;
        boost::optional<boost::uintmax_t> rangeOffset;
};

}
