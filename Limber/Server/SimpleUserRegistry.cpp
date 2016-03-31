/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Limber/Server/SimpleUserRegistry.h>

namespace Swift {

SimpleUserRegistry::SimpleUserRegistry() {
}

bool SimpleUserRegistry::isValidUserPassword(const JID& user, const SafeByteArray& password) const {
    std::map<JID,SafeByteArray>::const_iterator i = users.find(user);
    return i != users.end() ? i->second == password : false;
}

void SimpleUserRegistry::addUser(const JID& user, const std::string& password) {
    users.insert(std::make_pair(user, createSafeByteArray(password)));
}

}
