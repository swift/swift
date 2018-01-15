/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/CertificateFactory.h>

#include <cassert>
#include <memory>
#include <sstream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/TLS/PrivateKey.h>

namespace Swift {

CertificateFactory::~CertificateFactory() {
}

std::vector<Certificate::ref> CertificateFactory::createCertificateChain(const ByteArray& /* data */) {
    assert(false);
    return std::vector<Certificate::ref>();
}

PrivateKey::ref CertificateFactory::createPrivateKey(const SafeByteArray& data, boost::optional<SafeByteArray> password) {
    return std::make_shared<PrivateKey>(data, password);
}

}
