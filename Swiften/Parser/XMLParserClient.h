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

             /**
             * Client will have to implement only one of the following methods depending on whether
             * he is interested in processing the element prefix or not.
             */
            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
            virtual void handleStartElementPrefix(const std::string& prefix, const std::string& uri, const std::string& name, const std::string& element, const std::string& ns, const AttributeMap& attributes);

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
