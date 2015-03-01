/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>

#include <Swiften/FileTransfer/IBBSendTransportSession.h>

namespace Swift {

IBBSendTransportSession::IBBSendTransportSession(boost::shared_ptr<IBBSendSession> session) : session(session) {
	finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
	bytesSentConnection = session->onBytesSent.connect(boost::bind(boost::ref(onBytesSent), _1));
}

IBBSendTransportSession::~IBBSendTransportSession() {
}

 void IBBSendTransportSession::start() {
	session->start();
}

void IBBSendTransportSession::stop() {
	session->stop();
}

}
