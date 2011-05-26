/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "DefaultLocalJingleTransportCandidateGeneratorFactory.h"

#include <Swiften/FileTransfer/DefaultLocalJingleTransportCandidateGenerator.h>
#include <Swiften/Base/Log.h>

namespace Swift {

DefaultLocalJingleTransportCandidateGeneratorFactory::DefaultLocalJingleTransportCandidateGeneratorFactory(ConnectivityManager* connectivityManager, SOCKS5BytestreamRegistry* s5bRegistry, SOCKS5BytestreamProxy* s5bProxy, const JID& ownJID) : connectivityManager(connectivityManager), s5bRegistry(s5bRegistry), s5bProxy(s5bProxy), ownJID(ownJID) {
}

DefaultLocalJingleTransportCandidateGeneratorFactory::~DefaultLocalJingleTransportCandidateGeneratorFactory() {
}

LocalJingleTransportCandidateGenerator* DefaultLocalJingleTransportCandidateGeneratorFactory::createCandidateGenerator() {
	return new DefaultLocalJingleTransportCandidateGenerator(connectivityManager, s5bRegistry, s5bProxy, ownJID);
}


}
