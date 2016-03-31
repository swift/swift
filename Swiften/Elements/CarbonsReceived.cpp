/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/CarbonsReceived.h>

namespace Swift {
    CarbonsReceived::~CarbonsReceived() {

    }

    void CarbonsReceived::setForwarded(boost::shared_ptr<Forwarded> forwarded) {
        forwarded_ = forwarded;
    }

    boost::shared_ptr<Forwarded> CarbonsReceived::getForwarded() const {
        return forwarded_;
    }
}
