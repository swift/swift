/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include "DefaultRemoteJingleTransportCandidateSelectorFactory.h"

#include <Swiften/FileTransfer/DefaultRemoteJingleTransportCandidateSelector.h>

#include <Swiften/Base/Log.h>

namespace Swift {

DefaultRemoteJingleTransportCandidateSelectorFactory::DefaultRemoteJingleTransportCandidateSelectorFactory(ConnectionFactory* connectionFactory, TimerFactory* timerFactory, CryptoProvider* crypto) : connectionFactory(connectionFactory), timerFactory(timerFactory), crypto(crypto) {
}

DefaultRemoteJingleTransportCandidateSelectorFactory::~DefaultRemoteJingleTransportCandidateSelectorFactory() {
}

RemoteJingleTransportCandidateSelector* DefaultRemoteJingleTransportCandidateSelectorFactory::createCandidateSelector() {
	return new DefaultRemoteJingleTransportCandidateSelector(connectionFactory, timerFactory, crypto);
}

}
