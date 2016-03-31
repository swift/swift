/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/StanzaParser.h>

namespace Swift {

    class PayloadParserFactoryCollection;

    template<typename STANZA_TYPE>
    class SWIFTEN_API GenericStanzaParser : public StanzaParser {
        public:
            GenericStanzaParser(PayloadParserFactoryCollection* collection) :
                        StanzaParser(collection) {
                stanza_ = boost::make_shared<STANZA_TYPE>();
            }

            virtual boost::shared_ptr<ToplevelElement> getElement() const {
                return stanza_;
            }

            virtual boost::shared_ptr<STANZA_TYPE> getStanzaGeneric() const {
                return stanza_;
            }

        private:
            boost::shared_ptr<STANZA_TYPE> stanza_;
    };
}
