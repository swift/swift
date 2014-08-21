/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IncomingFileTransferManager.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Jingle/Jingle.h>
#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>

namespace Swift {

IncomingFileTransferManager::IncomingFileTransferManager(
		JingleSessionManager* jingleSessionManager,
		IQRouter* router,
		FileTransferTransporterFactory* transporterFactory,
		TimerFactory* timerFactory, 
		CryptoProvider* crypto) : 
			jingleSessionManager(jingleSessionManager), 
			router(router), 
			transporterFactory(transporterFactory),
			timerFactory(timerFactory), 
			crypto(crypto) {
	jingleSessionManager->addIncomingSessionHandler(this);
}

IncomingFileTransferManager::~IncomingFileTransferManager() {
	jingleSessionManager->removeIncomingSessionHandler(this);
}

bool IncomingFileTransferManager::handleIncomingJingleSession(
		JingleSession::ref session, 
		const std::vector<JingleContentPayload::ref>& contents, 
		const JID& recipient) {
	if (JingleContentPayload::ref content = Jingle::getContentWithDescription<JingleFileTransferDescription>(contents)) {
		if (content->getTransport<JingleS5BTransportPayload>()) {
			JingleFileTransferDescription::ref description = content->getDescription<JingleFileTransferDescription>();
			if (description && description->getOffers().size() == 1) {
				IncomingJingleFileTransfer::ref transfer = boost::make_shared<IncomingJingleFileTransfer>(
						recipient, session, content, transporterFactory, timerFactory, crypto);
				onIncomingFileTransfer(transfer);
			} 
			else {
				SWIFT_LOG(warning) << "Received a file-transfer request with no description or more than one file.";
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
