/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>

#include <Swiften/FileTransfer/IBBReceiveTransportSession.h>

namespace Swift {

IBBReceiveTransportSession::IBBReceiveTransportSession(boost::shared_ptr<IBBReceiveSession> session) : session(session) {
	finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
}

IBBReceiveTransportSession::~IBBReceiveTransportSession() {
}

void IBBReceiveTransportSession::start() {
	session->start();
}

void IBBReceiveTransportSession::stop() {
	session->stop();
}

}
