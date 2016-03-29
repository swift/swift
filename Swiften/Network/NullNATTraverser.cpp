/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/NullNATTraverser.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>

namespace Swift {

class NullNATTraversalGetPublicIPRequest : public NATTraversalGetPublicIPRequest {
	public:
		NullNATTraversalGetPublicIPRequest(EventLoop* eventLoop) : eventLoop(eventLoop) {
		}

		virtual void start() {
			eventLoop->postEvent(boost::bind(boost::ref(onResult), boost::optional<HostAddress>()));
		}

		virtual void stop() {
		}

	private:
		EventLoop* eventLoop;
};

class NullNATTraversalForwardPortRequest : public NATTraversalForwardPortRequest {
	public:
		NullNATTraversalForwardPortRequest(EventLoop* eventLoop) : eventLoop(eventLoop) {
		}

		virtual void start() {
			eventLoop->postEvent(boost::bind(boost::ref(onResult), boost::optional<NATPortMapping>()));
		}

		virtual void stop() {
		}

	private:
		EventLoop* eventLoop;
};

class NullNATTraversalRemovePortForwardingRequest : public NATTraversalRemovePortForwardingRequest {
	public:
		NullNATTraversalRemovePortForwardingRequest(EventLoop* eventLoop) : eventLoop(eventLoop) {
		}

		virtual void start() {
			eventLoop->postEvent(boost::bind(boost::ref(onResult), boost::optional<bool>(true)));
		}

		virtual void stop() {
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
