/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/LinkLocalService.h>

namespace Swift {

std::string LinkLocalService::getDescription() const {
    LinkLocalServiceInfo info = getInfo();
    if (!info.getNick().empty()) {
        return info.getNick();
    }
    else if (!info.getFirstName().empty()) {
        std::string result = info.getFirstName();
        if (!info.getLastName().empty()) {
            result += " " + info.getLastName();
        }
        return result;
    }
    else if (!info.getLastName().empty()) {
        return info.getLastName();
    }
    return getName();
}

JID LinkLocalService::getJID() const {
    return JID(getName());
}

}
