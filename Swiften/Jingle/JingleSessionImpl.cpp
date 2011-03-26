/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleSessionImpl.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

JingleSessionImpl::JingleSessionImpl(const JID& initiator, const std::string& id) : JingleSession(initiator, id) {
}

void JingleSessionImpl::handleIncomingAction(JinglePayload::ref) {
}

void JingleSessionImpl::terminate(JinglePayload::Reason::Type reason) {
	JinglePayload::ref payload = boost::make_shared<JinglePayload>(JinglePayload::SessionTerminate, getID());
	payload->setReason(JinglePayload::Reason(reason));
	//onAction(payload)
}

void JingleSessionImpl::acceptTransport(const JingleContentID&, JingleTransportPayload::ref) {

}

void JingleSessionImpl::rejectTransport(const JingleContentID&, JingleTransportPayload::ref) {

}

void JingleSessionImpl::accept(JingleTransportPayload::ref) {
}

void JingleSessionImpl::sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref) {

}



}
