/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <queue>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>


namespace Swift {

class ConnectionFactory;
class TimerFactory;

class DefaultRemoteJingleTransportCandidateSelector : public RemoteJingleTransportCandidateSelector {
public:
	DefaultRemoteJingleTransportCandidateSelector(ConnectionFactory*, TimerFactory*);
	virtual ~DefaultRemoteJingleTransportCandidateSelector();

	virtual void addRemoteTransportCandidates(JingleTransportPayload::ref);
	virtual void selectCandidate();
	virtual void setMinimumPriority(int);
	void setRequesterTargtet(const JID& requester, const JID& target);
	virtual SOCKS5BytestreamClientSession::ref getS5BSession();

	virtual bool isActualCandidate(JingleTransportPayload::ref);
	virtual int getPriority(JingleTransportPayload::ref);
	virtual JingleTransport::ref selectTransport(JingleTransportPayload::ref);

private:
	void tryNextCandidate(bool error);

private:
	ConnectionFactory* connectionFactory;
	TimerFactory* timerFactory;

	std::priority_queue<JingleS5BTransportPayload::Candidate, std::vector<JingleS5BTransportPayload::Candidate>, JingleS5BTransportPayload::CompareCandidate> candidates;

	std::string transportSID;
	boost::shared_ptr<Connection> connection;
	boost::shared_ptr<SOCKS5BytestreamClientSession> s5bSession;
	JingleS5BTransportPayload::Candidate lastCandidate;
	JID requester;
	JID target;
};

}
