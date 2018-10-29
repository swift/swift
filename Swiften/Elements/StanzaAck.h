/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API StanzaAck : public ToplevelElement {
        public:
            typedef std::shared_ptr<StanzaAck> ref;

            StanzaAck() : valid(false), handledStanzasCount(0) {}
            StanzaAck(unsigned int handledStanzasCount) : valid(true), handledStanzasCount(handledStanzasCount) {}
            virtual ~StanzaAck();

            unsigned int getHandledStanzasCount() const {
                return handledStanzasCount;
            }

            void setHandledStanzasCount(unsigned int i);

            bool isValid() const {
                return valid;
            }

        private:
            bool valid;
            unsigned int handledStanzasCount;
    };
}
