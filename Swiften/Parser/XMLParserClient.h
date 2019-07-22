/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
    class SWIFTEN_API XMLParserClient {
        public:
            virtual ~XMLParserClient();

            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) = 0;
            virtual void handleEndElement(const std::string& element, const std::string& ns) = 0;
            virtual void handleCharacterData(const std::string& data) = 0;

            /**
             * Signal that a namespace prefix has been declared
             * This callback might be called multiple times for a single element,
             * and will trigger before the corresponding \ref handleStartElement
             * is called.
             */
            virtual void handleNamespaceDeclaration(const std::string& prefix, const std::string& uri);
    };
}
