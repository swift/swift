/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/JID/JID.h>

namespace Swift {

class SOCKS5BytestreamRegistry;
class SOCKS5BytestreamProxy;
class ConnectivityManager;

class DefaultLocalJingleTransportCandidateGenerator : public LocalJingleTransportCandidateGenerator {
public:
	DefaultLocalJingleTransportCandidateGenerator(ConnectivityManager* connectivityManager, SOCKS5BytestreamRegistry* s5bRegistry, SOCKS5BytestreamProxy* s5bProxy, JID& ownJID);
	virtual ~DefaultLocalJingleTransportCandidateGenerator();

	virtual void generateLocalTransportCandidates(JingleTransportPayload::ref);

	virtual bool isActualCandidate(JingleTransportPayload::ref);
	virtual int getPriority(JingleTransportPayload::ref);
	virtual JingleTransport::ref selectTransport(JingleTransportPayload::ref);

private:
	IDGenerator idGenerator;
	ConnectivityManager* connectivityManager;
	SOCKS5BytestreamRegistry* s5bRegistry;
	SOCKS5BytestreamProxy* s5bProxy;
	JID ownJID;
};

}
