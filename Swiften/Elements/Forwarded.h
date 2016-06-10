/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class Delay;
    class Stanza;

    class SWIFTEN_API Forwarded : public Payload {
        public:
            typedef std::shared_ptr<Forwarded> ref;

        public:
            virtual ~Forwarded();

            void setDelay(std::shared_ptr<Delay> delay) { delay_ = delay; }
            const std::shared_ptr<Delay>& getDelay() const { return delay_; }

            void setStanza(std::shared_ptr<Stanza> stanza) { stanza_ = stanza; }
            const std::shared_ptr<Stanza>& getStanza() const { return stanza_; }

        private:
            std::shared_ptr<Delay> delay_;
            std::shared_ptr<Stanza> stanza_;
    };
}
