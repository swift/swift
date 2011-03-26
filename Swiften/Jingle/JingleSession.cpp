/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleSession.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

JingleSession::JingleSession(const JID& initiator, const std::string& id) : initiator(initiator), id(id) {

}

JingleSession::~JingleSession() {
}

}
