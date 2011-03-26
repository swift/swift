/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/JingleIncomingIBBTransport.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>

namespace Swift {

IncomingJingleFileTransfer::IncomingJingleFileTransfer(
		JingleSession::ref session,
		JingleContentPayload::ref content,
		RemoteJingleTransportCandidateSelectorFactory* candidateSelectorFactory,
		LocalJingleTransportCandidateGeneratorFactory* candidateGeneratorFactory,
		IQRouter* router) :
			session(session),
			router(router),
			initialContent(content),
			contentID(content->getName(), content->getCreator()),
			state(Initial),
			remoteTransportCandidateSelectFinished(false),
			localTransportCandidateSelectFinished(false) {
	
	candidateSelector = candidateSelectorFactory->createCandidateSelector();
	candidateSelector->onRemoteTransportCandidateSelectFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished, this, _1));

	candidateGenerator = candidateGeneratorFactory->createCandidateGenerator();
	candidateGenerator->onLocalTransportCandidatesGenerated.connect(boost::bind(&IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated, this, _1));

	session->onTransportInfoReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleTransportInfoReceived, this, _1, _2));
	session->onTransportReplaceReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleTransportReplaceReceived, this, _1, _2));
	session->onSessionTerminateReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleSessionTerminateReceived, this));

	description = initialContent->getDescription<JingleFileTransferDescription>();
	assert(description);
}

IncomingJingleFileTransfer::~IncomingJingleFileTransfer() {
	session->onSessionTerminateReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleSessionTerminateReceived, this));
	session->onTransportReplaceReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleTransportReplaceReceived, this, _1, _2));
	session->onTransportInfoReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleTransportInfoReceived, this, _1, _2));

	candidateGenerator->onLocalTransportCandidatesGenerated.disconnect(boost::bind(&IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated, this, _1));
	delete candidateGenerator;

	candidateSelector->onRemoteTransportCandidateSelectFinished.disconnect(boost::bind(&IncomingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished, this, _1));
	delete candidateSelector;
}

void IncomingJingleFileTransfer::accept(WriteBytestream::ref stream) {
	assert(!stream);
	this->stream = stream;

	if (JingleIBBTransportPayload::ref ibbTransport = initialContent->getTransport<JingleIBBTransportPayload>()) {
		setActiveTransport(createIBBTransport(ibbTransport));
		session->accept();
	}
	else if (JingleS5BTransportPayload::ref s5bTransport = initialContent->getTransport<JingleS5BTransportPayload>()) {
		state = CreatingInitialTransports;
		candidateSelector->addRemoteTransportCandidates(s5bTransport);
		candidateGenerator->generateLocalTransportCandidates();
	}
	else {
		assert(false);
	}
}


void IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated(JingleTransportPayload::ref candidates) {
	if (state == CreatingInitialTransports) {
		if (!candidates) {
			localTransportCandidateSelectFinished = true;
		}
		session->accept(candidates);
		state = NegotiatingTransport;
		candidateSelector->selectCandidate();
	}
}


void IncomingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished(JingleTransportPayload::ref transport) {
	remoteTransportCandidateSelectFinished = true;
	selectedRemoteTransportCandidate = transport;
	session->sendTransportInfo(contentID, transport);
	checkCandidateSelected();
}

void IncomingJingleFileTransfer::checkCandidateSelected() {
	if (localTransportCandidateSelectFinished && remoteTransportCandidateSelectFinished) {
		if (candidateGenerator->isActualCandidate(selectedLocalTransportCandidate) && candidateSelector->isActualCandidate(selectedRemoteTransportCandidate)) {
			if (candidateGenerator->getPriority(selectedLocalTransportCandidate) > candidateSelector->getPriority(selectedRemoteTransportCandidate)) {
				setActiveTransport(candidateGenerator->selectTransport(selectedLocalTransportCandidate));
			}
			else {
				setActiveTransport(candidateSelector->selectTransport(selectedRemoteTransportCandidate));
			}
		}
		else if (candidateSelector->isActualCandidate(selectedRemoteTransportCandidate)) {
			setActiveTransport(candidateSelector->selectTransport(selectedRemoteTransportCandidate));
		}
		else if (candidateGenerator->isActualCandidate(selectedLocalTransportCandidate)) {
			setActiveTransport(candidateGenerator->selectTransport(selectedLocalTransportCandidate));
		}
		else {
			state = WaitingForFallbackOrTerminate;
		}
	}
}

void IncomingJingleFileTransfer::setActiveTransport(JingleTransport::ref transport) {
	state = Transferring;
	activeTransport = transport;
	activeTransport->onDataReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleTransportDataReceived, this, _1));
	activeTransport->start();
}

void IncomingJingleFileTransfer::handleSessionTerminateReceived() {
	// TODO
	state = Terminated;
}

void IncomingJingleFileTransfer::handleTransportDataReceived(const std::vector<unsigned char>& data) {
	stream->write(data);
}


void IncomingJingleFileTransfer::handleTransportInfoReceived(const JingleContentID&, JingleTransportPayload::ref transport) {
	localTransportCandidateSelectFinished = true;
	selectedLocalTransportCandidate = transport;
	if (candidateGenerator->isActualCandidate(transport)) {
		candidateSelector->setMinimumPriority(candidateGenerator->getPriority(transport));
	}
	checkCandidateSelected();
}

void IncomingJingleFileTransfer::handleTransportReplaceReceived(const JingleContentID& content, JingleTransportPayload::ref transport) {
	if (JingleIBBTransportPayload::ref ibbTransport = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transport)) {
		setActiveTransport(createIBBTransport(ibbTransport));
		session->acceptTransport(content, transport);
	}
	else {
		session->rejectTransport(content, transport);
	}
}

void IncomingJingleFileTransfer::stopActiveTransport() {
	if (activeTransport) {
		activeTransport->stop();
		activeTransport->onDataReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleTransportDataReceived, this, _1));
	}
}

JingleIncomingIBBTransport::ref IncomingJingleFileTransfer::createIBBTransport(JingleIBBTransportPayload::ref ibbTransport) {
	return boost::make_shared<JingleIncomingIBBTransport>(session->getInitiator(), ibbTransport->getSessionID(), description->getOffer()->size, router);
}

}
