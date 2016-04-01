/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/CarbonsReceived.h>

namespace Swift {
    CarbonsReceived::~CarbonsReceived() {

    }

    void CarbonsReceived::setForwarded(std::shared_ptr<Forwarded> forwarded) {
        forwarded_ = forwarded;
    }

    std::shared_ptr<Forwarded> CarbonsReceived::getForwarded() const {
        return forwarded_;
    }
}
