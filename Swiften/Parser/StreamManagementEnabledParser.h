/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
    class SWIFTEN_API StreamManagementEnabledParser : public GenericElementParser<StreamManagementEnabled> {
        public:
            StreamManagementEnabledParser();
            ~StreamManagementEnabledParser();

            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
            virtual void handleEndElement(const std::string&, const std::string&);

        private:
            enum Level {
                TopLevel = 0
            };
            int level;
    };
}
