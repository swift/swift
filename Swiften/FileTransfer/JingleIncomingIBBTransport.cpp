/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/JingleIncomingIBBTransport.h>

namespace Swift {

JingleIncomingIBBTransport::JingleIncomingIBBTransport(const JID& from, const std::string& id, size_t size, IQRouter* router) : ibbSession(from, id, size, router) {
	ibbSession.onDataReceived.connect(boost::ref(onDataReceived));
}

void JingleIncomingIBBTransport::start() {
	ibbSession.start();
}

void JingleIncomingIBBTransport::stop() {
	ibbSession.stop();
}

}
