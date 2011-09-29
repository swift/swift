/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Jingle/JingleSession.h>
#include <Swiften/StringCodecs/SHA1.h>

namespace Swift {

class RemoteJingleTransportCandidateSelectorFactory;
class RemoteJingleTransportCandidateSelector;
class LocalJingleTransportCandidateGeneratorFactory;
class LocalJingleTransportCandidateGenerator;
class IQRouter;
class ReadBytestream;
class IBBSendSession;
class IDGenerator;
class IncrementalBytestreamHashCalculator;
class SOCKS5BytestreamRegistry;
class SOCKS5BytestreamProxy;

class OutgoingJingleFileTransfer : public OutgoingFileTransfer {
public:
	OutgoingJingleFileTransfer(JingleSession::ref,
					RemoteJingleTransportCandidateSelectorFactory*,
					LocalJingleTransportCandidateGeneratorFactory*,
					IQRouter*,
					IDGenerator*,
					const JID& from,
					const JID& to,
					boost::shared_ptr<ReadBytestream>,
					const StreamInitiationFileInfo&,
					SOCKS5BytestreamRegistry*,
					SOCKS5BytestreamProxy*);
	virtual ~OutgoingJingleFileTransfer();
	
	void start();
	void stop();

	void cancel();

private:
	void handleSessionAcceptReceived(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref);
	void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason);
	void handleTransportAcceptReceived(const JingleContentID&, JingleTransportPayload::ref);
	void handleTransportInfoReceived(const JingleContentID&, JingleTransportPayload::ref);

	void handleLocalTransportCandidatesGenerated(JingleTransportPayload::ref);
	void handleRemoteTransportCandidateSelectFinished(JingleTransportPayload::ref);

private:
	void replaceTransportWithIBB(const JingleContentID&);
	void handleTransferFinished(boost::optional<FileTransferError>);
	void activateProxySession(const JID &proxy);
	void handleActivateProxySessionResult(boost::shared_ptr<S5BProxyRequest> request, ErrorPayload::ref error);
	void proxySessionReady(const JID& proxy, bool error);

private:
	typedef std::map<std::string, JingleS5BTransportPayload::Candidate> CandidateMap;

private:
	void startTransferViaOurCandidateChoice(JingleS5BTransportPayload::Candidate);
	void startTransferViaTheirCandidateChoice(JingleS5BTransportPayload::Candidate);
	void decideOnCandidates();
	void fillCandidateMap(CandidateMap& map, JingleS5BTransportPayload::ref s5bPayload);

private:
	void sendSessionInfoHash();

private:
	JingleSession::ref session;
	RemoteJingleTransportCandidateSelector* remoteCandidateSelector;
	RemoteJingleTransportCandidateSelectorFactory* remoteFactory;
	LocalJingleTransportCandidateGenerator* localCandidateGenerator;
	LocalJingleTransportCandidateGeneratorFactory* localFactory;

	IQRouter* router;
	IDGenerator* idGenerator;
	JID fromJID;
	JID toJID;
	boost::shared_ptr<ReadBytestream> readStream;
	StreamInitiationFileInfo fileInfo;
	IncrementalBytestreamHashCalculator *hashCalculator;

	boost::shared_ptr<IBBSendSession> ibbSession;

	JingleS5BTransportPayload::ref ourCandidateChoice;
	JingleS5BTransportPayload::ref theirCandidateChoice;
	CandidateMap ourCandidates;
	CandidateMap theirCandidates;

	SOCKS5BytestreamRegistry* s5bRegistry;
	SOCKS5BytestreamProxy* s5bProxy;
	SOCKS5BytestreamClientSession::ref clientSession;
	SOCKS5BytestreamServerSession* serverSession;
	JingleContentID contentID;
	std::string s5bSessionID;

	bool canceled;
};

}
