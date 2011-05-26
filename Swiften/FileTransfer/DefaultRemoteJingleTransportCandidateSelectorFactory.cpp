/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "DefaultRemoteJingleTransportCandidateSelectorFactory.h"

#include <Swiften/FileTransfer/DefaultRemoteJingleTransportCandidateSelector.h>

#include <Swiften/Base/Log.h>

namespace Swift {

DefaultRemoteJingleTransportCandidateSelectorFactory::DefaultRemoteJingleTransportCandidateSelectorFactory(ConnectionFactory* connectionFactory, TimerFactory* timerFactory) : connectionFactory(connectionFactory), timerFactory(timerFactory) {
}

DefaultRemoteJingleTransportCandidateSelectorFactory::~DefaultRemoteJingleTransportCandidateSelectorFactory() {
}

RemoteJingleTransportCandidateSelector* DefaultRemoteJingleTransportCandidateSelectorFactory::createCandidateSelector() {
	return new DefaultRemoteJingleTransportCandidateSelector(connectionFactory, timerFactory);
}

}
