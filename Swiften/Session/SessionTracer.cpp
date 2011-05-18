/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Session/SessionTracer.h>

#include <iostream>
#include <boost/bind.hpp>

namespace Swift {

SessionTracer::SessionTracer(boost::shared_ptr<Session> session) : session(session) {
	session->onDataRead.connect(boost::bind(&SessionTracer::printData, this, '<', _1));
	session->onDataWritten.connect(boost::bind(&SessionTracer::printData, this, '>', _1));
}

void SessionTracer::printData(char direction, const SafeByteArray& data) {
	std::cerr << direction << direction << " " << session->getLocalJID() << " ";
	for (unsigned int i = 0; i < 72 - session->getLocalJID().toString().size() - session->getRemoteJID().toString().size(); ++i) {
		std::cerr << direction;
	}
	std::cerr << " " << session->getRemoteJID()<< " " << direction << direction << std::endl;
	std::cerr << byteArrayToString(ByteArray(data.begin(), data.end())) << std::endl;
}

}
