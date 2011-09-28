/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IncomingFileTransferManager.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Jingle/Jingle.h>
#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>

namespace Swift {

IncomingFileTransferManager::IncomingFileTransferManager(JingleSessionManager* jingleSessionManager, IQRouter* router,
							RemoteJingleTransportCandidateSelectorFactory* remoteFactory,
														 LocalJingleTransportCandidateGeneratorFactory* localFactory, SOCKS5BytestreamRegistry* bytestreamRegistry, SOCKS5BytestreamProxy* bytestreamProxy, TimerFactory* timerFactory) : jingleSessionManager(jingleSessionManager), router(router), remoteFactory(remoteFactory), localFactory(localFactory), bytestreamRegistry(bytestreamRegistry), bytestreamProxy(bytestreamProxy), timerFactory(timerFactory) {
	jingleSessionManager->addIncomingSessionHandler(this);
}

IncomingFileTransferManager::~IncomingFileTransferManager() {
	jingleSessionManager->removeIncomingSessionHandler(this);
}

bool IncomingFileTransferManager::handleIncomingJingleSession(JingleSession::ref session, const std::vector<JingleContentPayload::ref>& contents, const JID& recipient) {
	if (JingleContentPayload::ref content = Jingle::getContentWithDescription<JingleFileTransferDescription>(contents)) {
		if (content->getTransport<JingleIBBTransportPayload>() || content->getTransport<JingleS5BTransportPayload>()) {

			JingleFileTransferDescription::ref description = content->getDescription<JingleFileTransferDescription>();

			if (description && description->getOffers().size() == 1) {
				IncomingJingleFileTransfer::ref transfer = boost::make_shared<IncomingJingleFileTransfer>(recipient, session, content, remoteFactory, localFactory, router, bytestreamRegistry, bytestreamProxy, timerFactory);
				onIncomingFileTransfer(transfer);
			} else {
				std::cerr << "Received a file-transfer request with no description or more than one file!" << std::endl;
				session->sendTerminate(JinglePayload::Reason::FailedApplication);
			}
		}
		else {
			session->sendTerminate(JinglePayload::Reason::UnsupportedTransports);
		}
		return true;
	}
	else {
		return false;
	}
}


}
