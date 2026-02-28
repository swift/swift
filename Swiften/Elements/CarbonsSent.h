/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API CarbonsSent : public Payload {
        public:
            typedef std::shared_ptr<CarbonsSent> ref;

        public:
            virtual ~CarbonsSent();
            void setForwarded(std::shared_ptr<Forwarded> forwarded);
            std::shared_ptr<Forwarded> getForwarded() const;

        private:
            std::shared_ptr<Forwarded> forwarded_;
    };
}
