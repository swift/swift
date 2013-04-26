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

#include <boost/shared_ptr.hpp>

#include <Swiften/JID/JID.h>

namespace Swift {

class JingleSessionManager;
class IQRouter;
class EntityCapsProvider;
class RemoteJingleTransportCandidateSelectorFactory;
class LocalJingleTransportCandidateGeneratorFactory;
class OutgoingFileTransfer;
class JID;
class IDGenerator;
class ReadBytestream;
class StreamInitiationFileInfo;
class SOCKS5BytestreamRegistry;
class SOCKS5BytestreamProxy;
class CryptoProvider;

class OutgoingFileTransferManager {
public:
	OutgoingFileTransferManager(JingleSessionManager* jingleSessionManager, IQRouter* router, EntityCapsProvider* capsProvider, RemoteJingleTransportCandidateSelectorFactory* remoteFactory, LocalJingleTransportCandidateGeneratorFactory* localFactory, SOCKS5BytestreamRegistry* bytestreamRegistry, SOCKS5BytestreamProxy* bytestreamProxy, CryptoProvider* crypto);
	~OutgoingFileTransferManager();
	
	boost::shared_ptr<OutgoingFileTransfer> createOutgoingFileTransfer(const JID& from, const JID& to, boost::shared_ptr<ReadBytestream>, const StreamInitiationFileInfo&);

private:
	JingleSessionManager* jsManager;
	IQRouter* iqRouter;
	EntityCapsProvider* capsProvider;
	RemoteJingleTransportCandidateSelectorFactory* remoteFactory;
	LocalJingleTransportCandidateGeneratorFactory* localFactory;
	IDGenerator *idGenerator;
	SOCKS5BytestreamRegistry* bytestreamRegistry;
	SOCKS5BytestreamProxy* bytestreamProxy;
	CryptoProvider* crypto;
};

}
