/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/StanzaParser.h>

namespace Swift {

    class PayloadParserFactoryCollection;

    template<typename STANZA_TYPE>
    class SWIFTEN_API GenericStanzaParser : public StanzaParser {
        public:
            GenericStanzaParser(PayloadParserFactoryCollection* collection) :
                        StanzaParser(collection) {
                stanza_ = std::make_shared<STANZA_TYPE>();
            }

            virtual std::shared_ptr<ToplevelElement> getElement() const {
                return stanza_;
            }

            virtual std::shared_ptr<STANZA_TYPE> getStanzaGeneric() const {
                return stanza_;
            }

        private:
            std::shared_ptr<STANZA_TYPE> stanza_;
    };
}
