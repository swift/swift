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

#pragma once

#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>

namespace Swift {

class ConnectionFactory;
class TimerFactory;
class CryptoProvider;

class DefaultRemoteJingleTransportCandidateSelectorFactory : public RemoteJingleTransportCandidateSelectorFactory {
public:
	DefaultRemoteJingleTransportCandidateSelectorFactory(ConnectionFactory*, TimerFactory*, CryptoProvider*);
	virtual ~DefaultRemoteJingleTransportCandidateSelectorFactory();

	RemoteJingleTransportCandidateSelector* createCandidateSelector();

private:
	ConnectionFactory* connectionFactory;
	TimerFactory* timerFactory;
	CryptoProvider* crypto;
};

}
