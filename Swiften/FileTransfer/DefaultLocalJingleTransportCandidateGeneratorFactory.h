/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/FileTransfer/LocalJingleTransportCandidateGeneratorFactory.h>

#include <Swiften/JID/JID.h>

namespace Swift {

class ConnectivityManager;
class SOCKS5BytestreamRegistry;
class SOCKS5BytestreamProxy;

class DefaultLocalJingleTransportCandidateGeneratorFactory : public LocalJingleTransportCandidateGeneratorFactory{
public:
	DefaultLocalJingleTransportCandidateGeneratorFactory(ConnectivityManager* connectivityManager, SOCKS5BytestreamRegistry* s5bRegistry, SOCKS5BytestreamProxy* s5bProxy, const JID& ownJID);
	virtual ~DefaultLocalJingleTransportCandidateGeneratorFactory();
	
	LocalJingleTransportCandidateGenerator* createCandidateGenerator();

private:
	ConnectivityManager* connectivityManager;
	SOCKS5BytestreamRegistry* s5bRegistry;
	SOCKS5BytestreamProxy* s5bProxy;
	JID ownJID;
};

}
