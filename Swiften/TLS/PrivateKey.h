/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/PrivateKey.h>

namespace Swift {

class PrivateKey {
public:
    using ref = std::shared_ptr<PrivateKey>;

public:
    PrivateKey(const SafeByteArray& data, boost::optional<SafeByteArray> password = boost::optional<SafeByteArray>());

    const SafeByteArray& getData() const;
    const boost::optional<SafeByteArray>& getPassword() const;

private:
    SafeByteArray data_;
    boost::optional<SafeByteArray> password_;
};

}
