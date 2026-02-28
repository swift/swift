/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API IsodeIQDelegation : public Payload {
        public:

            IsodeIQDelegation();

            virtual ~IsodeIQDelegation();

            std::shared_ptr<Forwarded> getForward() const {
                return forward;
            }

            void setForward(std::shared_ptr<Forwarded> value) {
                this->forward = value ;
            }


        private:
            std::shared_ptr<Forwarded> forward;
    };
}
