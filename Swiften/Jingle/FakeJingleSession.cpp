/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Jingle/FakeJingleSession.h>

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

void FakeJingleSession::sendTransportInfo(const JingleContentID& id, JingleTransportPayload::ref payload) {
	calledCommands.push_back(InfoTransportCall(id, payload));
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

}
