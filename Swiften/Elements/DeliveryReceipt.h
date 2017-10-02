/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {

class SWIFTEN_API DeliveryReceipt : public Payload {
    public:
        typedef std::shared_ptr<DeliveryReceipt> ref;

    public:
        DeliveryReceipt() {}

        DeliveryReceipt(const std::string& msgId) : receivedID_(msgId) {}

        void setReceivedID(const std::string& msgId) {
            receivedID_ = msgId;
        }

        std::string getReceivedID() const {
            return receivedID_;
        }

    private:
        std::string receivedID_;
};

}
