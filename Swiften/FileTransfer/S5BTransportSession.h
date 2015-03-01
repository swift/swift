/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {

template <typename T>
class SWIFTEN_API S5BTransportSession : public TransportSession {
	public:
		S5BTransportSession(
				boost::shared_ptr<T> session,
				boost::shared_ptr<ReadBytestream> readStream) :
					session(session),
					readStream(readStream) {
			initialize();
		}

		S5BTransportSession(
				boost::shared_ptr<T> session,
				boost::shared_ptr<WriteBytestream> writeStream) :
					session(session),
					writeStream(writeStream) {
			initialize();
		}

		virtual ~S5BTransportSession() {
		}

		virtual void start() SWIFTEN_OVERRIDE {
			if (readStream) {
				session->startSending(readStream);
			}
			else {
				session->startReceiving(writeStream);
			}
		}

		virtual void stop() SWIFTEN_OVERRIDE {
			session->stop();
		}

	private:
		void initialize() {
			finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
			bytesSentConnection = session->onBytesSent.connect(boost::bind(boost::ref(onBytesSent), _1));
		}

	private:
		boost::shared_ptr<T> session;
		boost::shared_ptr<ReadBytestream> readStream;
		boost::shared_ptr<WriteBytestream> writeStream;

		boost::bsignals::scoped_connection finishedConnection;
		boost::bsignals::scoped_connection bytesSentConnection;
};

}
