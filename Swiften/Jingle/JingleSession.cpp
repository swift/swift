/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleSession.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

JingleSession::JingleSession(const std::string& id, const std::vector<JingleContent::ref>& contents) : id(id), contents(contents) {

}

JingleSession::~JingleSession() {
}

void JingleSession::handleIncomingAction(JinglePayload::ref) {
}

void JingleSession::terminate(JinglePayload::Reason::Type reason) {
	JinglePayload::ref payload = boost::make_shared<JinglePayload>(JinglePayload::SessionTerminate, id);
	payload->setReason(JinglePayload::Reason(reason));
	//onAction(payload)
}


}
