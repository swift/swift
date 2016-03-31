/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamResumed.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
    class SWIFTEN_API StreamResumedParser : public GenericElementParser<StreamResumed> {
        public:
            StreamResumedParser();
            ~StreamResumedParser();

            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
            virtual void handleEndElement(const std::string&, const std::string&);

        private:
            enum Level {
                TopLevel = 0
            };
            int level;
    };
}
