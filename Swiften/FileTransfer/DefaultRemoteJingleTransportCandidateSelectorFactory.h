/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>

namespace Swift {

class ConnectionFactory;
class TimerFactory;

class DefaultRemoteJingleTransportCandidateSelectorFactory : public RemoteJingleTransportCandidateSelectorFactory {
public:
	DefaultRemoteJingleTransportCandidateSelectorFactory(ConnectionFactory*, TimerFactory*);
	virtual ~DefaultRemoteJingleTransportCandidateSelectorFactory();

	RemoteJingleTransportCandidateSelector* createCandidateSelector();

private:
	ConnectionFactory* connectionFactory;
	TimerFactory* timerFactory;
};

}
