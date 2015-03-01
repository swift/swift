/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Jingle/FakeJingleSession.h>
#include <Swiften/Jingle/JingleSessionListener.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

FakeJingleSession::FakeJingleSession(const JID& initiator, const std::string& id) : JingleSession(initiator, id) {

}

FakeJingleSession::~FakeJingleSession() {
}

void FakeJingleSession::sendInitiate(const JingleContentID& id, JingleDescription::ref desc, JingleTransportPayload::ref payload) {
	calledCommands.push_back(InitiateCall(id, desc, payload));
}

void FakeJingleSession::sendTerminate(JinglePayload::Reason::Type reason) {
	calledCommands.push_back(TerminateCall(reason));
}

void FakeJingleSession::sendInfo(boost::shared_ptr<Payload> payload) {
	calledCommands.push_back(InfoCall(payload));
}

void FakeJingleSession::sendAccept(const JingleContentID& id, JingleDescription::ref desc, JingleTransportPayload::ref payload) {
	calledCommands.push_back(AcceptCall(id, desc, payload));
}

std::string FakeJingleSession::sendTransportInfo(const JingleContentID& id, JingleTransportPayload::ref payload) {
	calledCommands.push_back(InfoTransportCall(id, payload));
	return idGenerator.generateID();
}

void FakeJingleSession::sendTransportAccept(const JingleContentID& id, JingleTransportPayload::ref payload) {
	calledCommands.push_back(AcceptTransportCall(id, payload));
}

void FakeJingleSession::sendTransportReject(const JingleContentID& id, JingleTransportPayload::ref payload) {
	calledCommands.push_back(RejectTransportCall(id, payload));
}

void FakeJingleSession::sendTransportReplace(const JingleContentID& id, JingleTransportPayload::ref payload) {
	calledCommands.push_back(ReplaceTransportCall(id, payload));
}

void FakeJingleSession::handleTransportReplaceReceived(const JingleContentID& contentID, JingleTransportPayload::ref transport) {
	notifyListeners(&JingleSessionListener::handleTransportReplaceReceived, contentID, transport);
}

void FakeJingleSession::handleTransportAcceptReceived(const JingleContentID& contentID, JingleTransportPayload::ref transport) {
	notifyListeners(&JingleSessionListener::handleTransportAcceptReceived, contentID, transport);
}

void FakeJingleSession::handleTransportInfoReceived(const JingleContentID& contentID, JingleTransportPayload::ref transport) {
	notifyListeners(&JingleSessionListener::handleTransportInfoReceived, contentID, transport);
}

void FakeJingleSession::handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason) {
	notifyListeners(&JingleSessionListener::handleSessionTerminateReceived, reason);
}

void FakeJingleSession::handleSessionAcceptReceived(const JingleContentID& contentID, boost::shared_ptr<JingleDescription> desc, boost::shared_ptr<JingleTransportPayload> payload) {
	notifyListeners(&JingleSessionListener::handleSessionAcceptReceived, contentID, desc, payload);
}

}
