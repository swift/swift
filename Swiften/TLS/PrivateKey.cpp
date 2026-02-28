/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/PrivateKey.h>

namespace Swift {

PrivateKey::PrivateKey(const SafeByteArray& data, boost::optional<SafeByteArray> password) : data_(data), password_(password) {
}

const SafeByteArray& PrivateKey::getData() const {
    return data_;
}

const boost::optional<SafeByteArray>& PrivateKey::getPassword() const {
    return password_;
}

}
