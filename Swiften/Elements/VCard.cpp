/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/VCard.h>

namespace Swift {

VCard::EMailAddress VCard::getPreferredEMailAddress() const {
    for (const auto& address : emailAddresses_) {
        if (address.isPreferred) {
            return address;
        }
    }
    if (!emailAddresses_.empty()) {
        return emailAddresses_[0];
    }
    return EMailAddress();
}


}
