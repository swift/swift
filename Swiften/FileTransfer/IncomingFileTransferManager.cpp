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

IncomingFileTransferManager::IncomingFileTransferManager(JingleSessionManager* jingleSessionManager, IQRouter* router) : jingleSessionManager(jingleSessionManager), router(router) {
	jingleSessionManager->addIncomingSessionHandler(this);
}

IncomingFileTransferManager::~IncomingFileTransferManager() {
	jingleSessionManager->removeIncomingSessionHandler(this);
}

bool IncomingFileTransferManager::handleIncomingJingleSession(JingleSession::ref session, const std::vector<JingleContentPayload::ref>& contents) {
	if (JingleContentPayload::ref content = Jingle::getContentWithDescription<JingleFileTransferDescription>(contents)) {
		if (content->getTransport<JingleIBBTransportPayload>() || content->getTransport<JingleS5BTransportPayload>()) {
			RemoteJingleTransportCandidateSelectorFactory* a;
			LocalJingleTransportCandidateGeneratorFactory* b;
			IncomingJingleFileTransfer::ref transfer = boost::make_shared<IncomingJingleFileTransfer>(session, content, a, b, router);
			onIncomingFileTransfer(transfer);
		}
		else {
			session->terminate(JinglePayload::Reason::UnsupportedTransports);
		}
		return true;
	}
	else {
		return false;
	}
}


}
