/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/optional/optional_fwd.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>

namespace Swift {
	class TransportSession;
	class ErrorPayload;
	class ReadBytestream;
	class WriteBytestream;

	/**
	 * @brief The FileTransferTransporter class is an abstract factory definition
	 * to generate SOCKS5 bytestream transports or IBB bytestreams for use in file
	 * transfers.
	 */
	class SWIFTEN_API FileTransferTransporter {
		public:
			virtual ~FileTransferTransporter();
			
			virtual void startGeneratingLocalCandidates() = 0;
			virtual void stopGeneratingLocalCandidates() = 0;

			virtual void addRemoteCandidates(
					const std::vector<JingleS5BTransportPayload::Candidate>&, const std::string&) = 0;
			virtual void startTryingRemoteCandidates() = 0;
			virtual void stopTryingRemoteCandidates() = 0;

			virtual void startActivatingProxy(const JID& proxy) = 0;
			virtual void stopActivatingProxy() = 0;

			virtual boost::shared_ptr<TransportSession> createIBBSendSession(
					const std::string& sessionID, unsigned int blockSize, boost::shared_ptr<ReadBytestream>) = 0;
			virtual boost::shared_ptr<TransportSession> createIBBReceiveSession(
					const std::string& sessionID, unsigned long long size, boost::shared_ptr<WriteBytestream>) = 0;
			virtual boost::shared_ptr<TransportSession> createRemoteCandidateSession(
					boost::shared_ptr<ReadBytestream>, const JingleS5BTransportPayload::Candidate& candidate) = 0;
			virtual boost::shared_ptr<TransportSession> createRemoteCandidateSession(
					boost::shared_ptr<WriteBytestream>, const JingleS5BTransportPayload::Candidate& candidate) = 0;
			virtual boost::shared_ptr<TransportSession> createLocalCandidateSession(
					boost::shared_ptr<ReadBytestream>, const JingleS5BTransportPayload::Candidate& candidate) = 0;
			virtual boost::shared_ptr<TransportSession> createLocalCandidateSession(
					boost::shared_ptr<WriteBytestream>, const JingleS5BTransportPayload::Candidate& candidate) = 0;

			boost::signal<void (const std::string& /* sessionID */, const std::vector<JingleS5BTransportPayload::Candidate>&, const std::string& /* dstAddr */)> onLocalCandidatesGenerated;
			boost::signal<void (const std::string& /* sessionID */, const boost::optional<JingleS5BTransportPayload::Candidate>&)> onRemoteCandidateSelectFinished;
			boost::signal<void (const std::string& /* sessionID */, boost::shared_ptr<ErrorPayload>)> onProxyActivated;
	};
}
