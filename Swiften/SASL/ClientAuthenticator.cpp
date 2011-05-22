/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/SASL/ClientAuthenticator.h>

namespace Swift {

ClientAuthenticator::ClientAuthenticator(const std::string& name) : name(name) {
}

ClientAuthenticator::~ClientAuthenticator() {
}

}
