/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/ElementParser.h>

namespace Swift {

    template<typename ElementType>
    class SWIFTEN_API GenericElementParser : public ElementParser {
        public:
            GenericElementParser() {
                stanza_ = std::make_shared<ElementType>();
            }

            virtual std::shared_ptr<ToplevelElement> getElement() const {
                return stanza_;
            }

            virtual std::shared_ptr<ElementType> getElementGeneric() const {
                return stanza_;
            }

        private:
            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
            }

            virtual void handleEndElement(const std::string&, const std::string&) {
            }

            virtual void handleCharacterData(const std::string&) {
            }

        private:
            std::shared_ptr<ElementType> stanza_;
    };
}
