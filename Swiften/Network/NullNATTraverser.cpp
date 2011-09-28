/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/NullNATTraverser.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

class NullNATTraversalGetPublicIPRequest : public NATTraversalGetPublicIPRequest {
	public:
		NullNATTraversalGetPublicIPRequest(EventLoop* eventLoop) : eventLoop(eventLoop) {
		}

		virtual void run() {
			eventLoop->postEvent(boost::bind(boost::ref(onResult), boost::optional<HostAddress>()));
		}

	private:
		EventLoop* eventLoop;
};

class NullNATTraversalForwardPortRequest : public NATTraversalForwardPortRequest {
	public:
		NullNATTraversalForwardPortRequest(EventLoop* eventLoop) : eventLoop(eventLoop) {
		}

		virtual void run() {
			eventLoop->postEvent(boost::bind(boost::ref(onResult), boost::optional<NATPortMapping>()));
		}

	private:
		EventLoop* eventLoop;
};

class NullNATTraversalRemovePortForwardingRequest : public NATTraversalRemovePortForwardingRequest {
	public:
		NullNATTraversalRemovePortForwardingRequest(EventLoop* eventLoop) : eventLoop(eventLoop) {
		}

		virtual void run() {
			eventLoop->postEvent(boost::bind(boost::ref(onResult), boost::optional<bool>(true)));
		}

	private:
		EventLoop* eventLoop;
};

NullNATTraverser::NullNATTraverser(EventLoop* eventLoop) : eventLoop(eventLoop) {
}

boost::shared_ptr<NATTraversalGetPublicIPRequest> NullNATTraverser::createGetPublicIPRequest() {
	return boost::make_shared<NullNATTraversalGetPublicIPRequest>(eventLoop);
}

boost::shared_ptr<NATTraversalForwardPortRequest> NullNATTraverser::createForwardPortRequest(int, int) {
	return boost::make_shared<NullNATTraversalForwardPortRequest>(eventLoop);
}

boost::shared_ptr<NATTraversalRemovePortForwardingRequest> NullNATTraverser::createRemovePortForwardingRequest(int, int) {
	return boost::make_shared<NullNATTraversalRemovePortForwardingRequest>(eventLoop);
}

}
