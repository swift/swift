/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/CarbonsSent.h>

namespace Swift {
    CarbonsSent::~CarbonsSent() {

    }

    void CarbonsSent::setForwarded(std::shared_ptr<Forwarded> forwarded) {
        forwarded_ = forwarded;
    }

    std::shared_ptr<Forwarded> CarbonsSent::getForwarded() const {
        return forwarded_;
    }
}
