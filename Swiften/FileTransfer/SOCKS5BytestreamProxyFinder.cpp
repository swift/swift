/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamProxyFinder.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {

SOCKS5BytestreamProxyFinder::SOCKS5BytestreamProxyFinder(const JID& service, IQRouter *iqRouter) : service(service), iqRouter(iqRouter) {
}

SOCKS5BytestreamProxyFinder::~SOCKS5BytestreamProxyFinder() {
}

void SOCKS5BytestreamProxyFinder::start() {
	serviceWalker = boost::make_shared<DiscoServiceWalker>(service, iqRouter);
	serviceWalker->onServiceFound.connect(boost::bind(&SOCKS5BytestreamProxyFinder::handleServiceFound, this, _1, _2));
	serviceWalker->beginWalk();
}

void SOCKS5BytestreamProxyFinder::stop() {
	serviceWalker->endWalk();
	serviceWalker->onServiceFound.disconnect(boost::bind(&SOCKS5BytestreamProxyFinder::handleServiceFound, this, _1, _2));
	serviceWalker.reset();
}

void SOCKS5BytestreamProxyFinder::sendBytestreamQuery(const JID& jid) {
	S5BProxyRequest::ref proxyRequest = boost::make_shared<S5BProxyRequest>();
	boost::shared_ptr<GenericRequest<S5BProxyRequest> > request = boost::make_shared<GenericRequest<S5BProxyRequest> >(IQ::Get, jid, proxyRequest, iqRouter);
	request->onResponse.connect(boost::bind(&SOCKS5BytestreamProxyFinder::handleProxyResponse, this, _1, _2));
	request->send();
}

void SOCKS5BytestreamProxyFinder::handleServiceFound(const JID& jid, boost::shared_ptr<DiscoInfo> discoInfo) {
	if (discoInfo->hasFeature(DiscoInfo::Bytestream)) {
		sendBytestreamQuery(jid);
	}
}

void SOCKS5BytestreamProxyFinder::handleProxyResponse(boost::shared_ptr<S5BProxyRequest> request, ErrorPayload::ref error) {
	if (error) {
		SWIFT_LOG(debug) << "ERROR" << std::endl;
	} else {
		if (request) {
			onProxyFound(request);
		} else {
			//assert(false);
		}
	}
}

}
