/*
 * Copyright (c) 2010-2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/SASL/ClientAuthenticator.h>

namespace Swift {

ClientAuthenticator::ClientAuthenticator(const std::string& name) : name(name) {
}

ClientAuthenticator::~ClientAuthenticator() {
}

}
